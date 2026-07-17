from flask_limiter import Limiter
from flask_limiter.util import get_remote_address

# Shared extension instance; initialized in main.py via init_app.
limiter = Limiter(
    key_func=get_remote_address,
    default_limits=["200 per day", "60 per hour"],
    storage_uri="memory://",
)
