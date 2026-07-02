from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
import urllib.error
import urllib.request
import zipfile
from typing import Sequence

from PIL import Image, ImageDraw
import shapefile


ROOT = Path(__file__).resolve().parents[5]
PIPELINE_DIR = Path(__file__).resolve().parent
DATA_DIR = PIPELINE_DIR / "data" / "world"
OUTPUT_DIR = ROOT / "src" / "apps" / "geography" / "us_states_quiz" / "generated"
ARTIFACT_DIR = ROOT / "src" / "apps" / "geography" / "us_states_quiz" / "artifacts"
HEADER_FILE = OUTPUT_DIR / "WorldMapData.h"
SOURCE_FILE = OUTPUT_DIR / "WorldMapData.cpp"
PREVIEW_FILE = ARTIFACT_DIR / "world_cpp_grid_preview.png"

NATURAL_EARTH_URL = "https://naturalearth.s3.amazonaws.com/110m_cultural/ne_110m_admin_0_countries.zip"
ZIP_PATH = DATA_DIR / "ne_110m_admin_0_countries.zip"
EXTRACT_DIR = DATA_DIR / "ne_110m_admin_0_countries"
SHAPEFILE_NAME = "ne_110m_admin_0_countries.shp"

GRID_W = 360
GRID_H = 180
NONE = 255
PREVIEW_SCALE = 3

OCEAN = (39, 52, 109)
BORDER = (244, 244, 240)
COUNTRY_PALETTE = [
    (159, 181, 138),
    (197, 162, 91),
    (143, 168, 190),
    (185, 137, 134),
    (169, 155, 189),
    (151, 178, 170),
    (200, 189, 130),
    (154, 171, 124),
    (184, 162, 168),
    (142, 159, 198),
    (181, 177, 164),
    (193, 160, 118),
]

Point = tuple[float, float]
Ring = list[Point]


@dataclass(frozen=True)
class CapitalSpec:
    display_name: str
    aliases: tuple[str, ...]
    capital_name: str
    capital_lon: float
    capital_lat: float


@dataclass
class CountryShape:
    name: str
    country_id: int
    rings: list[Ring]
    record_debug: str


@dataclass(frozen=True)
class LabelSpec:
    text: str
    lon: float
    lat: float


