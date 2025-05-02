#ifndef DISPLAY_H
#define DISPLAY_H

#include "config.h"
#include "utils/utils.h"
#include "sys_utils.h"
#include "driver_pmu.h"
#include "driver_ssp.h"
#include "driver_gpio.h"

#define ST77XX_NOP        0x00
#define ST77XX_SWRESET    0x01
#define ST77XX_RDDID      0x04
#define ST77XX_RDDST      0x09

#define ST77XX_SLPIN      0x10
#define ST77XX_SLPOUT     0x11
#define ST77XX_PTLON      0x12
#define ST77XX_NORON      0x13

#define ST77XX_INVOFF     0x20
#define ST77XX_INVON      0x21
#define ST77XX_DISPOFF    0x28
#define ST77XX_DISPON     0x29
#define ST77XX_CASET      0x2A
#define ST77XX_RASET      0x2B
#define ST77XX_RAMWR      0x2C
#define ST77XX_RAMRD      0x2E

#define ST77XX_COLMOD     0x3A
#define ST77XX_MADCTL     0x36

#define ST77XX_MADCTL_MY  0x80
#define ST77XX_MADCTL_MX  0x40
#define ST77XX_MADCTL_MV  0x20
#define ST77XX_MADCTL_ML  0x10
#define ST77XX_MADCTL_RGB 0x08
#define ST77XX_MADCTL_BGR 0x00

#define ST_CMD_DELAY      0x80

#define ST77XX_BLACK 0x0000
#define ST77XX_WHITE 0xFFFF
#define ST77XX_RED 0xF800
#define ST77XX_GREEN 0x001F
#define ST77XX_BLUE 0x07E0
#define ST77XX_CYAN 0x07FF
#define ST77XX_MAGENTA 0xF81F
#define ST77XX_YELLOW 0xFFE0
#define ST77XX_ORANGE 0xFC00

#define DISPLAY_HEIGHT 240
#define DISPLAY_WIDTH 240

void display_init(uint16_t width, uint16_t height, uint8_t _rotation);
void display_fill_window(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint16_t* color_buffer, uint32_t size);
void display_set_addr_window(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
void display_fill_screen(uint16_t color);
void display_set_rotation(uint8_t m);
uint16_t display_get_color(uint8_t r, uint8_t g, uint8_t b);
void display_draw_pixel(uint16_t x, uint16_t y, uint16_t color);
void backlight_turn_off();
void backlight_turn_on();

#endif // DISPLAY_H