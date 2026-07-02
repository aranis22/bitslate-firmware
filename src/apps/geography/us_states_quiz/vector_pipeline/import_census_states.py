from __future__ import annotations

from pathlib import Path
import sys
import urllib.error
import urllib.request
import zipfile


CENSUS_STATE_20M_URL = "https://www2.census.gov/geo/tiger/GENZ2024/shp/cb_2024_us_state_20m.zip"

ROOT = Path(__file__).resolve().parent
DATA_DIR = ROOT / "data"
ZIP_PATH = DATA_DIR / "cb_2024_us_state_20m.zip"
EXTRACT_DIR = DATA_DIR / "cb_2024_us_state_20m"


def download_zip() -> None:
    DATA_DIR.mkdir(parents=True, exist_ok=True)
    if ZIP_PATH.exists():
        print(f"Using existing {ZIP_PATH}")
        return

    print(f"Downloading {CENSUS_STATE_20M_URL}")
    try:
        urllib.request.urlretrieve(CENSUS_STATE_20M_URL, ZIP_PATH)
    except (urllib.error.URLError, OSError) as exc:
        print(f"Could not download Census states zip: {exc}")
        print(f"Place the zip manually at: {ZIP_PATH}")
        raise SystemExit(1) from exc


def unzip() -> None:
    EXTRACT_DIR.mkdir(parents=True, exist_ok=True)
    try:
        with zipfile.ZipFile(ZIP_PATH) as archive:
            archive.extractall(EXTRACT_DIR)
    except zipfile.BadZipFile as exc:
        print(f"Invalid zip file: {ZIP_PATH}")
        raise SystemExit(1) from exc


def print_shapefiles() -> None:
    shapefiles = sorted(EXTRACT_DIR.glob("*.shp"))
    if not shapefiles:
        print(f"No shapefiles found in {EXTRACT_DIR}")
        return

    print("Detected shapefiles:")
    for path in shapefiles:
        print(f"  {path}")


def main() -> None:
    download_zip()
    unzip()
    print_shapefiles()
    print("Done. LVGL generated data was not modified.")


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        sys.exit(130)
