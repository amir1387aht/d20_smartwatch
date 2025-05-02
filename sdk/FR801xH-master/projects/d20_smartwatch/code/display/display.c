#include "display/display.h"

static uint16_t _width;       // Display width
static uint16_t _height;      // Display height
static uint8_t _rotation;     // Display rotation
static uint16_t _xstart;      // Starting X coordinate
static uint16_t _ystart;      // Starting Y coordinate
static uint8_t _colstart;     // Column offset
static uint8_t _rowstart;     // Row offset
static uint8_t _colstart2;    // Column offset for rotation
static uint8_t _rowstart2;    // Row offset for rotation

static void write_command(uint8_t cmd);
static void write_data(uint8_t data);
static void write_data16(uint16_t data);
static void set_dc_pin(uint8_t value);
static void set_reset_pin(uint8_t value);
static void cs_control(uint8_t op);

// Helper functions implementation
static void write_command(uint8_t cmd) {
    set_dc_pin(0);  // Command mode
    cs_control(SSP_CS_ENABLE);
    ssp_send_byte(cmd);
    ssp_wait_send_end();
    cs_control(SSP_CS_DISABLE);
}

static void write_data(uint8_t data) {
    set_dc_pin(1);  // Data mode
    cs_control(SSP_CS_ENABLE);
    ssp_send_byte(data);
    ssp_wait_send_end();
    cs_control(SSP_CS_DISABLE);
}

static void write_data16(uint16_t data) {
    set_dc_pin(1);  // Data mode
    cs_control(SSP_CS_ENABLE);
    
    // Send data in big-endian format (MSB first)
    uint8_t data_bytes[2] = {
        (uint8_t)(data >> 8),   // High byte
        (uint8_t)(data & 0xFF)  // Low byte
    };
    
    ssp_send_bytes(data_bytes, 2);
    ssp_wait_send_end();
    cs_control(SSP_CS_DISABLE);
}

static void set_dc_pin(uint8_t value) {
    gpio_set_pin_value(DISPLAY_DC_PIN_NAME, DISPLAY_DC_PIN_NUM, value);
}

static void set_reset_pin(uint8_t value) {
    gpio_set_pin_value(DISPLAY_RESET_PIN_NAME, DISPLAY_RESET_PIN_NUM, value);
}

static void cs_control(uint8_t op) {
    // Invert the logic - CS is typically active low
    gpio_set_pin_value(DISPLAY_CS_PIN_NAME, DISPLAY_CS_PIN_NUM, op == SSP_CS_ENABLE ? 0 : 1);
}

// Public functions

