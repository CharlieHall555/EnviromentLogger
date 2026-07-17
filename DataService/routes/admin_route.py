import os
from flask import Flask, Blueprint, request, jsonify
import database_connection
from config import Config
import hmac

config = Config()
admin_bp = Blueprint("admin", __name__, url_prefix="/admin")

def has_cleanup_authorisation() -> bool:
    auth_header = request.headers.get("Authorization", "")

    supplied_token = auth_header

    admin_token = config.admin_token
    cron_token = config.cron_token

    valid_admin_token = (
        bool(admin_token)
        and hmac.compare_digest(supplied_token, admin_token)
    )

    valid_cron_token = (
        bool(cron_token)
        and hmac.compare_digest(supplied_token, cron_token)
    )

    return valid_admin_token or valid_cron_token


@admin_bp.post("/cleanup-logs")
def cleanup_logs():
    if not has_cleanup_authorisation():
        return jsonify({
            "status": "ERROR",
            "message": "Unauthorized"
        }), 401

    conn = None

    try:
        conn = database_connection.establish_connection()

        with conn.cursor() as cursor:
            cursor.execute("""
                DELETE FROM "HARDWARE_LOGS"
                WHERE sent_at < NOW() - INTERVAL '7 days'
            """)

            deleted_count = cursor.rowcount

        conn.commit()

        return jsonify({
            "status": "OK",
            "deleted_logs": deleted_count
        }), 200

    except Exception as error:
        if conn is not None:
            conn.rollback()

        return jsonify({
            "status": "ERROR",
            "message": str(error)
        }), 500

    finally:
        if conn is not None:
            database_connection.close_connection(conn)