CAPITAL_SPECS = [
    CapitalSpec("India", ("India", "IND"), "New Delhi", 77.2090, 28.6139),
    CapitalSpec("China", ("China", "CHN"), "Beijing", 116.4074, 39.9042),
    CapitalSpec("United States", ("United States", "United States of America", "USA", "US"), "Washington, D.C.", -77.0369, 38.9072),
    CapitalSpec("Canada", ("Canada", "CAN"), "Ottawa", -75.6972, 45.4215),
    CapitalSpec("Indonesia", ("Indonesia", "IDN"), "Jakarta", 106.8456, -6.2088),
    CapitalSpec("Pakistan", ("Pakistan", "PAK"), "Islamabad", 73.0479, 33.6844),
    CapitalSpec("Brazil", ("Brazil", "BRA"), "Brasilia", -47.8825, -15.7942),
    CapitalSpec("Nigeria", ("Nigeria", "NGA"), "Abuja", 7.3986, 9.0765),
    CapitalSpec("Bangladesh", ("Bangladesh", "BGD"), "Dhaka", 90.4125, 23.8103),
    CapitalSpec("Russia", ("Russia", "Russian Federation", "RUS"), "Moscow", 37.6173, 55.7558),
    CapitalSpec("Mexico", ("Mexico", "MEX"), "Mexico City", -99.1332, 19.4326),
    CapitalSpec("Japan", ("Japan", "JPN"), "Tokyo", 139.6917, 35.6895),
    CapitalSpec("Ethiopia", ("Ethiopia", "ETH"), "Addis Ababa", 38.7578, 8.9806),
    CapitalSpec("Philippines", ("Philippines", "PHL"), "Manila", 120.9842, 14.5995),
    CapitalSpec("Egypt", ("Egypt", "EGY"), "Cairo", 31.2357, 30.0444),
    CapitalSpec("Vietnam", ("Vietnam", "VNM"), "Hanoi", 105.8342, 21.0278),
    CapitalSpec("DR Congo", ("Democratic Republic of the Congo", "Dem. Rep. Congo", "DR Congo", "COD"), "Kinshasa", 15.2663, -4.4419),
    CapitalSpec("Turkey", ("Turkey", "TUR"), "Ankara", 32.8597, 39.9334),
    CapitalSpec("Iran", ("Iran", "IRN"), "Tehran", 51.3890, 35.6892),
    CapitalSpec("Germany", ("Germany", "DEU"), "Berlin", 13.4050, 52.5200),
    CapitalSpec("Thailand", ("Thailand", "THA"), "Bangkok", 100.5018, 13.7563),
    CapitalSpec("United Kingdom", ("United Kingdom", "UK", "U.K.", "GBR"), "London", -0.1276, 51.5072),
    CapitalSpec("France", ("France", "FRA"), "Paris", 2.3522, 48.8566),
    CapitalSpec("Italy", ("Italy", "ITA"), "Rome", 12.4964, 41.9028),
    CapitalSpec("South Africa", ("South Africa", "ZAF"), "Pretoria", 28.2293, -25.7479),
    CapitalSpec("Tanzania", ("Tanzania", "United Republic of Tanzania", "TZA"), "Dodoma", 35.7516, -6.1630),
    CapitalSpec("Myanmar", ("Myanmar", "Burma", "MMR"), "Naypyidaw", 96.0785, 19.7633),
    CapitalSpec("Kenya", ("Kenya", "KEN"), "Nairobi", 36.8219, -1.2921),
    CapitalSpec("South Korea", ("South Korea", "Republic of Korea", "KOR"), "Seoul", 126.9780, 37.5665),
    CapitalSpec("Colombia", ("Colombia", "COL"), "Bogota", -74.0721, 4.7110),
    CapitalSpec("Spain", ("Spain", "ESP"), "Madrid", -3.7038, 40.4168),
    CapitalSpec("Argentina", ("Argentina", "ARG"), "Buenos Aires", -58.3816, -34.6037),
    CapitalSpec("Australia", ("Australia", "AUS"), "Canberra", 149.1300, -35.2809),
    CapitalSpec("Saudi Arabia", ("Saudi Arabia", "SAU"), "Riyadh", 46.6753, 24.7136),
    CapitalSpec("Algeria", ("Algeria", "DZA"), "Algiers", 3.0588, 36.7538),
    CapitalSpec("Ukraine", ("Ukraine", "UKR"), "Kyiv", 30.5234, 50.4501),
    CapitalSpec("Poland", ("Poland", "POL"), "Warsaw", 21.0122, 52.2297),
    CapitalSpec("Peru", ("Peru", "PER"), "Lima", -77.0428, -12.0464),
    CapitalSpec("Venezuela", ("Venezuela", "VEN"), "Caracas", -66.9036, 10.4806),
    CapitalSpec("Chile", ("Chile", "CHL"), "Santiago", -70.6693, -33.4489),
    CapitalSpec("Morocco", ("Morocco", "MAR"), "Rabat", -6.8498, 34.0209),
]

OCEANS = [
    LabelSpec("Pacific Ocean", -150.0, 5.0),
    LabelSpec("Atlantic Ocean", -35.0, 10.0),
    LabelSpec("Indian Ocean", 75.0, -25.0),
    LabelSpec("Arctic Ocean", 20.0, 78.0),
    LabelSpec("Southern Ocean", 20.0, -58.0),
]

COVERAGE_GROUPS = {
    "Central America": ["Guatemala", "Belize", "Honduras", "El Salvador", "Nicaragua", "Costa Rica", "Panama"],
    "Africa": [
        "Sudan",
        "South Sudan",
        "Chad",
        "Niger",
        "Mali",
        "Ghana",
        "Cameroon",
        "Angola",
        "Zambia",
        "Zimbabwe",
        "Uganda",
        "Somalia",
        "Libya",
        "Tunisia",
        "Senegal",
        "Mozambique",
        "Madagascar",
    ],
    "Europe": [
        "Norway",
        "Sweden",
        "Finland",
        "Denmark",
        "Netherlands",
        "Belgium",
        "Switzerland",
        "Austria",
        "Czechia",
        "Slovakia",
        "Hungary",
        "Romania",
        "Bulgaria",
        "Greece",
        "Serbia",
        "Croatia",
    ],
    "Central Asia": ["Kazakhstan", "Uzbekistan", "Turkmenistan", "Kyrgyzstan", "Tajikistan", "Afghanistan"],
    "Major": ["Canada", "United States", "Mexico", "Brazil", "Argentina", "Australia", "Russia", "China", "India"],
}


