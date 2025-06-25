
import json
import pathlib

BASE_DIR = pathlib.Path(__file__).resolve().parent
CONFIG_PATH = BASE_DIR / "pills_config.json"

with CONFIG_PATH.open(encoding="utf-8") as fh:
    data = json.load(fh)

PILLS: dict = data["pills"]
