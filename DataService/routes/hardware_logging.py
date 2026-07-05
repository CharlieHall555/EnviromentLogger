from flask import Blueprint, request, jsonify
from datetime import datetime, timezone
import math
from typing import Any
import traceback

import database_connection
from psycopg2.extras import Json
from pydantic import BaseModel, Field, ValidationError
from config import Config

config = Config()


class HardwareLogAddBody(BaseModel):
    sent_at: int = Field(ge=0)  # unix time
    severity: str
    event: str
    metadata: dict[str, Any] | None = None


class HardwareLog(BaseModel):
    id: int
    sent_at: datetime
    severity: str
    event: str
    metadata: dict[str, Any] | None = None


hardware_log_bp = Blueprint("hardware_log", __name__, url_prefix="/hardware_log")


def error_response(message: str, status_code: int = 500):
    return jsonify({"result": "error", "message": message}), status_code


@hardware_log_bp.route("/add", methods=["POST"])
def add_hardware_log():
    data = request.get_json(silent=True)

    if data is None:
        return error_response("Request body must be valid JSON", 400)

    auth_header = request.headers.get("Authorization")

    if not auth_header:
        return jsonify({"error": "Missing Authorization header"}), 401

    if auth_header != config.api_token:
        return jsonify({"error": "Unauthorized"}), 401

    try:
        log_entry = HardwareLogAddBody.model_validate(data)
    except ValidationError:
        return error_response("Request data is invalid", 400)

    try:
        sent_at_formatted = datetime.fromtimestamp(log_entry.sent_at, timezone.utc)
    except Exception:
        return error_response("timestamp invalid", 400)

    conn = None

    try:
        conn = database_connection.establish_connection()
        with conn.cursor() as cur:
            cur.execute(
                """
                INSERT INTO "HARDWARE_LOGS" (sent_at, severity, event, metadata)
                VALUES (%s, %s, %s, %s)
                RETURNING id, sent_at, severity, event, metadata;
                """,
                (
                    sent_at_formatted,
                    log_entry.severity,
                    log_entry.event,
                    Json(log_entry.metadata) if log_entry.metadata is not None else None,
                ),
            )
            row = cur.fetchone()

        if row is None:
            return error_response("database error", 500)

        conn.commit()

        return jsonify(
            {
                "id": row[0],
                "sent_at": row[1],
                "severity": row[2],
                "event": row[3],
                "metadata": row[4],
            }
        ), 201

    except Exception as e:
        print(e)
        return error_response("database error", 500)

    finally:
        if conn is not None:
            conn.close()


@hardware_log_bp.route("/size", methods=["GET"])
def get_hardware_log_size():
    page_size = request.args.get("page_size", default=10, type=int)

    if page_size is None:
        return error_response("page_size must be an integer", 400)

    page_size = min(max(page_size, 1), 100)

    conn = None

    try:
        conn = database_connection.establish_connection()

        with conn.cursor() as cur:
            cur.execute('SELECT COUNT(*) FROM "HARDWARE_LOGS";')
            total_rows = cur.fetchone()[0]  # type: ignore

        total_pages = math.ceil(total_rows / page_size)

        return jsonify(
            {
                "total_size": total_rows,
                "page_size": page_size,
                "n_pages": total_pages,
            }
        ), 200

    except Exception as error:
        return error_response(str(error), 500)

    finally:
        if conn is not None:
            conn.close()


@hardware_log_bp.route("/page", methods=["GET"])
def get_hardware_log_page():
    page = request.args.get("page", default=1, type=int)
    page_size = request.args.get("page_size", default=10, type=int)

    page = max(page, 1)
    page_size = min(max(page_size, 1), 100)
    offset = (page - 1) * page_size

    conn = None

    try:
        conn = database_connection.establish_connection()

        with conn.cursor() as cur:
            cur.execute(
                """
                SELECT id, sent_at, severity, event, metadata
                FROM "HARDWARE_LOGS"
                ORDER BY id DESC
                LIMIT %s OFFSET %s;
                """,
                (page_size, offset),
            )

            rows = cur.fetchall()

        logs = []
        for row in rows:
            logs.append(
                {
                    "id": row[0],
                    "sent_at": row[1],
                    "severity": row[2],
                    "event": row[3],
                    "metadata": row[4],
                }
            )

        return jsonify({"page": page, "page_size": page_size, "logs": logs}), 200

    except Exception as error:
        return error_response(str(error), 500)

    finally:
        if conn is not None:
            conn.close()

