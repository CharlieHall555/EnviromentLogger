from flask import Flask, jsonify
import psycopg2
from config import Config
import database_connection
from flask_cors import CORS

from routes.data_route import data_bp
from routes.hardware_logging import hardware_log_bp

from flask_limiter import Limiter
from flask_limiter.util import get_remote_address

version: float = 0.1
health: str = "OK"
config = Config()

app = Flask(__name__)
cors = CORS(app, resources={r"/*": {"origins": "*"}})

limiter = Limiter(
    key_func=get_remote_address,
    app=app,
    default_limits=["200 per day", "60 per hour"],
    storage_uri="memory://"
)

@app.route("/health", methods=["GET"])
def getHealth():
    return jsonify({"health": health}), 200


@app.route("/version", methods=["GET"])
def getVersion():
    return jsonify({"version": version}), 200


@app.route("/db-health", methods=["GET"])
def getDbHealth():
    try:
        conn = database_connection.establish_connection()
        database_connection.close_connection(conn)
        return jsonify({"database": "OK"})
    except Exception as error:
        return jsonify({"database": "ERROR", "message": str(error)}), 500


app.register_blueprint(data_bp)
app.register_blueprint(hardware_log_bp)

print(config.debug)

if __name__ == "__main__" and config.debug == True:
    app.run(host="0.0.0.0", port=5000,debug=True)
