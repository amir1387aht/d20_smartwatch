#include "app/app.h"

#define CIRCLE_RADIUS    60

// Needle configuration
#define MAX_NEEDLES      30
#define ANGLE_STEPS      24       // 360° / 15° increments
#define SPIN_STEPS       1        // rotation steps per frame
#define FRAME_DELAY_MS   45
#define COLLISION_STEPS  1        // minimum angular separation

// Needle geometry offsets (outside circle)
static const int8_t base_dx[ANGLE_STEPS] = {  64,  62,  56,  45,  33,  18,   4, -18,
                                             -33, -45, -56, -62, -64, -62, -56, -45,
                                             -33, -18,   4,  18,  33,  45,  56,  62 };
static const int8_t base_dy[ANGLE_STEPS] = {   0,  18,  33,  45,  56,  62,  64,  62,
                                              56,  45,  33,  18,   0, -18, -33, -45,
                                             -56, -62, -64, -62, -56, -45, -33, -18 };
// Extended one pixel radially for a slightly longer needle
static const int8_t tip_dx[ANGLE_STEPS]  = {  73,  71,  64,  53,  39,  21,   4, -21,
                                             -39, -53, -64, -71, -73, -71, -64, -53,
                                             -39, -21,   4,  21,  39,  53,  64,  71 };
static const int8_t tip_dy[ANGLE_STEPS]  = {   0,  21,  39,  53,  64,  71,  73,  71,
                                              64,  53,  39,  21,   0, -21, -39, -53,
                                             -64, -71, -73, -71, -64, -53, -39, -21 };

// Game states
typedef enum { STATE_PLAY, STATE_FLASH } game_state_t;

// Global state
static uint8_t      needles[MAX_NEEDLES];  // stored (unspun) angle indices
static int          num_needles;
static int          num_needles_cache;
static uint8_t      spin_index;
static bool         last_btn;
static game_state_t state;
static int          flash_count;
static bool         flash_on;

// Helpers
static inline int wrap_idx(int i) {
    i %= ANGLE_STEPS;
    return (i < 0) ? i + ANGLE_STEPS : i;
}
static inline int wrap_diff(int a, int b) {
    int d = a - b;
    if (d >  ANGLE_STEPS/2)  d -= ANGLE_STEPS;
    if (d < -ANGLE_STEPS/2)  d += ANGLE_STEPS;
    return d;
}
static bool check_collision(int idx) {
    for (int i = 0; i < num_needles; i++) {
        if (abs(wrap_diff(idx, needles[i])) < COLLISION_STEPS) 
            return true;
    }
    return false;
}

// Draw the static indicator just above the circle
static void draw_indicator(int cx, int cy) {
    int y0 = cy - CIRCLE_RADIUS - 8;
    int y1 = cy - CIRCLE_RADIUS - 2;
    gfx_draw_line(cx, y0, cx, y1, ST77XX_BLACK);
}

// Draw one needle, 3 px thick
static void draw_thick_needle(int cx, int cy, int idx, uint16_t color) {
    int bx = cx + base_dx[idx], by = cy + base_dy[idx];
    int tx = cx + tip_dx[idx],  ty = cy + tip_dy[idx];
    // simple 3‑pixel thickness: draw the central line plus offsets ±1 in X and Y
    for (int t = -1; t <= 1; t++) {
        gfx_draw_line(bx + t, by,     tx + t, ty,     color);
        gfx_draw_line(bx,     by + t, tx,     ty + t, color);
    }
}

void app_init(void) {
    gfx_init(DISPLAY_WIDTH, DISPLAY_HEIGHT, 2);

    gfx_set_font(&FreeMono9pt7b);
    gfx_set_text_color(ST77XX_BLACK);
    gfx_set_text_size(1);
    gfx_set_text_wrap(false);
    gfx_set_cursor(5, 5);

    // Start with a clean white screen + big black circle + top indicator
    gfx_fill_screen(ST77XX_WHITE);
    gfx_fill_circle(DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2, CIRCLE_RADIUS, ST77XX_BLACK);
    draw_indicator(DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2);

    // Reset state
    num_needles = 0;
    spin_index  = 0;
    last_btn    = false;
    state       = STATE_PLAY;
    flash_count = 0;
    flash_on    = false;
}

void app_update(void) {
    int cx = DISPLAY_WIDTH/2, cy = DISPLAY_HEIGHT/2;
    bool btn = read_button_state();

    if (state == STATE_PLAY) {
        // 1) On button-press, queue up a new needle so that when drawn
        //    it lands at the *top* of the circle (world index = 18)
        if (btn && !last_btn) {
            int top_world = ANGLE_STEPS * 3 / 4;
            int new_idx   = wrap_idx(top_world - spin_index);
            if (check_collision(new_idx) || num_needles >= MAX_NEEDLES) {
                // collision → flash
                state       = STATE_FLASH;
                flash_count = 0;
                flash_on    = false;
            } else {
                needles[num_needles++] = new_idx;
            }
        }
        last_btn = btn;

        // 2) advance spinner
        uint8_t old_spin = spin_index;
        spin_index = wrap_idx(spin_index + SPIN_STEPS);

        // Erase each needle at its old position
        for (int i = 0; i < num_needles; i++) {
            int old_idx = wrap_idx(needles[i] + old_spin);
            draw_thick_needle(cx, cy, old_idx, ST77XX_WHITE);
        }
        // Draw each needle at its new position
        for (int i = 0; i < num_needles; i++) {
            int new_idx = wrap_idx(needles[i] + spin_index);
            draw_thick_needle(cx, cy, new_idx, ST77XX_BLACK);
        }

        // Display score in top-left
        // Clear previous score area
        if(num_needles != num_needles_cache) {
            gfx_fill_rect(0, 0, 30, 30, ST77XX_WHITE);
            
            // Replace int_to_str with sprintf
            char score_str[4];
            sprintf(score_str, "%d", num_needles);
            
            gfx_set_cursor(5, 5);
            gfx_print(score_str);
            num_needles_cache = num_needles;
        }

    } else {
        // FLASH state (as before)
        flash_on = !flash_on;
        gfx_fill_screen(flash_on ? ST77XX_RED : ST77XX_WHITE);
        flash_count++;
        if (flash_count >= 6) {
            // back to PLAY
            gfx_fill_screen(ST77XX_WHITE);
            gfx_fill_circle(cx, cy, CIRCLE_RADIUS, ST77XX_BLACK);
            draw_indicator(cx, cy);
            num_needles = 0;
            spin_index  = 0;
            last_btn    = false;
            state       = STATE_PLAY;
        }
    }

    delay_ms(FRAME_DELAY_MS);
}