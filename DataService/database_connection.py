import psycopg2
from config import Config

config = Config()

def establish_connection() -> psycopg2.extensions.connection:
    conn = psycopg2.connect(
        host = config.host_name,
        database = config.database_name,
        user = config.user,
        password = config.password,
        port = config.port
    )
    return conn

def close_connection(conn :  psycopg2.extensions.connection) -> None:
    conn.close()