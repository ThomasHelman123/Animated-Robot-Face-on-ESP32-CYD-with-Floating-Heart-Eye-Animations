#include <math.h>
#include <LovyanGFX.hpp>

#define DISPLAY_W 230
#define DISPLAY_H 240

#define EYE_RADIUS    24
#define EYE_BORDER     4
#define EYE_GAP       90

#define EYE_Y        160
#define EYE_LEFT_X   (DISPLAY_W/2 - EYE_GAP/2)
#define EYE_RIGHT_X  (DISPLAY_W/2 + EYE_GAP/2)
#define EYE_BAND_MARGIN 1

#define BOOT_PIN 0

struct GazePoint {
    int dx;
    int dy;
};

GazePoint gazes[] = {
    {  0,   0},
    {-12,   0},
    { 12,   0},
    {  0,  10},
    {  0, -10},
};

const int NUM_GAZES = sizeof(gazes)/sizeof(GazePoint);
const int GAZE_PAUSE_MS = 3000;
const int GAZE_MOVE_MS  = 700;

static int current_gaze = 0;
static int next_gaze = 0;
static unsigned long gaze_timer = 0;
static bool in_motion = false;

static bool heart_mode = false;
static bool last_boot_state = true;
static unsigned long last_toggle_time = 0;
static bool need_eye_clear = false;

void setup_eyes() {
    pinMode(BOOT_PIN, INPUT_PULLUP);
    current_gaze = 0;
    next_gaze = 0;
    gaze_timer = millis();
    in_motion = false;
    heart_mode = false;
    last_boot_state = true;
    last_toggle_time = millis();
    need_eye_clear = false;
}

float anim_dx = 0, anim_dy = 0;

// Fill a larger area over the eye to guarantee full coverage, but not too huge
void clear_eye_area(lgfx::LGFX_Device* gfx, int cx, int cy, uint16_t bg_col) {
    int clear_r = EYE_RADIUS + EYE_BORDER + 8; // just enough to cover any blue
    gfx->fillRect(cx - clear_r, cy - clear_r, clear_r * 2, clear_r * 2, bg_col);
    gfx->fillCircle(cx, cy, clear_r, bg_col);
}

// Full vector heart shape (parametric heart curve)
void draw_full_heart(lgfx::LGFX_Device* gfx, int cx, int cy, float scale, uint16_t color) {
    float s = scale * (EYE_RADIUS - 7);
    int px[32], py[32];
    for (int i = 0; i < 32; ++i) {
        float t = 3.14159f * 2.0f * i / 32.0f;
        float x = 16.0f * powf(sinf(t), 3.0f);
        float y = 13.0f * cosf(t) - 5.0f * cosf(2.0f * t) - 2.0f * cosf(3.0f * t) - cosf(4.0f * t);
        px[i] = cx + int(s * x / 17.0f);
        py[i] = cy - int(s * y / 17.0f);
    }
    for (int i = 1; i < 31; ++i)
        gfx->fillTriangle(cx, cy, px[i], py[i], px[i+1], py[i+1], color);
    gfx->fillTriangle(cx, cy, px[31], py[31], px[0], py[0], color);
}

