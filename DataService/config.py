import os
from dotenv import load_dotenv

load_dotenv()


def _get_bool_env(name: str, default: bool = False) -> bool:
    value = os.getenv(name)
    if value is None:
        return default

    return value.strip().lower() in {"1", "true", "t", "yes", "y", "on"}

class Config:

    def __init__(self):
        self.host_name = os.getenv("HOST_NAME", "localhost")
        self.database_name = os.getenv("DATABASE_NAME")
        self.user = os.getenv("USER")
        self.password = os.getenv("PASSWORD")
        self.port = int(os.getenv("PORT", "30050"))
        self.admin_token = os.getenv("ADMIN_TOKEN")
        self.api_token = os.getenv("API_TOKEN")
        self.debug = _get_bool_env("DEBUG", default=False)
        self.cron_token = os.getenv("CRON_TOKEN")