#include "openltto_ui.h"

static const char *TAG = "openltto-ui";
static lv_disp_t *disp;
static esp_lcd_touch_handle_t tp;   // LCD touch handle
static esp_event_loop_handle_t loop_handle;
static bool driver_initialized = false;

ESP_EVENT_DEFINE_BASE(OPENLTTO_UI_EVENT_BASE);

void openltto_ui_driver_init()
{
    ESP_LOGI(TAG,"Initializing OpenLTTO UI Driver...");

    //Initialize UI Lib and Drivers...
    openltto_ui_lvgl_port_init();
    disp = openltto_ui_lcd_init();
    openltto_ui_lvgl_indev_init();

    driver_initialized = true;
}


void openltto_ui_init()
{
    if(!driver_initialized){
        openltto_ui_driver_init();
    }

    ESP_LOGI(TAG,"Initializing OpenLTTO UI...");

    //Initialize the UI event loop
    openltto_ui_init_event_loop();

    //Launch the UI Home Screen
    ESP_LOGI(TAG,"Launching OpenLTTO UI Home Screen...");
    ui_init();
}

void openltto_ui_init_event_loop()
{
    esp_event_loop_args_t loop_args = {
        .queue_size = CONFIG_OPENLTTO_UI_EVENT_QUEUE_DEPTH,
        .task_name = "OpenLTTO_UI_Events",
        .task_priority = CONFIG_OPENLTTO_UI_EVENT_TASK_PRIORITY,
        .task_stack_size = CONFIG_OPENLTTO_UI_EVENT_TASK_HEAP * 1024,
        .task_core_id = 1,
    };

    esp_err_t rv = ESP_OK;

    rv = esp_event_loop_create(&loop_args, &loop_handle);
    if(rv!=ESP_OK){
        ESP_LOGE(TAG, "Failed to create UI event loop! [%s]",esp_err_to_name(rv));
    }

    rv = esp_event_handler_register_with(loop_handle, OPENLTTO_UI_EVENT_BASE, ESP_EVENT_ANY_ID, openltto_ui_event_handler, NULL);
    if(rv!=ESP_OK){
        ESP_LOGE(TAG, "Failed to register UI event handler! [%s]",esp_err_to_name(rv));
    }

}

void openltto_ui_event_handler(void* handler_arg, esp_event_base_t base, int32_t id, void * event_data)
{
    switch(id){
        case OPENLTTO_UI_EVENT_RESET_CONFIRMED:
            ESP_LOGI(TAG,"UI Reset Requested!");
            vTaskDelay(3000/portTICK_PERIOD_MS);    //Delay for user readability
            openltto_wifi_reset_config();
            break;
    }
}

void openltto_ui_event_emitter(openlttoUIEvent_t event)
{
    esp_event_post_to(loop_handle, OPENLTTO_UI_EVENT_BASE, event, NULL, NULL, pdMS_TO_TICKS(CONFIG_OPENLTTO_UI_EVENT_EMITTER_BLOCK_DURATION));
}

void openltto_ui_lvgl_port_init()
{
    ESP_LOGI(TAG, "Initializing LVGL library...");
    const lvgl_port_cfg_t lvgl_cfg = {
        .task_priority = 4,
        .task_stack = 4096,
        .task_affinity = 1,
        .task_max_sleep_ms = 500,
        .timer_period_ms = 5,
    };
    esp_err_t rv = lvgl_port_init(&lvgl_cfg);
    if(rv!=ESP_OK){
        ESP_LOGE(TAG, "Failed to initialize LVGL! [%s]",esp_err_to_name(rv));
        return; //TODO: Better handling of this error case
    }
}

void openltto_ui_lvgl_indev_init()
{
    ESP_LOGI(TAG, "Initializing LVGL Input Driver");

    esp_err_t rv = openltto_ui_touch_new(&tp);
    if(rv!=ESP_OK){
        ESP_LOGE(TAG, "Failed to initialize LVGL Input Driver! [%s]",esp_err_to_name(rv));
        return; //TODO: Better handling of this error case
    }

    assert(tp);

    /* Add touch input (for selected screen) */
    const lvgl_port_touch_cfg_t touch_cfg = {
        .disp = disp,
        .handle = tp,
    };

    lvgl_port_add_touch(&touch_cfg);
}

esp_err_t openltto_ui_touch_new(esp_lcd_touch_handle_t *ret_touch)
{
    /* Initialize touch */
    const esp_lcd_touch_config_t tp_cfg = {
        .x_max = OPENLTTO_UI_LCD_H_RES,
        .y_max = OPENLTTO_UI_LCD_V_RES,
        .rst_gpio_num = GPIO_NUM_NC, // Shared with LCD reset
        .int_gpio_num = GPIO_NUM_NC,
        .levels = {
            .reset = 0,
            .interrupt = 0,
        },
        .flags = {
            .swap_xy = 0,
            .mirror_x = 0,
            .mirror_y = 0,
        },
    };
    esp_lcd_panel_io_handle_t tp_io_handle = NULL;
    const esp_lcd_panel_io_i2c_config_t tp_io_config = ESP_LCD_TOUCH_IO_I2C_FT5x06_CONFIG();
    esp_err_t rv = esp_lcd_new_panel_io_i2c((esp_lcd_i2c_bus_handle_t)OPENLTTO_UI_I2C_NUM, &tp_io_config, &tp_io_handle);
    if(rv!=ESP_OK){
        ESP_LOGE(TAG, "Failed to initialize ESP_LCD Touch Driver! [%s]",esp_err_to_name(rv));
        return(rv); //TODO: Better handling of this error case
    }
    vTaskDelay(1000);
    return esp_lcd_touch_new_i2c_ft5x06(tp_io_handle, &tp_cfg, ret_touch);
}

