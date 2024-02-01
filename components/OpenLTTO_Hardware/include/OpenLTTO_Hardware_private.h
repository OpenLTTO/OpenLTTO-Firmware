#ifndef OPENLTTO_HARDWARE_PRIVATE_H_
#define OPENLTTO_HARDWARE_PRIVATE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "axp2101.h"

#include "OpenLTTO_Hardware.h"

#include "sdkconfig.h"

#ifdef __cplusplus
}
#endif

void func(void);

#endif