void display_init(uint16_t width, uint16_t height, uint8_t rotation) {
    // Set up the display size
    _width = width;
    _height = height;
    
    if (width == 240 && height == 240) {
        // 1.3", 1.54" displays (right justified)
        _rowstart = (320 - height);
        _rowstart2 = 0;
        _colstart = _colstart2 = (240 - width);
    } else if (width == 135 && height == 240) {
        // 1.14" display (centered, with odd size)
        _rowstart = _rowstart2 = (int)((320 - height) / 2);
        _colstart = (int)((240 - width + 1) / 2);
        _colstart2 = (int)((240 - width) / 2);
    } else {
        // 1.47", 1.69, 1.9", 2.0" displays (centered)
        _rowstart = _rowstart2 = (int)((320 - height) / 2);
        _colstart = _colstart2 = (int)((240 - width) / 2);
    }

    system_set_port_mux(DISPLAY_SCL_PIN_NAME, DISPLAY_SCL_PIN_NUM, DISPLAY_SCL_PIN_FUNC);
    system_set_port_mux(DISPLAY_SDA_PIN_NAME, DISPLAY_SDA_PIN_NUM, DISPLAY_SDA_PIN_FUNC);
    
    // Initialize SPI with 8-bit frame width, Motorola frame type, master mode
    ssp_init_(8, SSP_FRAME_MOTO, SSP_MASTER_MODE, 24000000, 2, cs_control);

    // CS
    system_set_port_mux(DISPLAY_CS_PIN_NAME, DISPLAY_CS_PIN_NUM, DISPLAY_CS_PIN_FUNC);
    gpio_set_dir(DISPLAY_CS_PIN_NAME, DISPLAY_CS_PIN_NUM, GPIO_DIR_OUT);

    // DC
    system_set_port_mux(DISPLAY_DC_PIN_NAME, DISPLAY_DC_PIN_NUM, DISPLAY_DC_PIN_FUNC);
    gpio_set_dir(DISPLAY_DC_PIN_NAME, DISPLAY_DC_PIN_NUM, GPIO_DIR_OUT);

    // Reset
    system_set_port_mux(DISPLAY_RESET_PIN_NAME, DISPLAY_RESET_PIN_NUM, DISPLAY_RESET_PIN_FUNC);
    gpio_set_dir(DISPLAY_RESET_PIN_NAME, DISPLAY_RESET_PIN_NUM, GPIO_DIR_OUT);
    
    // Hardware reset the display
    set_reset_pin(1);
    delay_ms(10);
    set_reset_pin(0);
    delay_ms(10);
    set_reset_pin(1);
    delay_ms(10);
        
    // Software Reset
    write_command(ST77XX_SWRESET);
    delay_ms(15);
    
    // Sleep Out
    write_command(ST77XX_SLPOUT);
    delay_ms(10);
    
    // Set Color Mode to 16-bit
    write_command(ST77XX_COLMOD);
    write_data(0x55);  // 16-bit color
    delay_ms(10);
    
    // Memory Access Control (directions)
    write_command(ST77XX_MADCTL);
    write_data(0x08);  // Row/col addr, bottom-top refresh
    
    // Column Address Set
    write_command(ST77XX_CASET);
    write_data(0x00);  // XSTART = 0
    write_data(0x00);
    write_data(0x00);  // XEND = 240
    write_data(0xF0);
    
    // Row Address Set
    write_command(ST77XX_RASET);
    write_data(0x00);  // YSTART = 0
    write_data(0x00);
    write_data(0x01);  // YEND = 240
    write_data(0x40);
    
    // Inversion On
    write_command(ST77XX_INVON);
    delay_ms(10);
    
    // Normal Display On
    write_command(ST77XX_NORON);
    delay_ms(10);
    
    // Display On
    write_command(ST77XX_DISPON);
    delay_ms(10);
    
    display_set_rotation(rotation);
    display_fill_screen(ST77XX_BLACK);
    backlight_turn_on();
}

void display_set_addr_window(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
    // Account for screen rotation and offsets
    x0 += _xstart;
    y0 += _ystart;
    x1 += _xstart;
    y1 += _ystart;
    
    // Set column address
    write_command(ST77XX_CASET);
    write_data16(x0);
    write_data16(x1);
    
    // Set row address
    write_command(ST77XX_RASET);
    write_data16(y0);
    write_data16(y1);
    
    // Prepare to write to display RAM
    write_command(ST77XX_RAMWR);
}

void display_fill_window(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint16_t* color_buffer, uint32_t size) {
    // Set address window
    display_set_addr_window(x0, y0, x1, y1);
    
    // Write color data
    set_dc_pin(1);  // Data mode
    cs_control(SSP_CS_ENABLE);
    
    // Calculate number of pixels to fill
    uint32_t pixel_count = (x1 - x0 + 1) * (y1 - y0 + 1);
    uint32_t bytes_to_send = pixel_count * 2;  // 2 bytes per pixel
    
    // Send data in chunks if needed
    uint32_t chunk_size = 120;  // SSP can send 120 bytes at once efficiently
    uint8_t* bytes = (uint8_t*)color_buffer;
    
    for (uint32_t i = 0; i < bytes_to_send; i += chunk_size) {
        uint32_t current_chunk = (bytes_to_send - i < chunk_size) ? (bytes_to_send - i) : chunk_size;
        
        if (current_chunk == 120) {
            ssp_send_120Bytes(&bytes[i]);
        } else {
            ssp_send_bytes(&bytes[i], current_chunk);
        }
        ssp_wait_send_end();
    }
    
    cs_control(SSP_CS_DISABLE);
}

