#include "utils/utils.h"

void device_init(void)
{
    co_printf("Device Init\r\n");

    // Configure vibrator
    system_set_port_mux(VIBRATOR_GPIO_PIN_NAME, VIBRATOR_GPIO_PIN_NUM, VIBRATOR_GPIO_PIN_FUNC);
    gpio_set_dir(VIBRATOR_GPIO_PIN_NAME, VIBRATOR_GPIO_PIN_NUM, GPIO_DIR_OUT);

    // Configure button
    system_set_port_mux(BUTTON_GPIO_PIN_NAME, BUTTON_GPIO_PIN_NUM, BUTTON_GPIO_PIN_FUNC);
    gpio_set_dir(BUTTON_GPIO_PIN_NAME, BUTTON_GPIO_PIN_NUM, GPIO_DIR_IN);
}

void device_vibrate(int count)
{
    for(int i = 0; i < count; i++)
    {
        if(i != 0) delay_ms(300);

        gpio_set_pin_value(VIBRATOR_GPIO_PIN_NAME, VIBRATOR_GPIO_PIN_NUM, 1);
        delay_ms(300);
        gpio_set_pin_value(VIBRATOR_GPIO_PIN_NAME, VIBRATOR_GPIO_PIN_NUM, 0);
    }
}

void device_led_blank(int count)
{
    for(int i = 0; i < count; i++)
    {
        if(i != 0) delay_ms(300);

        pmu_set_led1_value(1);
        delay_ms(300);
        pmu_set_led1_value(0);
    }
}

bool read_button_state() {
    return gpio_get_pin_value(BUTTON_GPIO_PIN_NAME, BUTTON_GPIO_PIN_NUM);
}

void delay_ms(uint32_t ms)
{
    co_delay_100us(ms * 10);
}

void int_to_str(int val, char *buf) {
    int i = 0, j;
    bool neg = false;

    if (val == 0) {
        buf[i++] = '0';
    } else {
        if (val < 0) {
            neg = true;
            val = -val;
        }

        while (val > 0) {
            buf[i++] = (val % 10) + '0';
            val /= 10;
        }

        if (neg) {
            buf[i++] = '-';
        }
    }

    // Reverse the string
    buf[i] = '\0';
    for (j = 0; j < i / 2; j++) {
        char tmp = buf[j];
        buf[j] = buf[i - 1 - j];
        buf[i - 1 - j] = tmp;
    }
}