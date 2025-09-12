//#pragma once
//#include <LovyanGFX.hpp>

//void drawBaseFace(lgfx::LGFX_Device* gfx);
//void drawEyes(lgfx::LGFX_Device* gfx, int frame);


#pragma once
#include <LovyanGFX.hpp>
#include <stdint.h>

void draw_face_eyes_region(lgfx::LGFX_Device* gfx, int y_start, int y_end);
void draw_image_rle_conv(lgfx::LGFX_Device* gfx, const uint8_t* image_rle, int image_rle_length, int h, int w);
uint16_t rgb332_to_rgb565(uint8_t rgb332);