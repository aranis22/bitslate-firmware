/*******************************************************************************
 * Size: 24 px
 * Bpp: 1
 * Opts: --font src/assets/fonts/monogram.ttf --range 32-126 --size 24 --bpp 1 --format lvgl --force-fast-kern-format -o src/assets/fonts/generated/monogram_24.c
 ******************************************************************************/

#include "lvgl.h"

#ifndef MONOGRAM_24
#define MONOGRAM_24 1
#endif

#if MONOGRAM_24

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */
    0x0,

    /* U+0021 "!" */
    0xff, 0xff, 0xc,

    /* U+0022 "\"" */
    0xde, 0xf7, 0xbd, 0x80,

    /* U+0023 "#" */
    0x6c, 0xff, 0x6c, 0x6c, 0x6c, 0x6c, 0xff, 0x6c,
    0x6c,

    /* U+0024 "$" */
    0x18, 0x18, 0x3f, 0xd8, 0x58, 0x3c, 0x1b, 0x18,
    0xfc, 0x18, 0x18,

    /* U+0025 "%" */
    0xc3, 0xc3, 0xc3, 0x4, 0x0, 0x18, 0x20, 0x0,
    0xc3, 0xc3, 0xc3,

    /* U+0026 "&" */
    0x38, 0xc4, 0xc4, 0xc4, 0x4, 0x3f, 0xc4, 0xc4,
    0xc4, 0x0, 0x3b,

    /* U+0027 "'" */
    0xff, 0xc0,

    /* U+0028 "(" */
    0x3b, 0x6d, 0xb6, 0xc0, 0x80,

    /* U+0029 ")" */
    0xc4, 0x92, 0x49, 0x23, 0x0,

    /* U+002A "*" */
    0x18, 0x18, 0xdb, 0x3c, 0x5c, 0xdb, 0x18, 0x18,

    /* U+002B "+" */
    0x18, 0x18, 0x18, 0xff, 0x18, 0x18, 0x18, 0x18,

    /* U+002C "," */
    0x24, 0xe0,

    /* U+002D "-" */
    0xff,

    /* U+002E "." */
    0xfc,

    /* U+002F "/" */
    0x3, 0x3, 0x3, 0x4, 0x0, 0x18, 0x20, 0x0,
    0xc0, 0xc0, 0xc0,

    /* U+0030 "0" */
    0x3c, 0xc3, 0xc3, 0xc7, 0xcb, 0xdb, 0xe3, 0xc3,
    0xc3, 0x0, 0x3c,

    /* U+0031 "1" */
    0x18, 0x18, 0x78, 0x18, 0x18, 0x18, 0x18, 0x18,
    0x18, 0x18, 0xff,

    /* U+0032 "2" */
    0x3c, 0xc3, 0x3, 0x3, 0x0, 0x4, 0x18, 0x20,
    0x20, 0x20, 0xff,

    /* U+0033 "3" */
    0x3c, 0xc3, 0x3, 0x3, 0x0, 0x1c, 0x3, 0x3,
    0xc3, 0x0, 0x3c,

    /* U+0034 "4" */
    0x23, 0x23, 0x23, 0xc3, 0xc3, 0xff, 0x3, 0x3,
    0x3, 0x3, 0x3,

    /* U+0035 "5" */
    0xff, 0xc0, 0xc0, 0xfc, 0x3, 0x3, 0x3, 0x3,
    0xc3, 0x0, 0x3c,

    /* U+0036 "6" */
    0x3c, 0xc0, 0xc0, 0xc0, 0xc0, 0xfc, 0xc3, 0xc3,
    0xc3, 0x0, 0x3c,

    /* U+0037 "7" */
    0xff, 0x3, 0x3, 0x3, 0x3, 0x4, 0x18, 0x18,
    0x18, 0x18, 0x18,

    /* U+0038 "8" */
    0x3c, 0xc3, 0xc3, 0xc3, 0x0, 0x3c, 0xc3, 0xc3,
    0xc3, 0x0, 0x3c,

    /* U+0039 "9" */
    0x3c, 0xc3, 0xc3, 0xc3, 0x3, 0x3f, 0x3, 0x3,
    0xc3, 0x0, 0x3c,

    /* U+003A ":" */
    0xfc, 0x3, 0xf0,

    /* U+003B ";" */
    0x24, 0x80, 0x1, 0x27, 0x0,

    /* U+003C "<" */
    0x7, 0x0, 0x38, 0xc0, 0x0, 0x38, 0x0, 0x7,

    /* U+003D "=" */
    0xff, 0x0, 0x0, 0xff,

    /* U+003E ">" */
    0xe0, 0x0, 0x1c, 0x3, 0x0, 0x1c, 0x0, 0xe0,

    /* U+003F "?" */
    0x3c, 0xc3, 0x3, 0x3, 0x0, 0x4, 0x18, 0x0,
    0x0, 0x0, 0x18,

    /* U+0040 "@" */
    0x3c, 0xc7, 0xc7, 0xdb, 0xdb, 0xdb, 0xc7, 0xc0,
    0xc0, 0x0, 0x3c,

    /* U+0041 "A" */
    0x3c, 0x0, 0xc3, 0xc3, 0xc3, 0xc3, 0xff, 0xc3,
    0xc3, 0xc3, 0xc3,

    /* U+0042 "B" */
    0xfc, 0xc3, 0xc3, 0xc3, 0xc0, 0xfc, 0xc3, 0xc3,
    0xc3, 0xc4, 0xfc,

    /* U+0043 "C" */
    0x3c, 0x0, 0xc3, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
    0xc3, 0x0, 0x3c,

    /* U+0044 "D" */
    0xfc, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3,
    0xc3, 0xc4, 0xfc,

    /* U+0045 "E" */
    0xff, 0xc0, 0xc0, 0xc0, 0xc0, 0xfc, 0xc0, 0xc0,
    0xc0, 0xc0, 0xff,

    /* U+0046 "F" */
    0xff, 0xc0, 0xc0, 0xc0, 0xc0, 0xfc, 0xc0, 0xc0,
    0xc0, 0xc0, 0xc0,

    /* U+0047 "G" */
    0x3c, 0xc3, 0xc0, 0xc0, 0xc0, 0xdf, 0xc3, 0xc3,
    0xc3, 0x0, 0x3c,

    /* U+0048 "H" */
    0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xff, 0xc3, 0xc3,
    0xc3, 0xc3, 0xc3,

    /* U+0049 "I" */
    0xff, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
    0x18, 0x18, 0xff,

    /* U+004A "J" */
    0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0xc3, 0xc3,
    0xc3, 0x0, 0x3c,

    /* U+004B "K" */
    0xc3, 0xc0, 0xc4, 0xd8, 0xc0, 0xe0, 0xd8, 0xc8,
    0xc4, 0xc0, 0xc3,

    /* U+004C "L" */
    0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
    0xc0, 0xc0, 0xff,

    /* U+004D "M" */
    0xc3, 0xc3, 0xe7, 0xdb, 0xc3, 0xc3, 0xc3, 0xc3,
    0xc3, 0xc3, 0xc3,

    /* U+004E "N" */
    0xc3, 0xc3, 0xc3, 0xe3, 0xe3, 0xdb, 0xc7, 0xc3,
    0xc3, 0xc3, 0xc3,

    /* U+004F "O" */
    0x3c, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3,
    0xc3, 0x0, 0x3c,

    /* U+0050 "P" */
    0xfc, 0xc3, 0xc3, 0xc3, 0xc0, 0xfc, 0xc0, 0xc0,
    0xc0, 0xc0, 0xc0,

    /* U+0051 "Q" */
    0x3c, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3,
    0xc3, 0x0, 0x3c, 0x7,

    /* U+0052 "R" */
    0xfc, 0xc3, 0xc3, 0xc3, 0xc0, 0xfc, 0xc3, 0xc3,
    0xc3, 0xc3, 0xc3,

    /* U+0053 "S" */
    0x3c, 0xc3, 0xc0, 0xc0, 0x0, 0x3c, 0x3, 0x3,
    0xc3, 0x0, 0x3c,

    /* U+0054 "T" */
    0xff, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
    0x18, 0x18, 0x18,

    /* U+0055 "U" */
    0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3,
    0xc3, 0x0, 0x3c,

    /* U+0056 "V" */
    0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0x24, 0x24,
    0x24, 0x0, 0x18,

    /* U+0057 "W" */
    0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xdb, 0xc3,
    0xe7, 0xc3, 0xc3,

    /* U+0058 "X" */
    0xc3, 0xc3, 0xc3, 0x24, 0x0, 0x18, 0x24, 0x0,
    0xc3, 0xc3, 0xc3,

    /* U+0059 "Y" */
    0xc3, 0xc3, 0xc3, 0x24, 0x0, 0x18, 0x18, 0x18,
    0x18, 0x18, 0x18,

    /* U+005A "Z" */
    0xff, 0x3, 0x3, 0x4, 0x0, 0x18, 0x20, 0xc0,
    0xc0, 0xc0, 0xff,

    /* U+005B "[" */
    0xfb, 0x6d, 0xb6, 0xdb, 0x80,

    /* U+005C "\\" */
    0xc0, 0xc0, 0xc0, 0x20, 0x0, 0x18, 0x4, 0x0,
    0x3, 0x3, 0x3,

    /* U+005D "]" */
    0xf3, 0x33, 0x33, 0x33, 0x33, 0xf0,

    /* U+005E "^" */
    0x18, 0x24, 0x0, 0xc3,

    /* U+005F "_" */
    0xff,

    /* U+0060 "`" */
    0xc0, 0x80,

    /* U+0061 "a" */
    0x3f, 0x43, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0x3f,

    /* U+0062 "b" */
    0xc0, 0xc0, 0xc0, 0xfc, 0xc4, 0xc3, 0xc3, 0xc3,
    0xc3, 0xc3, 0xfc,

    /* U+0063 "c" */
    0x3c, 0x0, 0xc3, 0xc0, 0xc0, 0xc3, 0x0, 0x3c,

    /* U+0064 "d" */
    0x3, 0x3, 0x3, 0x3f, 0x43, 0xc3, 0xc3, 0xc3,
    0xc3, 0xc3, 0x3f,

    /* U+0065 "e" */
    0x3c, 0x0, 0xc3, 0xff, 0xc0, 0xc0, 0xc0, 0x3c,

    /* U+0066 "f" */
    0x1c, 0x0, 0x63, 0x60, 0x60, 0xfc, 0x60, 0x60,
    0x60, 0x60, 0x60,

    /* U+0067 "g" */
    0x3f, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0x43, 0x3f,
    0x3, 0x3, 0x3c,

    /* U+0068 "h" */
    0xc0, 0xc0, 0xc0, 0xfc, 0xc0, 0xc3, 0xc3, 0xc3,
    0xc3, 0xc3, 0xc3,

    /* U+0069 "i" */
    0x18, 0x0, 0x0, 0x78, 0x18, 0x18, 0x18, 0x18,
    0x18, 0x18, 0xff,

    /* U+006A "j" */
    0x3, 0x0, 0x0, 0x7, 0x3, 0x3, 0x3, 0x3,
    0x3, 0x3, 0x3, 0xc3, 0x0, 0x3c,

    /* U+006B "k" */
    0xc0, 0xc0, 0xc0, 0xc3, 0xc0, 0xcc, 0xf8, 0xc8,
    0xcc, 0xc0, 0xc3,

    /* U+006C "l" */
    0xf0, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
    0x30, 0x30, 0xf,

    /* U+006D "m" */
    0xfc, 0xd8, 0xdb, 0xdb, 0xdb, 0xdb, 0xdb, 0xdb,

    /* U+006E "n" */
    0xfc, 0xc0, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3,

    /* U+006F "o" */
    0x3c, 0x0, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0x3c,

    /* U+0070 "p" */
    0xfc, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc0, 0xfc,
    0xc0, 0xc0, 0xc0,

    /* U+0071 "q" */
    0x3f, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0x43, 0x3f,
    0x3, 0x3, 0x3,

    /* U+0072 "r" */
    0xdc, 0xc0, 0xe3, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,

    /* U+0073 "s" */
    0x3f, 0x0, 0xc0, 0x3c, 0x0, 0x3, 0x0, 0xfc,

    /* U+0074 "t" */
    0x60, 0x60, 0x60, 0xfc, 0x60, 0x60, 0x60, 0x60,
    0x60, 0x0, 0x1f,

    /* U+0075 "u" */
    0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0x3, 0x3f,

    /* U+0076 "v" */
    0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0x24, 0x0, 0x18,

    /* U+0077 "w" */
    0xc3, 0xc3, 0xc3, 0xdb, 0xdb, 0xdb, 0x0, 0x24,

    /* U+0078 "x" */
    0xc3, 0x0, 0x24, 0x18, 0x0, 0x24, 0x0, 0xc3,

    /* U+0079 "y" */
    0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0x3, 0x3f,
    0x3, 0x0, 0x3c,

    /* U+007A "z" */
    0xff, 0x4, 0x4, 0x18, 0x60, 0x60, 0x60, 0xff,

    /* U+007B "{" */
    0xc, 0x82, 0x8, 0x3, 0x2, 0x8, 0x20, 0x0,
    0xc0,

    /* U+007C "|" */
    0xff, 0xff, 0xfc,

    /* U+007D "}" */
    0xc0, 0x82, 0x8, 0x0, 0x32, 0x8, 0x20, 0xc,
    0x0,

    /* U+007E "~" */
    0x23, 0x0, 0xdc
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 144, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 144, .box_w = 2, .box_h = 11, .ofs_x = 3, .ofs_y = 0},
    {.bitmap_index = 4, .adv_w = 144, .box_w = 5, .box_h = 5, .ofs_x = 1, .ofs_y = 6},
    {.bitmap_index = 8, .adv_w = 144, .box_w = 8, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 17, .adv_w = 144, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 28, .adv_w = 144, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 39, .adv_w = 144, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 50, .adv_w = 144, .box_w = 2, .box_h = 5, .ofs_x = 3, .ofs_y = 6},
    {.bitmap_index = 52, .adv_w = 144, .box_w = 3, .box_h = 11, .ofs_x = 3, .ofs_y = 0},
    {.bitmap_index = 57, .adv_w = 144, .box_w = 3, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 62, .adv_w = 144, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 70, .adv_w = 144, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 78, .adv_w = 144, .box_w = 3, .box_h = 4, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 80, .adv_w = 144, .box_w = 8, .box_h = 1, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 81, .adv_w = 144, .box_w = 2, .box_h = 3, .ofs_x = 3, .ofs_y = 0},
    {.bitmap_index = 82, .adv_w = 144, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 93, .adv_w = 144, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 104, .adv_w = 144, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 115, .adv_w = 144, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 126, .adv_w = 144, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 137, .adv_w = 144, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 148, .adv_w = 144, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 159, .adv_w = 144, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 170, .adv_w = 144, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 181, .adv_w = 144, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 192, .adv_w = 144, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 203, .adv_w = 144, .box_w = 2, .box_h = 10, .ofs_x = 3, .ofs_y = 0},
    {.bitmap_index = 206, .adv_w = 144, .box_w = 3, .box_h = 11, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 211, .adv_w = 144, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 219, .adv_w = 144, .box_w = 8, .box_h = 4, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 223, .adv_w = 144, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 231, .adv_w = 144, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 242, .adv_w = 144, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 253, .adv_w = 144, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 264, .adv_w = 144, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 275, .adv_w = 144, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 286, .adv_w = 144, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 297, .adv_w = 144, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 308, .adv_w = 144, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 319, .adv_w = 144, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 330, .adv_w = 144, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 341, .adv_w = 144, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 352, .adv_w = 144, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 363, .adv_w = 144, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 374, .adv_w = 144, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 385, .adv_w = 144, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 396, .adv_w = 144, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 407, .adv_w = 144, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 418, .adv_w = 144, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 429, .adv_w = 144, .box_w = 8, .box_h = 12, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 441, .adv_w = 144, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 452, .adv_w = 144, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 463, .adv_w = 144, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 474, .adv_w = 144, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 485, .adv_w = 144, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 496, .adv_w = 144, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 507, .adv_w = 144, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 518, .adv_w = 144, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 529, .adv_w = 144, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 540, .adv_w = 144, .box_w = 3, .box_h = 11, .ofs_x = 3, .ofs_y = 0},
    {.bitmap_index = 545, .adv_w = 144, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 556, .adv_w = 144, .box_w = 4, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 562, .adv_w = 144, .box_w = 8, .box_h = 4, .ofs_x = 0, .ofs_y = 7},
    {.bitmap_index = 566, .adv_w = 144, .box_w = 8, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 567, .adv_w = 144, .box_w = 3, .box_h = 3, .ofs_x = 1, .ofs_y = 8},
    {.bitmap_index = 569, .adv_w = 144, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 577, .adv_w = 144, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 588, .adv_w = 144, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 596, .adv_w = 144, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 607, .adv_w = 144, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 615, .adv_w = 144, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 626, .adv_w = 144, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 637, .adv_w = 144, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 648, .adv_w = 144, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 659, .adv_w = 144, .box_w = 8, .box_h = 14, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 673, .adv_w = 144, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 684, .adv_w = 144, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 695, .adv_w = 144, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 703, .adv_w = 144, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 711, .adv_w = 144, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 719, .adv_w = 144, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 730, .adv_w = 144, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 741, .adv_w = 144, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 749, .adv_w = 144, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 757, .adv_w = 144, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 768, .adv_w = 144, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 776, .adv_w = 144, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 784, .adv_w = 144, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 792, .adv_w = 144, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 800, .adv_w = 144, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 811, .adv_w = 144, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 819, .adv_w = 144, .box_w = 6, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 828, .adv_w = 144, .box_w = 2, .box_h = 11, .ofs_x = 3, .ofs_y = 0},
    {.bitmap_index = 831, .adv_w = 144, .box_w = 6, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 840, .adv_w = 144, .box_w = 8, .box_h = 3, .ofs_x = 0, .ofs_y = 5}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/



/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 32, .range_length = 95, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    }
};



/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LVGL_VERSION_MAJOR == 8
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
#endif

#if LVGL_VERSION_MAJOR >= 8
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = NULL,
    .kern_scale = 0,
    .cmap_num = 1,
    .bpp = 1,
    .kern_classes = 0,
    .bitmap_format = 0,
#if LVGL_VERSION_MAJOR == 8
    .cache = &cache
#endif
};



/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LVGL_VERSION_MAJOR >= 8
const lv_font_t monogram_24 = {
#else
lv_font_t monogram_24 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 14,          /*The maximum line height required by the font*/
    .base_line = 3,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -3,
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = NULL,
#endif
    .user_data = NULL,
};



#endif /*#if MONOGRAM_24*/