def lon_lat_to_grid(lon: float, lat: float) -> Point:
    return lon + 180.0, 90.0 - lat


def find_shapefile() -> Path | None:
    expected = EXTRACT_DIR / SHAPEFILE_NAME
    if expected.exists():
        return expected
    candidates = sorted(DATA_DIR.rglob("*.shp"))
    if candidates:
        return candidates[0]

    DATA_DIR.mkdir(parents=True, exist_ok=True)
    try:
        print(f"Downloading Natural Earth countries: {NATURAL_EARTH_URL}")
        urllib.request.urlretrieve(NATURAL_EARTH_URL, ZIP_PATH)
        EXTRACT_DIR.mkdir(parents=True, exist_ok=True)
        with zipfile.ZipFile(ZIP_PATH) as archive:
            archive.extractall(EXTRACT_DIR)
    except (urllib.error.URLError, OSError, zipfile.BadZipFile) as exc:
        print(f"Could not download/extract Natural Earth countries: {exc}")
        return None
    return expected if expected.exists() else None


def shape_to_rings(shape: shapefile.Shape) -> list[Ring]:
    points = shape.points
    parts = list(shape.parts) + [len(points)]
    rings: list[Ring] = []
    for index in range(len(parts) - 1):
        ring = [lon_lat_to_grid(float(lon), float(lat)) for lon, lat in points[parts[index] : parts[index + 1]]]
        if len(ring) >= 3:
            rings.append(ring)
    return rings


def record_values(record: dict[str, object]) -> set[str]:
    keys = (
        "NAME_LONG",
        "ADMIN",
        "NAME",
        "GEOUNIT",
        "NAME_SORT",
        "BRK_NAME",
        "ABBREV",
        "POSTAL",
        "ADM0_A3",
        "ISO_A2",
        "ISO_A3",
        "ADM0_ISO",
    )
    return {str(record.get(key, "")) for key in keys if record.get(key)}


def record_debug(record: dict[str, object]) -> str:
    keys = ("NAME_LONG", "ADMIN", "NAME", "TYPE", "ADM0_A3", "ISO_A3", "POSTAL")
    return " | ".join(f"{key}={record.get(key, '')}" for key in keys)


def display_name(record: dict[str, object]) -> str:
    name = str(record.get("NAME_LONG") or record.get("ADMIN") or record.get("NAME") or "")
    if name == "Russian Federation":
        return "Russia"
    if name == "Republic of Korea":
        return "South Korea"
    if name == "Democratic Republic of the Congo":
        return "DR Congo"
    if name == "Czech Republic":
        return "Czechia"
    return name


def should_export(record: dict[str, object]) -> tuple[bool, str]:
    name = display_name(record)
    if name == "Antarctica" or str(record.get("ADMIN", "")) == "Antarctica":
        return False, "Antarctica excluded"
    if not name:
        return False, "missing country name"
    if str(record.get("ADM0_A3", "")) == "-99" and str(record.get("ISO_A3", "")) == "-99":
        return False, "missing stable ISO/admin code"
    return True, ""


def load_countries(path: Path) -> tuple[list[CountryShape], list[str]]:
    reader = shapefile.Reader(str(path), encoding="latin1")
    fields = [field[0] for field in reader.fields[1:]]
    countries: list[CountryShape] = []
    skipped: list[str] = []

    for shape_record in reader.iterShapeRecords():
        record = dict(zip(fields, shape_record.record))
        ok, reason = should_export(record)
        if not ok:
            skipped.append(f"{display_name(record) or '<unnamed>'}: {reason} ({record_debug(record)})")
            continue
        if len(countries) >= NONE:
            skipped.append(f"{display_name(record)}: uint8_t id limit reached")
            continue
        rings = shape_to_rings(shape_record.shape)
        if not rings:
            skipped.append(f"{display_name(record)}: no drawable rings ({record_debug(record)})")
            continue
        countries.append(CountryShape(display_name(record), len(countries), rings, record_debug(record)))

    return countries, skipped


def point_in_ring(point: Point, ring: Ring) -> bool:
    x, y = point
    inside = False
    j = len(ring) - 1
    for i, (xi, yi) in enumerate(ring):
        xj, yj = ring[j]
        if (yi > y) != (yj > y):
            cross_x = (xj - xi) * (y - yi) / ((yj - yi) or 1e-12) + xi
            if x < cross_x:
                inside = not inside
        j = i
    return inside


