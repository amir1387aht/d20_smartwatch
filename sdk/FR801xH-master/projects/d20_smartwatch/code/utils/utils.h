#ifndef UTILS_H
#define UTILS_H

#include "driver_gpio.h"
#include "driver_pmu.h"
#include "sys_utils.h"
#include "os_task.h"
#include "config.h"

void device_init(void);
void device_vibrate(int count);
void device_led_blank(int count);
bool read_button_state();
void delay_ms(uint32_t ms);

#endif // UTILS_H