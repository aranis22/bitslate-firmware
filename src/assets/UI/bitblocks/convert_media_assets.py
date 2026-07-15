"""Generate device-sized LVGL 9.5 BitBlocks sprite/backdrop descriptors."""
from pathlib import Path
import re
import shutil
from PIL import Image

MEDIA_ROOT=Path(__file__).resolve().parent.parent
GENERATED=Path(__file__).resolve().parent/"generated"

def identifier(value):
    value=re.sub(r"[^0-9A-Za-z_]","_",value).lower()
    if not value or value[0].isdigit():value="asset_"+value
    return re.sub(r"_+","_",value)

def format_bytes(data):
    return ",\n".join("    "+",".join(f"0x{x:02x}" for x in data[i:i+32]) for i in range(0,len(data),32))

def fit_sprite(source,size):
    image=source.convert("RGBA");bounds=image.getbbox()
    if bounds:image=image.crop(bounds)
    image.thumbnail((size,size),Image.Resampling.NEAREST)
    canvas=Image.new("RGBA",(size,size),(0,0,0,0))
    canvas.alpha_composite(image,((size-image.width)//2,(size-image.height)//2))
    return canvas

def argb8888_payload(image):
    rgba=image.tobytes();out=bytearray(len(rgba))
    for i in range(0,len(rgba),4):out[i:i+4]=(rgba[i+2],rgba[i+1],rgba[i],rgba[i+3])
    return bytes(out),"LV_COLOR_FORMAT_ARGB8888",image.width*4

def rgb565_payload(image):
    out=bytearray(image.width*image.height*2);i=0
    for r,g,b in image.convert("RGB").getdata():
        value=((r&0xf8)<<8)|((g&0xfc)<<3)|(b>>3)
        out[i]=value&0xff;out[i+1]=value>>8;i+=2
    return bytes(out),"LV_COLOR_FORMAT_RGB565",image.width*2

def write_group(folder,input_dir,prefix,transform,payload_fn):
    output=GENERATED/folder;output.mkdir(parents=True,exist_ok=True);entries=[];total=0
    for path in sorted(input_dir.glob("*.png"),key=lambda p:p.name.lower()):
        image=transform(Image.open(path));payload,color_format,stride=payload_fn(image);total+=len(payload)
        slug=identifier(path.stem);symbol=f"bitblocks_{prefix}_{slug}";array=f"{symbol}_data"
        text=f'''#include "{folder}_manifest.h"

#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif
#ifndef LV_ATTRIBUTE_LARGE_CONST
#define LV_ATTRIBUTE_LARGE_CONST
#endif

static const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST uint8_t {array}[] = {{
{format_bytes(payload)}
}};

const lv_image_dsc_t {symbol} = {{
    .header.magic = LV_IMAGE_HEADER_MAGIC,
    .header.cf = {color_format},
    .header.flags = 0,
    .header.w = {image.width},
    .header.h = {image.height},
    .header.stride = {stride},
    .data_size = sizeof({array}),
    .data = {array},
}};
'''
        (output/f"{slug}.c").write_text(text,encoding="utf-8");entries.append(symbol)
    manifest=["#pragma once","",'#include "lvgl.h"',""]+[f"extern const lv_image_dsc_t {symbol};" for symbol in entries]
    (output/f"{folder}_manifest.h").write_text("\n".join(manifest)+"\n",encoding="utf-8")
    return len(entries),total

def main():
    if GENERATED.exists():shutil.rmtree(GENERATED)
    sprite_dir=MEDIA_ROOT/"sprites";backdrop_dir=MEDIA_ROOT/"backdrops"
    results={
        "sprites_128":write_group("sprites_128",sprite_dir,"sprite_128",lambda im:fit_sprite(im,128),argb8888_payload),
        "sprites_64":write_group("sprites_64",sprite_dir,"sprite_64",lambda im:fit_sprite(im,64),argb8888_payload),
        "backdrops_480x320":write_group("backdrops_480x320",backdrop_dir,"backdrop_480x320",lambda im:im.convert("RGB").resize((480,320),Image.Resampling.NEAREST),rgb565_payload),
    }
    for name,(count,total) in results.items():print(f"{name}: count={count} payload={total}")
    print(f"combined={sum(total for _,total in results.values())}")

if __name__=="__main__":main()