def point_in_country(point: Point, country: CountryShape) -> bool:
    return any(point_in_ring(point, ring) for ring in country.rings)


def rasterize_country_id_grid(countries: Sequence[CountryShape]) -> list[int]:
    grid = [NONE] * (GRID_W * GRID_H)
    for y in range(GRID_H):
        for x in range(GRID_W):
            point = (x + 0.5, y + 0.5)
            for country in countries:
                if point_in_country(point, country):
                    grid[y * GRID_W + x] = country.country_id
                    break
    return grid


def border_cells(grid: Sequence[int]) -> list[tuple[int, int]]:
    borders: set[tuple[int, int]] = set()
    for y in range(GRID_H):
        for x in range(GRID_W):
            country_id = grid[y * GRID_W + x]
            if country_id == NONE:
                continue
            for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1)):
                nx, ny = x + dx, y + dy
                if nx < 0 or nx >= GRID_W or ny < 0 or ny >= GRID_H:
                    borders.add((x, y))
                    break
                if grid[ny * GRID_W + nx] != country_id:
                    borders.add((x, y))
                    break
    return sorted(borders, key=lambda cell: (cell[1], cell[0]))


def cells_by_country(grid: Sequence[int]) -> dict[int, list[Point]]:
    cells: dict[int, list[Point]] = {}
    for y in range(GRID_H):
        for x in range(GRID_W):
            country_id = grid[y * GRID_W + x]
            if country_id == NONE:
                continue
            cells.setdefault(country_id, []).append((x + 0.5, y + 0.5))
    return cells


def label_points(
    countries: Sequence[CountryShape],
    grid: Sequence[int],
    label_ids: Sequence[int],
) -> list[tuple[int, float, float, int]]:
    cell_map = cells_by_country(grid)
    labels: list[tuple[int, float, float, int]] = []
    for country_id in label_ids:
        cells = cell_map.get(country_id, [])
        if not cells:
            continue
        centroid_x = sum(x for x, _ in cells) / len(cells)
        centroid_y = sum(y for _, y in cells) / len(cells)
        best_x, best_y = min(cells, key=lambda p: (p[0] - centroid_x) ** 2 + (p[1] - centroid_y) ** 2)
        labels.append((country_id, best_x, best_y, len(cells)))
    return labels


def cpp_string(value: str) -> str:
    return value.replace("\\", "\\\\").replace('"', '\\"')


def format_grid(grid: Sequence[int]) -> str:
    lines = []
    for y in range(GRID_H):
        row = grid[y * GRID_W : (y + 1) * GRID_W]
        for start in range(0, GRID_W, 24):
            lines.append("    " + ", ".join(f"{value:3d}" for value in row[start : start + 24]) + ",")
    return "\n".join(lines)


def format_cells(cells: Sequence[tuple[int, int]]) -> str:
    lines = []
    for index in range(0, len(cells), 8):
        chunk = cells[index : index + 8]
        lines.append("    " + ", ".join(f"{{{x}, {y}}}" for x, y in chunk) + ",")
    return "\n".join(lines)


def find_country_id(countries: Sequence[CountryShape], aliases: Sequence[str]) -> int | None:
    alias_set = set(aliases)
    for country in countries:
        if country.name in alias_set:
            return country.country_id
        parts = {part.split("=", 1)[1] for part in country.record_debug.split(" | ") if "=" in part}
        if parts.intersection(alias_set):
            return country.country_id
    return None


def capital_rows(countries: Sequence[CountryShape]) -> tuple[list[tuple[int, CapitalSpec]], list[str]]:
    rows: list[tuple[int, CapitalSpec]] = []
    skipped: list[str] = []
    for spec in CAPITAL_SPECS:
        country_id = find_country_id(countries, spec.aliases)
        if country_id is None:
            skipped.append(spec.display_name)
            continue
        rows.append((country_id, spec))
    return rows, skipped


