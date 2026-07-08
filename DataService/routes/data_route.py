from flask import Flask, Blueprint, request, jsonify
from datetime import datetime , timezone, timedelta
import database_connection
from pydantic import BaseModel, Field, ValidationError
import logging
from config import Config
import math

config = Config()

class ReadingAddBody(BaseModel):
    measured_at: int = Field(ge=0)  # unix time
    temperature: float | None = Field(default=None, ge=-50, le=80)
    humidity: float | None = Field(default=None, ge=0, le=100)
    pm1_0: int | None = Field(default=None, ge=0, le=10000)
    pm2_5: int | None = Field(default=None, ge=0, le=10000)
    pm10: int | None = Field(default=None, ge=0, le=10000)

class Reading(BaseModel):
    id: int
    measured_at: datetime
    received_at: datetime | None = None
    temperature: float | None = Field(default=None, ge=-50, le=80)
    humidity: float | None = Field(default=None, ge=0, le=100)
    pm1_0: int | None = Field(default=None, ge=0, le=10000)
    pm2_5: int | None = Field(default=None, ge=0, le=10000)
    pm10: int | None = Field(default=None, ge=0, le=10000)

data_bp = Blueprint("data", __name__, url_prefix="/data")

def error_response(message: str, status_code: int = 500):
    response = {
        "result": "error",
        "message": message
    }

    return jsonify(response), status_code


def round_two_decimals(value: float | None) -> float | None:
    if value is None:
        return None

    return round(value, 2)


