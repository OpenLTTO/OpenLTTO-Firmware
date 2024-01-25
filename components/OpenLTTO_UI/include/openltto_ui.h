#ifndef LIB_OPENLTTO_UI_H
#define LIB_OPENLTTO_UI_H

#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "esp_event.h"

#include "lvgl.h"
#include "esp_lvgl_port.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "driver/spi_master.h"
#include "esp_lcd_touch_ft5x06.h"
#include "sdkconfig.h"

#include "ui.h"

#define OPENLTTO_UI_LCD_H_RES              (CONFIG_OPENLTTO_BSP_LCD_H_RES)
#define OPENLTTO_UI_LCD_V_RES              (CONFIG_OPENLTTO_BSP_LCD_V_RES)
#define OPENLTTO_UI_LCD_PIXEL_CLOCK_HZ     (40 * 1000 * 1000)
#define OPENLTTO_UI_LCD_SPI_NUM            (CONFIG_OPENLTTO_BSP_LCD_SPI_NUM)

#define OPENLTTO_UI_LCD_DRAW_BUFF_SIZE     (OPENLTTO_UI_LCD_H_RES * OPENLTTO_UI_LCD_V_RES * (0.2))
#define OPENLTTO_UI_LCD_DRAW_BUFF_DOUBLE   (1)

#define OPENLTTO_UI_LCD_DATA0         (CONFIG_OPENLTTO_BSP_LCD_DAT0)
#define OPENLTTO_UI_LCD_PCLK          (CONFIG_OPENLTTO_BSP_LCD_PCLK)
#define OPENLTTO_UI_LCD_CS            (CONFIG_OPENLTTO_BSP_LCD_CS)
#define OPENLTTO_UI_LCD_DC            (CONFIG_OPENLTTO_BSP_LCD_DC)
#define OPENLTTO_UI_LCD_RST           (CONFIG_OPENLTTO_BSP_LCD_RST)
#define OPENLTTO_UI_LCD_BACKLIGHT     (CONFIG_OPENLTTO_BSP_LCD_BACKLIGHT)
#define OPENLTTO_UI_LCD_TOUCH_INT     (CONFIG_OPENLTTO_BSP_LCD_TOUCH_INT)

// Bit number used to represent command and parameter
#define LCD_CMD_BITS           8
#define LCD_PARAM_BITS         8
#define LCD_LEDC_CH            CONFIG_OPENLTTO_UI_DISPLAY_BRIGHTNESS_LEDC_CH

#define ESP_LCD_COLOR_FORMAT_RGB565    (1)
#define ESP_LCD_COLOR_FORMAT_RGB888    (2)

/* LCD display color format */
#define OPENLTTO_UI_LCD_COLOR_FORMAT        (ESP_LCD_COLOR_FORMAT_RGB565)
/* LCD display color bytes endianess */
#define OPENLTTO_UI_LCD_BIGENDIAN           (1)
/* LCD display color bits */
#define OPENLTTO_UI_LCD_BITS_PER_PIXEL      (16)
/* LCD display color space */
#define OPENLTTO_UI_LCD_COLOR_SPACE         (ESP_LCD_COLOR_SPACE_BGR)

#define OPENLTTO_UI_I2C_NUM     CONFIG_OPENLTTO_BSP_I2C_NUM

ESP_EVENT_DECLARE_BASE(OPENLTTO_UI_EVENT_BASE);

//Define all your UI callback events here
typedef enum {
    OPENLTTO_UI_EVENT_RESET_CONFIRMED,
} openlttoUIEvent_t;

typedef struct {
    int max_transfer_sz;    /*!< Maximum transfer size, in bytes. */
} openltto_ui_display_config_t;

// Device Health Functions
void openltto_ui_driver_init();
void openltto_ui_init();
void openltto_ui_init_event_loop();
void openltto_ui_event_handler(void* handler_arg, esp_event_base_t base, int32_t id, void * event_data);
void openltto_ui_event_emitter(openlttoUIEvent_t event);
void openltto_ui_lvgl_port_init();
void openltto_ui_lvgl_indev_init();
lv_disp_t *openltto_ui_lcd_init();
esp_err_t openltto_ui_touch_new(esp_lcd_touch_handle_t *ret_touch);
esp_err_t openltto_ui_display_new(const openltto_ui_display_config_t *config, esp_lcd_panel_handle_t *ret_panel, esp_lcd_panel_io_handle_t *ret_io);

#endif
