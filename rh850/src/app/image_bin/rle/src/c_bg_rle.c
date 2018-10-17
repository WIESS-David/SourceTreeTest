#include "r_typedefs.h"
#include "img_format.h"

static uint8_t * locImgPtr = (uint8_t*)0;

const Img_t Img_c_bg_rle = {
    "c_bg_rle.bin",
    0,
    1312,
    480,
    IMG_RGB888,
    IMG_ATTRIBUTE_RLE_DECOMPRESS | IMG_ATTRIBUTE_USE_FROM_PERSISTENT_MEMORY,
    0,
    0,
    0,
    &locImgPtr,
    216678
};