@data_bp.route("/add", methods=["POST"])
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
        reading = ReadingAddBody.model_validate(data)
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

    measured_at_formatted = None

    try:
        print(reading.measured_at)
        measured_at_formatted = datetime.fromtimestamp(reading.measured_at , timezone.utc)
    except:
        return error_response(
            "timestamp invalid",
            500
        )

    temperature = round_two_decimals(reading.temperature)
    humidity = round_two_decimals(reading.humidity)

    try:

        cur = conn.cursor()
        cur.execute(
            """
            INSERT INTO "READINGS" (temperature, humidity, pm10, pm1_0, pm2_5, measured_at)
            VALUES (%s, %s, %s, %s, %s, %s)
            RETURNING id, measured_at, temperature, humidity, pm10, pm1_0, pm2_5, received_at;
            """,
            (
                temperature,
                humidity,
                reading.pm10,
                reading.pm1_0,
                reading.pm2_5,
                measured_at_formatted,
            )
        )

        cur.execute(
            """
            INSERT INTO "DAILY_READING_SUMMARY" (
                day,
                temp_sum,
                temp_count,
                temp_min,
                temp_max,
                humidity_sum,
                humidity_count,
                humidity_min,
                humidity_max,
                pm10_sum,
                pm10_count,
                pm10_min,
                pm10_max,
                pm1_0_sum,
                pm1_0_count,
                pm1_0_min,
                pm1_0_max,
                pm2_5_sum,
                pm2_5_count,
                pm2_5_min,
                pm2_5_max
            )
            VALUES (
                %s,
                %s,
                1,
                %s,
                %s,
                %s,
                1,
                %s,
                %s,
                %s,
                1,
                %s,
                %s,
                %s,
                1,
                %s,
                %s,
                %s,
                1,
                %s,
                %s
            )
            ON CONFLICT (day)
            DO UPDATE SET
                temp_sum = "DAILY_READING_SUMMARY".temp_sum + EXCLUDED.temp_sum,
                temp_count = "DAILY_READING_SUMMARY".temp_count + EXCLUDED.temp_count,
                temp_min = LEAST("DAILY_READING_SUMMARY".temp_min, EXCLUDED.temp_min),
                temp_max = GREATEST("DAILY_READING_SUMMARY".temp_max, EXCLUDED.temp_max),

                humidity_sum = "DAILY_READING_SUMMARY".humidity_sum + EXCLUDED.humidity_sum,
                humidity_count = "DAILY_READING_SUMMARY".humidity_count + EXCLUDED.humidity_count,
                humidity_min = LEAST("DAILY_READING_SUMMARY".humidity_min, EXCLUDED.humidity_min),
                humidity_max = GREATEST("DAILY_READING_SUMMARY".humidity_max, EXCLUDED.humidity_max),

                pm10_sum = "DAILY_READING_SUMMARY".pm10_sum + EXCLUDED.pm10_sum,
                pm10_count = "DAILY_READING_SUMMARY".pm10_count + EXCLUDED.pm10_count,
                pm10_min = LEAST("DAILY_READING_SUMMARY".pm10_min, EXCLUDED.pm10_min),
                pm10_max = GREATEST("DAILY_READING_SUMMARY".pm10_max, EXCLUDED.pm10_max),

                pm1_0_sum = "DAILY_READING_SUMMARY".pm1_0_sum + EXCLUDED.pm1_0_sum,
                pm1_0_count = "DAILY_READING_SUMMARY".pm1_0_count + EXCLUDED.pm1_0_count,
                pm1_0_min = LEAST("DAILY_READING_SUMMARY".pm1_0_min, EXCLUDED.pm1_0_min),
                pm1_0_max = GREATEST("DAILY_READING_SUMMARY".pm1_0_max, EXCLUDED.pm1_0_max),

                pm2_5_sum = "DAILY_READING_SUMMARY".pm2_5_sum + EXCLUDED.pm2_5_sum,
                pm2_5_count = "DAILY_READING_SUMMARY".pm2_5_count + EXCLUDED.pm2_5_count,
                pm2_5_min = LEAST("DAILY_READING_SUMMARY".pm2_5_min, EXCLUDED.pm2_5_min),
                pm2_5_max = GREATEST("DAILY_READING_SUMMARY".pm2_5_max, EXCLUDED.pm2_5_max);
            """,
            (
                measured_at_formatted,
                temperature,
                temperature,
                temperature,
                humidity,
                humidity,
                humidity,
                reading.pm10,
                reading.pm10,
                reading.pm10,
                reading.pm1_0,
                reading.pm1_0,
                reading.pm1_0,
                reading.pm2_5,
                reading.pm2_5,
                reading.pm2_5,
            )
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
            SELECT id, measured_at, temperature, humidity, pm10, pm1_0, pm2_5, received_at
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
                "pm10": row[4],
                "pm1_0": row[5],
                "pm2_5": row[6],
                "received_at": row[7]
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
    date_value = request.args.get("date", type=str)

    if not date_value:
        return error_response("date is required in YYYY-MM-DD format", 400)

    try:
        day_start = datetime.strptime(date_value, "%Y-%m-%d").replace(tzinfo=timezone.utc)
    except ValueError:
        return error_response("date must be in YYYY-MM-DD format", 400)

    day_end = day_start + timedelta(days=1) 

    try:
        conn = database_connection.establish_connection()
    except Exception:
        return error_response("Database connection failed", 500)

    try:
        with conn.cursor() as cur:
            cur.execute(
                """
                SELECT
                    COALESCE(SUM(temp_sum), 0),
                    COALESCE(SUM(temp_count), 0),
                    MIN(temp_min),
                    MAX(temp_max),
                    COALESCE(SUM(humidity_sum), 0),
                    COALESCE(SUM(humidity_count), 0),
                    MIN(humidity_min),
                    MAX(humidity_max),
                    COALESCE(SUM(pm10_sum), 0),
                    COALESCE(SUM(pm10_count), 0),
                    MIN(pm10_min),
                    MAX(pm10_max),
                    COALESCE(SUM(pm1_0_sum), 0),
                    COALESCE(SUM(pm1_0_count), 0),
                    MIN(pm1_0_min),
                    MAX(pm1_0_max),
                    COALESCE(SUM(pm2_5_sum), 0),
                    COALESCE(SUM(pm2_5_count), 0),
                    MIN(pm2_5_min),
                    MAX(pm2_5_max)
                FROM "DAILY_READING_SUMMARY"
                WHERE day >= %s AND day < %s;
                """,
                (day_start, day_end)
            )

            summary_row = cur.fetchone()

        (
            temp_sum,
            temp_count,
            temp_min,
            temp_max,
            humidity_sum,
            humidity_count,
            humidity_min,
            humidity_max,
            pm10_sum,
            pm10_count,
            pm10_min,
            pm10_max,
            pm1_0_sum,
            pm1_0_count,
            pm1_0_min,
            pm1_0_max,
            pm2_5_sum,
            pm2_5_count,
            pm2_5_min,
            pm2_5_max,
        ) = summary_row # type: ignore

        temperature_avg = None
        if temp_count:
            temperature_avg = temp_sum / temp_count

        humidity_avg = None
        if humidity_count:
            humidity_avg = humidity_sum / humidity_count

        pm10_avg = None
        if pm10_count:
            pm10_avg = pm10_sum / pm10_count

        pm1_0_avg = None
        if pm1_0_count:
            pm1_0_avg = pm1_0_sum / pm1_0_count

        pm2_5_avg = None
        if pm2_5_count:
            pm2_5_avg = pm2_5_sum / pm2_5_count

        return jsonify({
            "date": date_value,
            "total_size": temp_count,
            "temperature_avg": temperature_avg,
            "temperature_min": temp_min,
            "temperature_max": temp_max,
            "humidity_avg": humidity_avg,
            "humidity_min": humidity_min,
            "humidity_max": humidity_max,
            "pm10_avg": pm10_avg,
            "pm10_min": pm10_min,
            "pm10_max": pm10_max,
            "pm1_0_avg": pm1_0_avg,
            "pm1_0_min": pm1_0_min,
            "pm1_0_max": pm1_0_max,
            "pm2_5_avg": pm2_5_avg,
            "pm2_5_min": pm2_5_min,
            "pm2_5_max": pm2_5_max
        }), 200

    except Exception as error:
        return error_response(str(error), 500)

    finally:
        conn.close()
