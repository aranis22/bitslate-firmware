/*******************************************************************************
 * Size: 20 px
 * Bpp: 1
 * Opts: --font src/assets/fonts/monogram.ttf --range 32-126 --size 20 --bpp 1 --format lvgl --force-fast-kern-format -o src/assets/fonts/generated/monogram_20.c
 ******************************************************************************/

#include "lvgl.h"

#ifndef MONOGRAM_20
#define MONOGRAM_20 1
#endif

#if MONOGRAM_20

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */
    0x0,

    /* U+0021 "!" */
    0xfd,

    /* U+0022 "\"" */
    0x99, 0x90,

    /* U+0023 "#" */
    0x53, 0xf5, 0x14, 0x53, 0xf5, 0x0,

    /* U+0024 "$" */
    0x11, 0xf9, 0x1e, 0x14, 0x4f, 0x84,

    /* U+0025 "%" */
    0x86, 0x10, 0x84, 0x40, 0x8, 0x61,

    /* U+0026 "&" */
    0x72, 0x28, 0x9f, 0x8a, 0x28, 0x9d,

    /* U+0027 "'" */
    0xe0,

    /* U+0028 "(" */
    0x6a, 0xa9,

    /* U+0029 ")" */
    0x95, 0x56,

    /* U+002A "*" */
    0x12, 0x57, 0xa5, 0x10, 0x40,

    /* U+002B "+" */
    0x10, 0x41, 0x3f, 0x10, 0x40,

    /* U+002C "," */
    0x58,

    /* U+002D "-" */
    0xfc,

    /* U+002E "." */
    0xc0,

    /* U+002F "/" */
    0x4, 0x10, 0x84, 0x40, 0x8, 0x20,

    /* U+0030 "0" */
    0x7a, 0x18, 0xe5, 0xe6, 0x18, 0x5e,

    /* U+0031 "1" */
    0x11, 0xc1, 0x4, 0x10, 0x41, 0x3f,

    /* U+0032 "2" */
    0x7a, 0x10, 0x42, 0x10, 0x4, 0x3f,

    /* U+0033 "3" */
    0x7a, 0x10, 0x4e, 0x0, 0x18, 0x5e,

    /* U+0034 "4" */
    0x45, 0x18, 0x7f, 0x4, 0x10, 0x41,

    /* U+0035 "5" */
    0xfe, 0xf, 0x80, 0x4, 0x18, 0x5e,

    /* U+0036 "6" */
    0x7a, 0x8, 0x3e, 0x82, 0x18, 0x5e,

    /* U+0037 "7" */
    0xfc, 0x10, 0x42, 0x0, 0x41, 0x4,

    /* U+0038 "8" */
    0x7a, 0x18, 0x5e, 0x2, 0x18, 0x5e,

    /* U+0039 "9" */
    0x7a, 0x18, 0x5f, 0x4, 0x18, 0x5e,

    /* U+003A ":" */
    0xc6,

    /* U+003B ";" */
    0x50, 0x16,

    /* U+003C "<" */
    0xd, 0xc8, 0x1c, 0xc,

    /* U+003D "=" */
    0xfc, 0xf, 0xc0,

    /* U+003E ">" */
    0xe0, 0x30, 0x8, 0xce, 0x0,

    /* U+003F "?" */
    0x7a, 0x10, 0x42, 0x10, 0x0, 0x4,

    /* U+0040 "@" */
    0x7a, 0x39, 0x65, 0x8e, 0x8, 0x1e,

    /* U+0041 "A" */
    0x7a, 0x18, 0x61, 0xfe, 0x18, 0x61,

    /* U+0042 "B" */
    0xfa, 0x18, 0x7e, 0x8a, 0x18, 0x7e,

    /* U+0043 "C" */
    0x7a, 0x18, 0x20, 0x82, 0x8, 0x5e,

    /* U+0044 "D" */
    0xfa, 0x18, 0x61, 0x86, 0x18, 0x7e,

    /* U+0045 "E" */
    0xfe, 0x8, 0x3e, 0x82, 0x8, 0x3f,

    /* U+0046 "F" */
    0xfe, 0x8, 0x3e, 0x82, 0x8, 0x20,

    /* U+0047 "G" */
    0x7a, 0x18, 0x2f, 0x86, 0x18, 0x5e,

    /* U+0048 "H" */
    0x86, 0x18, 0x7f, 0x86, 0x18, 0x61,

    /* U+0049 "I" */
    0xfc, 0x41, 0x4, 0x10, 0x41, 0x3f,

    /* U+004A "J" */
    0x4, 0x10, 0x41, 0x6, 0x18, 0x5e,

    /* U+004B "K" */
    0x86, 0x29, 0x38, 0x92, 0x48, 0xa1,

    /* U+004C "L" */
    0x82, 0x8, 0x20, 0x82, 0x8, 0x3f,

    /* U+004D "M" */
    0x83, 0xde, 0x4c, 0x18, 0x30, 0x60, 0xc1,

    /* U+004E "N" */
    0x83, 0x7, 0x8c, 0x98, 0xf0, 0x60, 0xc1,

    /* U+004F "O" */
    0x7a, 0x18, 0x61, 0x86, 0x18, 0x5e,

    /* U+0050 "P" */
    0xfa, 0x18, 0x7e, 0x82, 0x8, 0x20,

    /* U+0051 "Q" */
    0x7a, 0x18, 0x61, 0x86, 0x18, 0x5e, 0x1c,

    /* U+0052 "R" */
    0xfa, 0x18, 0x7e, 0x82, 0x18, 0x61,

    /* U+0053 "S" */
    0x7a, 0x18, 0x1e, 0x0, 0x18, 0x5e,

    /* U+0054 "T" */
    0xfc, 0x41, 0x4, 0x10, 0x41, 0x4,

    /* U+0055 "U" */
    0x86, 0x18, 0x61, 0x86, 0x18, 0x5e,

    /* U+0056 "V" */
    0x86, 0x18, 0x61, 0x85, 0x24, 0x84,

    /* U+0057 "W" */
    0x83, 0x6, 0xc, 0x19, 0x30, 0x7b, 0xc1,

    /* U+0058 "X" */
    0x86, 0x14, 0x84, 0x48, 0x8, 0x61,

    /* U+0059 "Y" */
    0x86, 0x14, 0x84, 0x10, 0x41, 0x4,

    /* U+005A "Z" */
    0xfc, 0x10, 0x84, 0x40, 0x8, 0x3f,

    /* U+005B "[" */
    0xea, 0xab,

    /* U+005C "\\" */
    0x82, 0x4, 0x4, 0x8, 0x0, 0x41,

    /* U+005D "]" */
    0xd5, 0x57,

    /* U+005E "^" */
    0x10, 0x4, 0xa1,

    /* U+005F "_" */
    0xfc,

    /* U+0060 "`" */
    0x90,

    /* U+0061 "a" */
    0x7e, 0x18, 0x61, 0x85, 0xf0,

    /* U+0062 "b" */
    0x82, 0xf, 0xa1, 0x86, 0x18, 0xbe,

    /* U+0063 "c" */
    0x7a, 0x18, 0x20, 0x85, 0xe0,

    /* U+0064 "d" */
    0x4, 0x17, 0xe1, 0x86, 0x18, 0x5f,

    /* U+0065 "e" */
    0x7a, 0x1f, 0xe0, 0x1, 0xe0,

    /* U+0066 "f" */
    0x39, 0x14, 0x3e, 0x41, 0x4, 0x10,

    /* U+0067 "g" */
    0x7e, 0x18, 0x61, 0x85, 0xf0, 0x5e,

    /* U+0068 "h" */
    0x82, 0xf, 0xa1, 0x86, 0x18, 0x61,

    /* U+0069 "i" */
    0x10, 0x7, 0x4, 0x10, 0x41, 0x3f,

    /* U+006A "j" */
    0x4, 0x0, 0xc1, 0x4, 0x10, 0x41, 0x85, 0xe0,

    /* U+006B "k" */
    0x82, 0x8, 0x62, 0xf2, 0x48, 0xa1,

    /* U+006C "l" */
    0xc1, 0x4, 0x10, 0x41, 0x4, 0xf,

    /* U+006D "m" */
    0xfd, 0x26, 0x4c, 0x99, 0x32, 0x40,

    /* U+006E "n" */
    0xfa, 0x18, 0x61, 0x86, 0x10,

    /* U+006F "o" */
    0x7a, 0x18, 0x61, 0x1, 0xe0,

    /* U+0070 "p" */
    0xfa, 0x18, 0x61, 0x83, 0xe8, 0x20,

    /* U+0071 "q" */
    0x7e, 0x18, 0x61, 0x85, 0xf0, 0x41,

    /* U+0072 "r" */
    0xbe, 0x61, 0x8, 0x40,

    /* U+0073 "s" */
    0x7e, 0x7, 0x80, 0x7, 0xe0,

    /* U+0074 "t" */
    0x20, 0x43, 0xe1, 0x2, 0x4, 0x8, 0xf,

    /* U+0075 "u" */
    0x86, 0x18, 0x61, 0x85, 0xf0,

    /* U+0076 "v" */
    0x86, 0x18, 0x40, 0x48, 0x40,

    /* U+0077 "w" */
    0x86, 0x18, 0x65, 0x95, 0x20,

    /* U+0078 "x" */
    0x85, 0x21, 0x0, 0x4a, 0x10,

    /* U+0079 "y" */
    0x86, 0x18, 0x61, 0x85, 0xf0, 0x5e,

    /* U+007A "z" */
    0xfc, 0x21, 0x0, 0x43, 0xf0,

    /* U+007B "{" */
    0x29, 0x40, 0x91,

    /* U+007C "|" */
    0xff,

    /* U+007D "}" */
    0x89, 0x10, 0x94,

    /* U+007E "~" */
    0x46, 0xe0
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 120, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 120, .box_w = 1, .box_h = 8, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 2, .adv_w = 120, .box_w = 4, .box_h = 3, .ofs_x = 1, .ofs_y = 5},
    {.bitmap_index = 4, .adv_w = 120, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 10, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 16, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 22, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 28, .adv_w = 120, .box_w = 1, .box_h = 3, .ofs_x = 2, .ofs_y = 5},
    {.bitmap_index = 29, .adv_w = 120, .box_w = 2, .box_h = 8, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 31, .adv_w = 120, .box_w = 2, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 33, .adv_w = 120, .box_w = 6, .box_h = 6, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 38, .adv_w = 120, .box_w = 6, .box_h = 6, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 43, .adv_w = 120, .box_w = 2, .box_h = 3, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 44, .adv_w = 120, .box_w = 6, .box_h = 1, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 45, .adv_w = 120, .box_w = 1, .box_h = 2, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 46, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 52, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 58, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 64, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 70, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 76, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 82, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 88, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 94, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 100, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 106, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 112, .adv_w = 120, .box_w = 1, .box_h = 7, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 113, .adv_w = 120, .box_w = 2, .box_h = 8, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 115, .adv_w = 120, .box_w = 6, .box_h = 5, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 119, .adv_w = 120, .box_w = 6, .box_h = 3, .ofs_x = 0, .ofs_y = 3},
    {.bitmap_index = 122, .adv_w = 120, .box_w = 7, .box_h = 5, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 127, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 133, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 139, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 145, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 151, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 157, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 163, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 169, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 175, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 181, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 187, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 193, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 199, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 205, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 211, .adv_w = 120, .box_w = 7, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 218, .adv_w = 120, .box_w = 7, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 225, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 231, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 237, .adv_w = 120, .box_w = 6, .box_h = 9, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 244, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 250, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 256, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 262, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 268, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 274, .adv_w = 120, .box_w = 7, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 281, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 287, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 293, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 299, .adv_w = 120, .box_w = 2, .box_h = 8, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 301, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 307, .adv_w = 120, .box_w = 2, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 309, .adv_w = 120, .box_w = 6, .box_h = 4, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 312, .adv_w = 120, .box_w = 6, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 313, .adv_w = 120, .box_w = 2, .box_h = 2, .ofs_x = 1, .ofs_y = 6},
    {.bitmap_index = 314, .adv_w = 120, .box_w = 6, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 319, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 325, .adv_w = 120, .box_w = 6, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 330, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 336, .adv_w = 120, .box_w = 6, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 341, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 347, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 353, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 359, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 365, .adv_w = 120, .box_w = 6, .box_h = 10, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 373, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 379, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 385, .adv_w = 120, .box_w = 7, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 391, .adv_w = 120, .box_w = 6, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 396, .adv_w = 120, .box_w = 6, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 401, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 407, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 413, .adv_w = 120, .box_w = 5, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 417, .adv_w = 120, .box_w = 6, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 422, .adv_w = 120, .box_w = 7, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 429, .adv_w = 120, .box_w = 6, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 434, .adv_w = 120, .box_w = 6, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 439, .adv_w = 120, .box_w = 6, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 444, .adv_w = 120, .box_w = 6, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 449, .adv_w = 120, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 455, .adv_w = 120, .box_w = 6, .box_h = 6, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 460, .adv_w = 120, .box_w = 3, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 463, .adv_w = 120, .box_w = 1, .box_h = 8, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 464, .adv_w = 120, .box_w = 3, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 467, .adv_w = 120, .box_w = 6, .box_h = 2, .ofs_x = 0, .ofs_y = 4}
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
const lv_font_t monogram_20 = {
#else
lv_font_t monogram_20 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 11,          /*The maximum line height required by the font*/
    .base_line = 2,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -2,
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = NULL,
#endif
    .user_data = NULL,
};



#endif /*#if MONOGRAM_20*/
