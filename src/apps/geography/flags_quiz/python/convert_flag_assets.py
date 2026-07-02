from pathlib import Path

from PIL import Image


ROOT = Path(__file__).resolve().parents[1]
REPO_ROOT = ROOT.parents[3]
SOURCE_DIRS = [
    ROOT,
    REPO_ROOT / "src" / "apps" / "assets" / "images" / "geography",
]
OUT_DIR = ROOT / "generated"
HEADER = OUT_DIR / "FlagsQuizAssets.h"
CPP = OUT_DIR / "FlagsQuizAssets.cpp"

MAX_FLAG_W = 270
MAX_FLAG_H = 180
IGNORED_SLUGS = {
    "flag-gui",
    "usa-map-full",
    "usa-map-unselected",
    "map-full",
    "map-unselected",
}


def rgb565(r: int, g: int, b: int) -> int:
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)


def display_name(slug: str) -> str:
    if slug == "usa":
        return "USA"
    return " ".join(part.capitalize() for part in slug.replace("_", "-").split("-"))


def symbol_name(slug: str) -> str:
    return "".join(part.capitalize() for part in slug.replace("_", "-").split("-"))


def format_bytes(values: list[int]) -> str:
    lines = []
    for i in range(0, len(values), 16):
        chunk = values[i : i + 16]
        lines.append("    " + ",".join(f"0x{value:02X}" for value in chunk) + ",")
    return "\n".join(lines)


def find_sources() -> tuple[list[Path], list[str]]:
    by_slug: dict[str, Path] = {}
    ignored: list[str] = []
    for folder in SOURCE_DIRS:
        if not folder.exists():
            continue
        for path in sorted(folder.glob("*.png")):
            slug = path.stem.lower()
            if slug in IGNORED_SLUGS or slug.startswith("usa-map"):
                ignored.append(str(path.relative_to(REPO_ROOT)))
                continue
            if slug not in by_slug:
                by_slug[slug] = path
    return [by_slug[slug] for slug in sorted(by_slug)], ignored


def convert_image(path: Path) -> tuple[int, int, list[int]]:
    source = Image.open(path).convert("RGBA")
    scale = min(MAX_FLAG_W / source.width, MAX_FLAG_H / source.height)
    width = max(1, round(source.width * scale))
    height = max(1, round(source.height * scale))
    image = source.resize((width, height), Image.Resampling.LANCZOS)

    data: list[int] = []
    for r, g, b, a in image.getdata():
        if a < 255:
            r = (r * a + 0x22 * (255 - a)) // 255
            g = (g * a + 0x28 * (255 - a)) // 255
            b = (b * a + 0x2C * (255 - a)) // 255
        value = rgb565(r, g, b)
        data.append(value & 0xFF)
        data.append(value >> 8)
    return width, height, data


def main() -> None:
    sources, ignored = find_sources()
    if not sources:
        raise SystemExit("No flag PNGs found for flags quiz asset generation")

    OUT_DIR.mkdir(parents=True, exist_ok=True)

    entries = []
    cpp_chunks = []
    for path in sources:
        slug = path.stem.lower()
        width, height, data = convert_image(path)
        symbol = symbol_name(slug)
        array_name = f"FLAG_{symbol.upper()}_IMAGE_DATA"
        image_name = f"FLAG_{symbol.upper()}_IMAGE"
        cpp_chunks.append(
            f"""static const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST uint8_t {array_name}[] = {{
{format_bytes(data)}
}};

static const lv_image_dsc_t {image_name} = {{
    {{
        LV_IMAGE_HEADER_MAGIC,
        LV_COLOR_FORMAT_RGB565,
        0,
        {width},
        {height},
        {width * 2},
        0,
    }},
    sizeof({array_name}),
    {array_name},
    nullptr,
    nullptr,
}};
"""
        )
        entries.append((display_name(slug), slug, image_name, width, height, path))

    asset_entries = "\n".join(
        f'    {{"{name}", "{slug}", &{image_name}, {width}, {height}}},'
        for name, slug, image_name, width, height, _ in entries
    )

    HEADER.write_text(
        """#pragma once

#include <stdint.h>
#include <lvgl.h>

namespace bitslate::geo::flags {

struct FlagQuizAsset {
  const char* name;
  const char* slug;
  const lv_image_dsc_t* image;
  uint16_t width;
  uint16_t height;
};

extern const FlagQuizAsset FLAGS_QUIZ_ASSETS[];
extern const uint8_t FLAGS_QUIZ_ASSET_COUNT;

}  // namespace bitslate::geo::flags
""",
        encoding="utf-8",
    )

    CPP.write_text(
        f"""#include "FlagsQuizAssets.h"

#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

#ifndef LV_ATTRIBUTE_LARGE_CONST
#define LV_ATTRIBUTE_LARGE_CONST
#endif

namespace bitslate::geo::flags {{

{"".join(cpp_chunks)}
const FlagQuizAsset FLAGS_QUIZ_ASSETS[] = {{
{asset_entries}
}};

const uint8_t FLAGS_QUIZ_ASSET_COUNT =
    static_cast<uint8_t>(sizeof(FLAGS_QUIZ_ASSETS) / sizeof(FLAGS_QUIZ_ASSETS[0]));

}}  // namespace bitslate::geo::flags
""",
        encoding="utf-8",
    )

    print(f"Generated {HEADER}")
    print(f"Generated {CPP}")
    print("Converted flags:")
    for name, slug, _, width, height, path in entries:
        print(f"  {name} ({slug}): {width}x{height} from {path.relative_to(REPO_ROOT)}")
    print("Ignored files:")
    for item in ignored:
        print(f"  {item}")


if __name__ == "__main__":
    main()