def write_header(country_count: int, capital_count: int, label_count: int) -> None:
    HEADER_FILE.write_text(
        f"""#pragma once

#include <stdint.h>

namespace bitslate::geo {{

static constexpr uint16_t WORLD_GRID_W = {GRID_W};
static constexpr uint16_t WORLD_GRID_H = {GRID_H};
static constexpr uint8_t WORLD_COUNTRY_COUNT = {country_count};
static constexpr uint8_t WORLD_COUNTRY_NONE = 255;
static constexpr uint8_t WORLD_CAPITAL_MARKER_COUNT = {capital_count};
static constexpr uint8_t WORLD_COUNTRY_LABEL_COUNT = {label_count};
static constexpr uint8_t WORLD_OCEAN_LABEL_COUNT = {len(OCEANS)};

struct WorldCountryInfo {{
  const char* name;
  uint8_t id;
}};

struct WorldMapCell {{
  uint16_t x;
  uint16_t y;
}};

struct WorldCapitalMarker {{
  uint8_t countryId;
  const char* countryName;
  const char* capitalName;
  float gridX;
  float gridY;
}};

struct WorldLabelPoint {{
  uint8_t countryId;
  const char* countryName;
  float gridX;
  float gridY;
  uint16_t cellCount;
}};

struct WorldOceanLabel {{
  const char* name;
  float gridX;
  float gridY;
}};

extern const WorldCountryInfo WORLD_COUNTRIES[WORLD_COUNTRY_COUNT];
extern const uint8_t WORLD_COUNTRY_ID_GRID[WORLD_GRID_W * WORLD_GRID_H];
extern const WorldMapCell WORLD_BORDER_CELLS[];
extern const uint16_t WORLD_BORDER_CELL_COUNT;
extern const WorldCapitalMarker WORLD_CAPITAL_MARKERS[WORLD_CAPITAL_MARKER_COUNT];
extern const WorldLabelPoint WORLD_COUNTRY_LABEL_POINTS[WORLD_COUNTRY_LABEL_COUNT];
extern const WorldOceanLabel WORLD_OCEAN_LABELS[WORLD_OCEAN_LABEL_COUNT];

int findWorldCountryByName(const char* name);
int worldCountryAtCell(uint16_t x, uint16_t y);

}}  // namespace bitslate::geo
""",
        encoding="utf-8",
    )


def write_source(
    countries: Sequence[CountryShape],
    grid: Sequence[int],
    borders: Sequence[tuple[int, int]],
    capitals: Sequence[tuple[int, CapitalSpec]],
    labels: Sequence[tuple[int, float, float, int]],
) -> None:
    lines = [
        '#include "WorldMapData.h"',
        "",
        "#include <cstring>",
        "",
        "namespace bitslate::geo {",
        "",
        "const WorldCountryInfo WORLD_COUNTRIES[WORLD_COUNTRY_COUNT] = {",
    ]
    for country in countries:
        lines.append(f'    {{"{cpp_string(country.name)}", {country.country_id}}},')
    lines.extend(
        [
            "};",
            "",
            "const uint8_t WORLD_COUNTRY_ID_GRID[WORLD_GRID_W * WORLD_GRID_H] = {",
            format_grid(grid),
            "};",
            "",
            "const WorldMapCell WORLD_BORDER_CELLS[] = {",
            format_cells(borders),
            "};",
            f"const uint16_t WORLD_BORDER_CELL_COUNT = {len(borders)};",
            "",
            "const WorldCapitalMarker WORLD_CAPITAL_MARKERS[WORLD_CAPITAL_MARKER_COUNT] = {",
        ]
    )
    for country_id, spec in capitals:
        grid_x, grid_y = lon_lat_to_grid(spec.capital_lon, spec.capital_lat)
        lines.append(
            f'    {{{country_id}, "{cpp_string(spec.display_name)}", "{cpp_string(spec.capital_name)}", {grid_x:.3f}f, {grid_y:.3f}f}},'
        )
    lines.extend(["};", "", "const WorldLabelPoint WORLD_COUNTRY_LABEL_POINTS[WORLD_COUNTRY_LABEL_COUNT] = {"])
    for country_id, x, y, cell_count in labels:
        lines.append(f'    {{{country_id}, "{cpp_string(countries[country_id].name)}", {x:.3f}f, {y:.3f}f, {cell_count}}},')
    lines.extend(["};", "", "const WorldOceanLabel WORLD_OCEAN_LABELS[WORLD_OCEAN_LABEL_COUNT] = {"])
    for ocean in OCEANS:
        x, y = lon_lat_to_grid(ocean.lon, ocean.lat)
        lines.append(f'    {{"{cpp_string(ocean.text)}", {x:.3f}f, {y:.3f}f}},')
    lines.extend(
        [
            "};",
            "",
            "int findWorldCountryByName(const char* name) {",
            "  for (uint8_t i = 0; i < WORLD_COUNTRY_COUNT; ++i) {",
            "    if (std::strcmp(WORLD_COUNTRIES[i].name, name) == 0) {",
            "      return WORLD_COUNTRIES[i].id;",
            "    }",
            "  }",
            "  return -1;",
            "}",
            "",
            "int worldCountryAtCell(uint16_t x, uint16_t y) {",
            "  if (x >= WORLD_GRID_W || y >= WORLD_GRID_H) {",
            "    return -1;",
            "  }",
            "  const uint8_t countryId = WORLD_COUNTRY_ID_GRID[y * WORLD_GRID_W + x];",
            "  return countryId == WORLD_COUNTRY_NONE ? -1 : countryId;",
            "}",
            "",
            "}  // namespace bitslate::geo",
            "",
        ]
    )
    SOURCE_FILE.write_text("\n".join(lines), encoding="utf-8")


