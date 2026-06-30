from __future__ import annotations

from pathlib import Path

from PIL import Image


PIECE_SIZE = 24

SRC_DIR = Path(__file__).resolve().parents[4] / "assets" / "images" / "chess-piece-png"
OUT_DIR = Path(__file__).resolve().parents[4] / "assets" / "images" / "chess" / "generated"
HEADER_PATH = OUT_DIR / "chess_piece_assets.h"
SOURCE_PATH = OUT_DIR / "chess_piece_assets.c"

PIECES = [
    ("pawn_white", "pawn_white.png"),
    ("rook_white", "rook_white.png"),
    ("knight_white", "knight_white.png"),
    ("bishop_white", "bishop_white.png"),
    ("queen_white", "queen_white.png"),
    ("king_white", "king_white.png"),
    ("pawn_black", "pawn_black.png"),
    ("rook_black", "rook_black.png"),
    ("knight_black", "knight_black.png"),
    ("bishop_black", "bishop_black.png"),
    ("queen_black", "queen_black.png"),
    ("king_black", "king_black.png"),
]


def wrap_bytes(data: bytes, bytes_per_line: int = 16) -> str:
    chunks = []
    for i in range(0, len(data), bytes_per_line):
        chunk = data[i : i + bytes_per_line]
        chunks.append("    " + ",".join(f"0x{b:02x}" for b in chunk))
    return ",\n".join(chunks)


def main() -> None:
    OUT_DIR.mkdir(parents=True, exist_ok=True)

    header_lines = [
        "#pragma once",
        "",
        '#include "lvgl.h"',
        "",
    ]

    source_lines = [
        '#include "chess_piece_assets.h"',
        "",
        "#ifndef LV_ATTRIBUTE_MEM_ALIGN",
        "#define LV_ATTRIBUTE_MEM_ALIGN",
        "#endif",
        "",
        "#ifndef LV_ATTRIBUTE_LARGE_CONST",
        "#define LV_ATTRIBUTE_LARGE_CONST",
        "#endif",
        "",
    ]

    for name, filename in PIECES:
        image_path = SRC_DIR / filename
        image = Image.open(image_path).convert("RGBA")
        image = image.resize((PIECE_SIZE, PIECE_SIZE), Image.LANCZOS)
        data = image.tobytes()
        array_name = f"img_{name}_map"
        descriptor_name = f"img_{name}"
        attr_name = f"LV_ATTRIBUTE_IMAGE_IMG_{name.upper()}"

        header_lines.append(f"extern const lv_image_dsc_t {descriptor_name};")

        source_lines.extend(
            [
                f"#ifndef {attr_name}",
                f"#define {attr_name}",
                "#endif",
                "",
                "static const",
                f"LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST {attr_name}",
                f"uint8_t {array_name}[] = {{",
                wrap_bytes(data),
                "};",
                "",
                f"const lv_image_dsc_t {descriptor_name} = {{",
                "    .header.magic = LV_IMAGE_HEADER_MAGIC,",
                "    .header.cf = LV_COLOR_FORMAT_ARGB8888,",
                "    .header.flags = 0,",
                f"    .header.w = {PIECE_SIZE},",
                f"    .header.h = {PIECE_SIZE},",
                f"    .header.stride = {PIECE_SIZE * 4},",
                f"    .data_size = sizeof({array_name}),",
                f"    .data = {array_name},",
                "};",
                "",
            ]
        )

    HEADER_PATH.write_text("\n".join(header_lines) + "\n", encoding="utf-8")
    SOURCE_PATH.write_text("\n".join(source_lines), encoding="utf-8")
    print(f"Generated {HEADER_PATH}")
    print(f"Generated {SOURCE_PATH}")


if __name__ == "__main__":
    main()
