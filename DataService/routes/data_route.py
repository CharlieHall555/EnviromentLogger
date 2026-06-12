from flask import Flask, Blueprint, request, jsonify
import database_connection
from pydantic import BaseModel, Field, ValidationError


class ReadingCreate(BaseModel):
    temperature: float = Field(ge=-50, le=80)
    humidity: float = Field(ge=0, le=100)


data_bp = Blueprint("data", __name__)


def error_response(message: str, status_code: int = 500, errors=None):
    response = {
        "result": "error",
        "message": message
    }

    if errors is not None:
        response["errors"] = errors

    return jsonify(response), status_code


@data_bp.route("/add_reading", methods=["POST"])
def add_reading():
    data = request.get_json(silent=True)

    if data is None:
        return error_response("Request body must be valid JSON", 400)

    try:
        reading = ReadingCreate.model_validate(data)
    except ValidationError as error:
        return error_response(
            "Request data is invalid",
            400,
            error.errors()
        )

    conn = None

    try:
        conn = database_connection.establish_connection()

    except Exception as error:
        return error_response(
            "Database connection failed",
            500,
            str(error)
        )

    cur = conn.cursor()
    cur.execute(
    """
        INSERT INTO "READINGS" (test)
        VALUES (%s);
    """, (reading.temperature,)
    )

    conn.commit()
    cur.close()
    conn.close()

    return "done" , 201
