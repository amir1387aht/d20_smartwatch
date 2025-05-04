#ifndef __APP_H__
#define __APP_H__

#include "config.h"
#include "utils/utils.h"
#include "sys_utils.h"
#include "display/display.h"
#include "display/gfx.h"
#include "fonts/FreeMono9pt7b.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef __cplusplus
extern "C"
{
#endif
    void app_init();
    void app_update();
#ifdef __cplusplus
}
#endif

#endif