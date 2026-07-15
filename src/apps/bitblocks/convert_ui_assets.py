"""Convert BitBlocks control PNGs to LVGL 9.5 ARGB8888 descriptors."""
from pathlib import Path
from PIL import Image

SIZE = 32
HERE = Path(__file__).resolve().parent
ASSET_DIR = HERE.parents[1] / "assets" / "UI" / "bitblocks-ui"
OUTPUT_H = HERE / "bitblocks_ui_assets.h"
OUTPUT_C = HERE / "bitblocks_ui_assets.c"
ASSETS = ("play", "stop", "trash")

def wrap(data, width=16):
    return ",\n".join("    "+",".join(f"0x{v:02x}" for v in data[i:i+width]) for i in range(0,len(data),width))

def fitted_rgba(path):
    image=Image.open(path).convert("RGBA")
    bounds=image.getbbox()
    if bounds:image=image.crop(bounds)
    image.thumbnail((SIZE,SIZE),Image.Resampling.NEAREST)
    canvas=Image.new("RGBA",(SIZE,SIZE),(0,0,0,0))
    canvas.alpha_composite(image,((SIZE-image.width)//2,(SIZE-image.height)//2))
    return canvas.tobytes()

def main():
    header=["#pragma once","",'#include "lvgl.h"',""]
    source=['#include "bitblocks_ui_assets.h"',"","#ifndef LV_ATTRIBUTE_MEM_ALIGN","#define LV_ATTRIBUTE_MEM_ALIGN","#endif","","#ifndef LV_ATTRIBUTE_LARGE_CONST","#define LV_ATTRIBUTE_LARGE_CONST","#endif",""]
    for name in ASSETS:
        descriptor=f"bitblocks_{name}_icon"
        array=f"{descriptor}_data"
        header.append(f"extern const lv_image_dsc_t {descriptor};")
        source += [f"static const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST uint8_t {array}[] = {{",wrap(fitted_rgba(ASSET_DIR/f"{name}.png")),"};","",f"const lv_image_dsc_t {descriptor} = {{","    .header.magic = LV_IMAGE_HEADER_MAGIC,","    .header.cf = LV_COLOR_FORMAT_ARGB8888,","    .header.flags = 0,",f"    .header.w = {SIZE},",f"    .header.h = {SIZE},",f"    .header.stride = {SIZE*4},",f"    .data_size = sizeof({array}),",f"    .data = {array},","};",""]
    OUTPUT_H.write_text("\n".join(header)+"\n",encoding="utf-8")
    OUTPUT_C.write_text("\n".join(source),encoding="utf-8")

if __name__ == "__main__":main()