esp_err_t openltto_ui_display_new(const openltto_ui_display_config_t *config, esp_lcd_panel_handle_t *ret_panel, esp_lcd_panel_io_handle_t *ret_io)
{
    esp_err_t ret = ESP_OK;
    assert(config != NULL && config->max_transfer_sz > 0);

    ESP_LOGD(TAG, "Initializing SPI bus");
    const spi_bus_config_t buscfg = {
        .sclk_io_num = OPENLTTO_UI_LCD_PCLK,
        .mosi_io_num = OPENLTTO_UI_LCD_DATA0,
        .miso_io_num = GPIO_NUM_NC,
        .quadwp_io_num = GPIO_NUM_NC,
        .quadhd_io_num = GPIO_NUM_NC,
        .max_transfer_sz = config->max_transfer_sz,
    };
    ret = spi_bus_initialize(OPENLTTO_UI_LCD_SPI_NUM, &buscfg, SPI_DMA_CH_AUTO);
    if(ret!=ESP_OK){
        ESP_LOGE(TAG, "Failed to Initialize SPI bus! [%s]",esp_err_to_name(ret));
        return(ret);
    }

    ESP_LOGD(TAG, "Install Panel IO");
    const esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = OPENLTTO_UI_LCD_DC,
        .cs_gpio_num = OPENLTTO_UI_LCD_CS,
        .pclk_hz = OPENLTTO_UI_LCD_PIXEL_CLOCK_HZ,
        .lcd_cmd_bits = LCD_CMD_BITS,
        .lcd_param_bits = LCD_PARAM_BITS,
        .spi_mode = 0,
        .trans_queue_depth = 10,
    };
    ret = esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)OPENLTTO_UI_LCD_SPI_NUM, &io_config, ret_io);
    if(ret!=ESP_OK){
        ESP_LOGE(TAG, "Failed to Initialize Panel IO! [%s]",esp_err_to_name(ret));
        return(ret);
    }

    ESP_LOGD(TAG, "Installing LCD driver");
    const esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = OPENLTTO_UI_LCD_RST, // Shared with Touch reset
        .color_space = OPENLTTO_UI_LCD_COLOR_SPACE,
        .bits_per_pixel = OPENLTTO_UI_LCD_BITS_PER_PIXEL,
    };
    ret = esp_lcd_new_panel_st7789(*ret_io, &panel_config, ret_panel);
    if(ret!=ESP_OK){
        ESP_LOGE(TAG, "Failed to Install LCD Driver! [%s]",esp_err_to_name(ret));
        return(ret);
    }

    esp_lcd_panel_reset(*ret_panel);
    esp_lcd_panel_init(*ret_panel);
    esp_lcd_panel_mirror(*ret_panel, false, false);
    esp_lcd_panel_invert_color(*ret_panel, true);
    return ESP_OK;
}

lv_disp_t *openltto_ui_lcd_init()
{
    ESP_LOGI(TAG, "Initializing LCD Display");

    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_handle_t panel_handle = NULL;
    const openltto_ui_display_config_t disp_cfg = {
        .max_transfer_sz = OPENLTTO_UI_LCD_DRAW_BUFF_SIZE * sizeof(uint16_t),
    };
    esp_err_t ret = openltto_ui_display_new(&disp_cfg, &panel_handle, &io_handle);
    if(ret!=ESP_OK){
        ESP_LOGE(TAG, "Failed to Initialize LCD Display! [%s]",esp_err_to_name(ret));
        return(NULL);
    }

    esp_lcd_panel_disp_on_off(panel_handle, true);

    /* Add LCD screen */
    ESP_LOGD(TAG, "Add LCD screen");
    const lvgl_port_display_cfg_t lvgl_disp_cfg = {
        .io_handle = io_handle,
        .panel_handle = panel_handle,
        .buffer_size = OPENLTTO_UI_LCD_DRAW_BUFF_SIZE,
        .double_buffer = OPENLTTO_UI_LCD_DRAW_BUFF_DOUBLE,
        .hres = OPENLTTO_UI_LCD_H_RES,
        .vres = OPENLTTO_UI_LCD_V_RES,
        .monochrome = false,
        /* Rotation values must be same as used in esp_lcd for initial settings of the screen */
        .rotation = {
            .swap_xy = false,
            .mirror_x = true,
            .mirror_y = true,
        },
        .flags = {
            .buff_dma = true,
        }
    };

    return lvgl_port_add_disp(&lvgl_disp_cfg);
}
