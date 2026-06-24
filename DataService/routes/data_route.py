from flask import Flask, Blueprint, request, jsonify
from datetime import datetime
import database_connection
from pydantic import BaseModel, Field, ValidationError
import logging
from config import Config
import math

config = Config()

class ReadingCreate(BaseModel):
    temperature: float = Field(ge=-50, le=80)
    humidity: float = Field(ge=0, le=100)
    measured_at: datetime | None = None

class Reading(BaseModel):
    id : int
    temperature: float = Field(ge=-50, le=80)
    humidity: float = Field(ge=0, le=100)
    measured_at: datetime | None = None
    received_at: datetime | None = None

data_bp = Blueprint("data", __name__)

def error_response(message: str, status_code: int = 500):
    response = {
        "result": "error",
        "message": message
    }

    return jsonify(response), status_code


@data_bp.route("/add_reading", methods=["POST"])
def add_reading():
    data = request.get_json(silent=True)

    if data is None:
        return error_response("Request body must be valid JSON", 400)

    auth_header = request.headers.get("Authorization")

    if not auth_header:
        return jsonify({"error": "Missing Authorization header"}), 401

    if auth_header != config.api_token:
        return jsonify({"error": "Unauthorized"}), 401

    try:
        reading = ReadingCreate.model_validate(data)
    except ValidationError as error:
        return error_response(
            "Request data is invalid",
            400
        )

    conn = None

    try:
        conn = database_connection.establish_connection()

    except Exception as error:
        return error_response(
            "database connection failed",
            500
        )

    try:
        cur = conn.cursor()
        cur.execute(
        """
        INSERT INTO "READINGS" (temperature, humidity, measured_at)
        VALUES (%s, %s, %s)
        RETURNING id, measured_at, temperature, humidity, received_at;
        """,
        (reading.temperature, reading.humidity, reading.measured_at)
        )

        conn.commit()
    except Exception as error:
        return error_response(
            "database error",
            500
        )
    finally:
        cur.close()
        conn.close()

    return "done" , 201

@data_bp.route("/size", methods=["GET"])
def get_size():
    page_size = request.args.get("page_size", default=10, type=int)

    if page_size is None:
        return error_response("page_size must be an integer", 400)

    page_size = min(max(page_size, 1), 100)

    try:
        conn = database_connection.establish_connection()

        with conn.cursor() as cur:
            cur.execute('SELECT COUNT(*) FROM "READINGS";')
            total_rows = cur.fetchone()[0] # type: ignore

        total_pages = math.ceil(total_rows / page_size)

        return jsonify({
            "total_size": total_rows,
            "page_size": page_size,
            "n_pages": total_pages
        }), 200

    except Exception as error:
        return error_response(str(error), 500)

    finally:
        if "conn" in locals():
            conn.close()

@data_bp.route("/page", methods=["GET"])
def get_page():
    page = request.args.get("page", default=1, type=int)
    page_size = request.args.get("page_size", default=10, type=int)

    page = max(page, 1)
    page_size = min(max(page_size, 1), 100)

    offset = (page - 1) * page_size

    try:
        conn = database_connection.establish_connection()
    except Exception:
        return error_response("Database connection failed", 500)

    try:
        cur = conn.cursor()

        cur.execute(
            """
            SELECT id, measured_at, temperature, humidity, received_at
            FROM "READINGS"
            ORDER BY received_at DESC, id DESC
            LIMIT %s OFFSET %s;
            """,
            (page_size, offset)
        )

        rows = cur.fetchall()

        readings = []

        for row in rows:
            readings.append({
                "id": row[0],
                "measured_at": row[1],
                "temperature": row[2],
                "humidity": row[3],
                "received_at": row[4]
            })

        return jsonify({
            "page": page,
            "page_size": page_size,
            "readings": readings
        }), 200

    except Exception as error:
        return error_response(str(error), 500)

    finally:
        cur.close()
        conn.close()

@data_bp.route("/get_day", methods=["GET"])
def get_by_day():
    return "done" , 201
