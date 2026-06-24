import os
from dotenv import load_dotenv

load_dotenv()

class Config:

    def __init__(self):
        self.host_name = os.getenv("HOST_NAME", "localhost")
        self.database_name = os.getenv("DATABASE_NAME")
        self.user = os.getenv("USER")
        self.password = os.getenv("PASSWORD")
        self.port = int(os.getenv("PORT", "30050"))
        self.admin_token = os.getenv("ADMIN_TOKEN")
        self.api_token = os.getenv("API_TOKEN")