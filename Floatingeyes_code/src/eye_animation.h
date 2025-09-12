#pragma once
#include <LovyanGFX.hpp>

// Adjust for your display and face art:
#define EYE_LEFT_X 85
#define EYE_RIGHT_X 155
#define EYE_Y 135
#define EYE_RADIUS 30
#define EYE_BORDER 8

void setup_eyes();
void loop_eyes_band(lgfx::LGFX_Device* gfx);

/*#ifndef EYE_ANIMATION_H
#define EYE_ANIMATION_H

#include <LovyanGFX.hpp>
#include "LGFX.hpp"

void drawBaseFace(LGFX &gfx);
void drawEyes(LGFX &gfx, int frame);
void captureEyesBackground(LGFX &gfx);
void restoreEyesBackground(LGFX &gfx);
void toggleEyeMode();

#endif

*/