def write_preview(grid: Sequence[int], borders: Sequence[tuple[int, int]]) -> None:
    image = Image.new("RGB", (GRID_W * PREVIEW_SCALE, GRID_H * PREVIEW_SCALE), OCEAN)
    draw = ImageDraw.Draw(image)
    for y in range(GRID_H):
        for x in range(GRID_W):
            country_id = grid[y * GRID_W + x]
            if country_id == NONE:
                continue
            color = COUNTRY_PALETTE[(country_id * 7 + 3) % len(COUNTRY_PALETTE)]
            draw.rectangle(
                (x * PREVIEW_SCALE, y * PREVIEW_SCALE, (x + 1) * PREVIEW_SCALE - 1, (y + 1) * PREVIEW_SCALE - 1),
                fill=color,
            )
    for x, y in borders:
        draw.rectangle(
            (x * PREVIEW_SCALE, y * PREVIEW_SCALE, (x + 1) * PREVIEW_SCALE - 1, (y + 1) * PREVIEW_SCALE - 1),
            fill=BORDER,
        )
    ARTIFACT_DIR.mkdir(parents=True, exist_ok=True)
    image.save(PREVIEW_FILE)


def print_coverage(countries: Sequence[CountryShape], grid: Sequence[int]) -> None:
    cell_map = cells_by_country(grid)
    print("Coverage check:")
    for group, names in COVERAGE_GROUPS.items():
        print(f"  {group}:")
        for name in names:
            country_id = find_country_id(countries, (name,))
            rendered = country_id is not None and len(cell_map.get(country_id, [])) > 0
            detail = f"id={country_id}, cells={len(cell_map.get(country_id, []))}" if country_id is not None else "missing"
            print(f"    {'yes' if rendered else 'NO '} {name}: {detail}")


def validate(countries: Sequence[CountryShape], grid: Sequence[int]) -> None:
    if len(countries) >= NONE:
        raise ValueError(f"Too many countries for uint8_t IDs: {len(countries)}")
    used_ids = {value for value in grid if value != NONE}
    missing = [country.name for country in countries if country.country_id not in used_ids]
    if missing:
        print(f"Countries with no rasterized cells: {', '.join(missing)}")


def main() -> None:
    shapefile_path = find_shapefile()
    if shapefile_path is None:
        raise SystemExit(1)

    countries, skipped = load_countries(shapefile_path)
    grid = rasterize_country_id_grid(countries)
    borders = border_cells(grid)
    capitals, skipped_capitals = capital_rows(countries)
    label_ids = [country_id for country_id, _ in capitals]
    labels = label_points(countries, grid, label_ids)
    validate(countries, grid)

    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)
    write_header(len(countries), len(capitals), len(labels))
    write_source(countries, grid, borders, capitals, labels)
    write_preview(grid, borders)

    print(f"Shapefile: {shapefile_path}")
    print(f"Grid: {GRID_W}x{GRID_H}")
    print(f"Rendered countries: {len(countries)}")
    print(f"Country ID headroom: {NONE - len(countries)}")
    print(f"Border cells: {len(borders)}")
    print(f"Capital markers: {len(capitals)}")
    print(f"Country labels: {len(labels)}")
    print(f"Ocean labels: {len(OCEANS)}")
    print("Skipped countries/reasons:")
    for item in skipped:
        print(f"  {item}")
    if not skipped:
        print("  none")
    print("Skipped curated capitals:")
    print("  " + (", ".join(skipped_capitals) if skipped_capitals else "none"))
    print_coverage(countries, grid)
    print(f"Generated {HEADER_FILE}")
    print(f"Generated {SOURCE_FILE}")
    print(f"Saved {PREVIEW_FILE}")


if __name__ == "__main__":
    main()