void display_fill_screen(uint16_t color) {
    // Create a buffer for one row of pixels
    uint16_t buffer[_width]; // Max width is 240

    // Fill the buffer with the specified color
    for (int i = 0; i < _width; i++) {
        buffer[i] = color;
    }
    
    // Set address window to entire screen (including offsets)
    display_set_addr_window(0, 0, _width - 1, _height - 1);
    
    // Fill screen row by row
    set_dc_pin(1);  // Data mode
    cs_control(SSP_CS_ENABLE);
    
    // Calculate total number of pixels
    uint32_t total_pixels = _width * _height;
    uint32_t pixels_sent = 0;
    
    // Send pixels in chunks
    while (pixels_sent < total_pixels) {
        uint32_t line_pixels = (_width < (total_pixels - pixels_sent)) ? _width : (total_pixels - pixels_sent);
        uint32_t pixels_remaining = line_pixels;
        uint32_t buffer_offset = 0;
        
        while (pixels_remaining > 0) {
            // Determine chunk size - SSP can efficiently send 120 bytes at once
            // 120 bytes = 60 pixels (since each pixel is 2 bytes)
            uint32_t pixels_to_send = (pixels_remaining > 60) ? 60 : pixels_remaining;
            uint32_t bytes_to_send = pixels_to_send * 2;
            
            if (bytes_to_send == 120) {
                ssp_send_120Bytes((const uint8_t*)&buffer[buffer_offset]);
            } else {
                ssp_send_bytes((const uint8_t*)&buffer[buffer_offset], bytes_to_send);
            }
            ssp_wait_send_end();
            
            buffer_offset += pixels_to_send;
            pixels_remaining -= pixels_to_send;
        }
        
        pixels_sent += line_pixels;
    }
    
    cs_control(SSP_CS_DISABLE);
}

void display_set_rotation(uint8_t m) {
    uint8_t madctl = 0;
    
    _rotation = m & 3;  // can't be higher than 3

    switch (_rotation) {
    case 0:
        madctl = ST77XX_MADCTL_MX | ST77XX_MADCTL_MY | ST77XX_MADCTL_BGR;
        _xstart = _colstart;
        _ystart = _rowstart;
        break;
    case 1:
        madctl = ST77XX_MADCTL_MY | ST77XX_MADCTL_MV | ST77XX_MADCTL_BGR;
        _xstart = _rowstart;
        _ystart = _colstart2;
        break;
    case 2:
        madctl = ST77XX_MADCTL_RGB;
        _xstart = _colstart2;
        _ystart = _rowstart2;
        break;
    case 3:
        madctl = ST77XX_MADCTL_MX | ST77XX_MADCTL_MV | ST77XX_MADCTL_BGR;
        _xstart = _rowstart2;
        _ystart = _colstart;
        break;
    }
    
    write_command(ST77XX_MADCTL);
    write_data(madctl);
}

uint16_t display_get_color(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

void display_draw_pixel(uint16_t x, uint16_t y, uint16_t color) {
    // Check if coordinates are in bounds
    if (x >= _width || y >= _height) {
        return;
    }
    
    // Set address window to single pixel
    display_set_addr_window(x, y, x, y);
    
    // Send color
    set_dc_pin(1);  // Data mode
    cs_control(SSP_CS_ENABLE);
    write_data16(color);
    cs_control(SSP_CS_DISABLE);
}

void backlight_turn_off()
{
    pmu_set_led2_value(0);
}

void backlight_turn_on()
{
    pmu_set_led2_value(1);
}