from flask import Flask, jsonify
import psycopg2
from config import Config
import database_connection

from routes.data_route import data_bp

version : float = 0.1
health : str = "OK"
config = Config()

app = Flask(__name__)

@app.route("/health" , methods = ["GET"])
def getHealth():
    return jsonify({"health" : health}) , 200


@app.route("/version" , methods = ["GET"])
def getVersion():
    return jsonify({"version" : version}), 200


@app.route("/db-health" , methods = ["GET"])
def getDbHealth():
    try:
        conn = database_connection.establish_connection()
        database_connection.close_connection(conn)
        return jsonify({"database": "OK"})
    except Exception as error:
        return jsonify({"database": "ERROR", "message": str(error)}), 500

app.register_blueprint(data_bp)