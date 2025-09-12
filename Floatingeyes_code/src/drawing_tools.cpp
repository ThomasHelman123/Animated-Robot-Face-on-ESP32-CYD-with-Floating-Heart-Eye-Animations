#include "drawing_tools.h"
#include "img_rle.h"

uint16_t rgb332_to_rgb565(uint8_t rgb332) {
    uint8_t r = (rgb332 & 0xE0) >> 5;
    uint8_t g = (rgb332 & 0x1C) >> 2;
    uint8_t b = (rgb332 & 0x03);
    return ((r * 255 / 7) & 0xF8) << 8 | ((g * 255 / 7) & 0xFC) << 3 | ((b * 255 / 3) >> 3);
}

void draw_image_rle_conv(lgfx::LGFX_Device* gfx, const uint8_t* image_rle, int image_rle_length, int h, int w) {
    int x = 0, y = 0;
    uint32_t index_img = 0;
    for (uint32_t ind_rle = 0; ind_rle < image_rle_length; ind_rle += 2) {
        uint8_t count = image_rle[ind_rle];
        uint8_t val_b = image_rle[ind_rle + 1];
        uint16_t val = rgb332_to_rgb565(val_b);
        y = index_img / w;
        x = index_img % w;
        gfx->drawFastHLine(x, y, count, val);
        index_img += count;
    }
}

// Overdraws only the eyes region with face background (for animation)
void draw_face_eyes_region(lgfx::LGFX_Device* gfx, int y_start, int y_end) {
    int x = 0, y = 0;
    uint32_t index_img = 0;
    for (uint32_t ind_rle = 0; ind_rle < img_rle_0_length; ind_rle += 2) {
        uint8_t count = img_rle_0[ind_rle];
        uint8_t val_b = img_rle_0[ind_rle + 1];
        uint16_t val = rgb332_to_rgb565(val_b);
        y = index_img / img_width;
        x = index_img % img_width;
        if (y >= y_start && y < y_end) {
            gfx->drawFastHLine(x, y, count, val);
        }
        index_img += count;
        if (y > y_end) break;
    }
}