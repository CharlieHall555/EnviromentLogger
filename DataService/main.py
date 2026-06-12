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
    return jsonify({"Health" : health})


@app.route("/version" , methods = ["GET"])
def getVersion():
    return jsonify({"Version" : version})


@app.route("/db-health" , methods = ["GET"])
def getDbHealth():
    try:
        conn = database_connection.establish_connection()
        database_connection.close_connection(conn)
        return jsonify({"Database": "OK"})
    except Exception as error:
        return jsonify({"Database": "ERROR", "Message": str(error)}), 500

if __name__ == '__main__':
    app.register_blueprint(data_bp)
    app.run()