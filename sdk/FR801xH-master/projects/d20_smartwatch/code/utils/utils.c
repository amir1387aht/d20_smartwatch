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