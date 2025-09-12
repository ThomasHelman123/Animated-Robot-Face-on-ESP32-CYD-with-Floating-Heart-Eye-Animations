#include <Arduino.h>
#include <LovyanGFX.hpp>
#include "LGFX.hpp"
#include "img_rle.h"
#include "drawing_tools.h"
#include "eye_animation.h"

LGFX display;

void setup() {
    display.init();
    display.setRotation(0);
    draw_image_rle_conv(&display, img_rle_0, img_rle_0_length, img_height, img_width);
    setup_eyes();
}

void loop() {
    // Efficient, flicker-free, sync eye animation using raster-band method
    loop_eyes_band(&display);
    delay(30); // adjust for smoothness and CPU usage
}
/*#include <Arduino.h>
#include "LGFX.hpp"
#include "eye_animation.h"

LGFX display;

void setup() {
  display.init();
  display.setRotation(0);
  drawBaseFace(display);
  captureEyesBackground(display); // <-- make sure this is called AFTER face is drawn!
}

void loop() {
  static int frame = 0;
  drawEyes(display, frame++);
  delay(40); // (try 30-50ms for smooth animation)
}*/