void loop_eyes_band(lgfx::LGFX_Device* gfx)
{
    unsigned long now = millis();

    // Debounced button toggle (200ms debounce)
    bool boot_now = (digitalRead(BOOT_PIN) == LOW);
    if (boot_now != last_boot_state && now - last_toggle_time > 200) {
        if (boot_now) {
            heart_mode = !heart_mode;
            last_toggle_time = now;
            need_eye_clear = heart_mode;
        }
    }
    last_boot_state = boot_now;

    // Set your face background color here!
    uint16_t bg_col = gfx->color565(0, 0, 0);

    if (!heart_mode) {
        // Eyes looking around (original code)
        static float anim_dx = 0;
        static float anim_dy = 0;

        if (!in_motion && (now - gaze_timer > GAZE_PAUSE_MS)) {
            next_gaze = (current_gaze + 1) % NUM_GAZES;
            gaze_timer = now;
            in_motion = true;
        }

        if (in_motion) {
            float t = float(now - gaze_timer) / float(GAZE_MOVE_MS);
            if (t >= 1.0f) {
                t = 1.0f;
                in_motion = false;
                current_gaze = next_gaze;
                gaze_timer = now;
            }
            anim_dx = gazes[current_gaze].dx + (gazes[next_gaze].dx - gazes[current_gaze].dx) * t;
            anim_dy = gazes[current_gaze].dy + (gazes[next_gaze].dy - gazes[current_gaze].dy) * t;
        } else {
            anim_dx = gazes[current_gaze].dx;
            anim_dy = gazes[current_gaze].dy;
        }

        int ex1 = EYE_LEFT_X  + int(anim_dx);
        int ey1 = EYE_Y       + int(anim_dy);
        int ex2 = EYE_RIGHT_X + int(anim_dx);
        int ey2 = EYE_Y       + int(anim_dy);

        int band_y = EYE_Y - EYE_RADIUS - EYE_BORDER - EYE_BAND_MARGIN;
        int band_h = 2 * (EYE_RADIUS + EYE_BORDER + EYE_BAND_MARGIN);

        int band_x = EYE_LEFT_X - 12 - EYE_RADIUS - EYE_BORDER - EYE_BAND_MARGIN;
        int band_w = (EYE_RIGHT_X + 12 + EYE_RADIUS + EYE_BORDER + EYE_BAND_MARGIN) - band_x;

        if (band_x < 17) band_x = 17;
        if (band_y < 50) band_y = 50;
        if (band_x + band_w > DISPLAY_W-17) band_w = DISPLAY_W-17 - band_x;
        if (band_y + band_h > DISPLAY_H-40) band_h = DISPLAY_H-40 - band_y;

        uint16_t scanline[band_w];

        uint16_t border_col = gfx->color565(255, 0, 0);
        uint16_t fill_col   = gfx->color565(120, 0, 0);

        for (int dy_i = 0; dy_i < band_h; ++dy_i) {
            int y = band_y + dy_i;
            gfx->readRect(band_x, y, band_w, 1, scanline);

            for (int dx_i = 0; dx_i < band_w; ++dx_i) {
                int x = band_x + dx_i;

                int rx1 = x - ex1;
                int ry1 = y - ey1;
                int r2 = EYE_RADIUS * EYE_RADIUS;
                int rb2 = (EYE_RADIUS + EYE_BORDER) * (EYE_RADIUS + EYE_BORDER);
                bool in_eye1 = (rx1 * rx1 + ry1 * ry1 <= r2);
                bool in_eye1_border = (rx1 * rx1 + ry1 * ry1 <= rb2) && !in_eye1;

                int rx2 = x - ex2;
                int ry2 = y - ey2;
                bool in_eye2 = (rx2 * rx2 + ry2 * ry2 <= r2);
                bool in_eye2_border = (rx2 * rx2 + ry2 * ry2 <= rb2) && !in_eye2;

                if (in_eye1_border || in_eye2_border) {
                    scanline[dx_i] = border_col;
                } else if (in_eye1 || in_eye2) {
                    scanline[dx_i] = fill_col;
                }
            }
            gfx->pushImage(band_x, y, band_w, 1, scanline);
        }
    } else {
        if (need_eye_clear) {
            clear_eye_area(gfx, EYE_LEFT_X, EYE_Y, bg_col);
            clear_eye_area(gfx, EYE_RIGHT_X, EYE_Y, bg_col);
            need_eye_clear = false;
        }
        // Dramatic, smooth sinusoidal heart pulse (from small to big to small)
        static float heart_phase = 0.0f;
        static unsigned long heart_last = 0;
        unsigned long now2 = millis();
        if (now2 - heart_last > 32) { // ~30 fps for less flicker
            heart_last = now2;
            heart_phase += 0.035f; // Slower for more obvious effect
            if (heart_phase > 6.283185f) heart_phase -= 6.283185f;
        }
        // min scale 0.5, max 1.5 (center at 1.0, amplitude 0.5)
        float heart_scale = 1.0f + 0.5f * sinf(heart_phase);

        draw_full_heart(gfx, EYE_LEFT_X,  EYE_Y, heart_scale, gfx->color565(255,0,0));
        draw_full_heart(gfx, EYE_RIGHT_X, EYE_Y, heart_scale, gfx->color565(255,0,0));
    }
} 