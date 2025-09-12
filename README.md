# Animated Robot Face on ESP32 CYD

A playful robot face running on the **Cheap Yellow Display (ESP32-2432S022)**.  
It renders a background face image and overlays **floating eyes** that can **toggle to heart eyes** with the **BOOT** button. Built with **PlatformIO** and **LovyanGFX**, driving the CYD’s built-in 2.4″ parallel TFT at smooth, flicker-free frame rates.

---

## Features 

- **Fast background blitter (RLE → RGB565)**  
  `img_rle.h` stores the face in **RGB332 run-length encoding**.  
  `drawing_tools.cpp` converts each value to **RGB565** (`rgb332_to_rgb565`) and draws
  runs via `drawFastHLine()` → small memory footprint + quick full-screen draw.

- **Flicker-free eye animation (band compositor)**  
  `loop_eyes_band()` updates only a **narrow raster band** around the eyes:  
  `readRect()` → composite pupils/borders in RAM → `pushImage()` back line-by-line.
  This avoids full refreshes and yields **30–60 FPS** visuals.

- **Natural “look-around” gaze engine**  
  A tiny state machine steps through `gazes[]` with timed interpolation
  (`GAZE_MOVE_MS`, `GAZE_PAUSE_MS`) for smooth motion.

- **Heart-eyes with pulse**  
  BOOT toggles `heart_mode` (debounced).  
  `draw_full_heart()` renders a **parametric heart** filled by triangles, scaled
  sinusoidally for a clear pulsing effect.

- **Clean overdraw on mode switch**  
  `clear_eye_area()` restores the background beneath each eye before
  drawing hearts to prevent artifacts.

---

## CYD TFT Pin Configuration (`LGFX.hpp`)

```text
WR  → GPIO 4
RD  → GPIO 2
RS  → GPIO 16
D0  → GPIO 15
D1  → GPIO 13
D2  → GPIO 12
D3  → GPIO 14
D4  → GPIO 27
D5  → GPIO 25
D6  → GPIO 33
D7  → GPIO 32
CS  → GPIO 17
