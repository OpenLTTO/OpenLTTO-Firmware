#include "OpenLTTO_Hardware_private.h"

static const char *TAG = "flow-bsp";

static esp_err_t bus_i2c_init();
static esp_err_t bus_i2c_read(uint8_t addr, const uint8_t out_data, size_t out_size, uint8_t *in_data, size_t in_size);
static esp_err_t bus_i2c_write(uint8_t addr, const uint8_t out_reg, size_t out_reg_size, const uint8_t out_data, size_t out_size);
static void pmu_init(void);
static void axp2101_init(void);
static void aw9523_init(void);

void openltto_hardware_init(void)
{
    bus_i2c_init();
    pmu_init();
}

static void pmu_init(void) {
    ESP_LOGI(TAG, "Initializing Power Management...");
    axp2101_init();
    aw9523_init();
    vTaskDelay(100);
}

static esp_err_t bus_i2c_init()
{
    ESP_LOGI(TAG, "Initializing I2C Bus...");

    int i2c_master_port = CONFIG_OPENLTTO_HW_I2C_NUM;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = CONFIG_OPENLTTO_HW_I2C_SDA_IO,
        .scl_io_num = CONFIG_OPENLTTO_HW_I2C_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_DISABLE,
        .scl_pullup_en = GPIO_PULLUP_DISABLE,
        //.master.clk_speed = CONFIG_OPENLTTO_HW_I2C_CLK_SPEED_HZ,
    };

    i2c_param_config((i2c_port_t)i2c_master_port, &conf);

    return i2c_driver_install((i2c_port_t)i2c_master_port, conf.mode, 0, 0, 0);
}

static esp_err_t bus_i2c_read(uint8_t addr, const uint8_t out_data,
                              size_t out_size, uint8_t *in_data,
                              size_t in_size) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    if (out_data && out_size) {
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, addr << 1, true);
        i2c_master_write(cmd, &out_data, out_size, true);
    }
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (addr << 1) | 1, true);
    i2c_master_read(cmd, in_data, in_size, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);

    esp_err_t res =
        i2c_master_cmd_begin((i2c_port_t)CONFIG_OPENLTTO_HW_I2C_NUM, cmd, pdMS_TO_TICKS(1000));
    if (res != ESP_OK)
        ESP_LOGE(TAG, "Could not read from device [0x%02x at %d]: %d", addr,
                 CONFIG_OPENLTTO_HW_I2C_NUM, res);

    i2c_cmd_link_delete(cmd);
    return res;
}

static esp_err_t bus_i2c_write(uint8_t addr, const uint8_t out_reg,
                               size_t out_reg_size, const uint8_t out_data,
                               size_t out_size) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, addr << 1, true);
    if (out_reg && out_reg_size)
        i2c_master_write(cmd, &out_reg, out_reg_size, true);
    i2c_master_write(cmd, &out_data, out_size, true);
    i2c_master_stop(cmd);
    esp_err_t res =
        i2c_master_cmd_begin((i2c_port_t)CONFIG_OPENLTTO_HW_I2C_NUM, cmd, pdMS_TO_TICKS(1000));
    if (res != ESP_OK)
        ESP_LOGE(TAG, "Could not write to device [0x%02x at %d]: %d", addr, 0,
                 res);
    i2c_cmd_link_delete(cmd);

    return res;
}

static void axp2101_write_register_byte(uint8_t reg, const uint8_t data_byte)
{
    bus_i2c_write(0x34, reg, 1, data_byte, 1);
}

static void axp2101_read_register_byte(uint8_t reg, const uint8_t *data_ptr)
{
    bus_i2c_read(0x34, reg, 1, (uint8_t *)data_ptr, 1);
}

static void axp2101_init(void) {
    ESP_LOGI(TAG, "Initializing AXP2101 Power Management Chip...");
    //Original AXP2101 init code:
    uint8_t ldo_onoff_cfg = 0;
    axp2101_read_register_byte(AXP2101_REG_LDOS_ON_OFF_CTL,&ldo_onoff_cfg);
    axp2101_write_register_byte(AXP2101_REG_LDOS_ON_OFF_CTL,ldo_onoff_cfg|0x80);
    axp2101_write_register_byte(AXP2101_REG_LDOS_V_SET + 7,0b11110 - 5);
}

static void aw9523_init(void) {
    ESP_LOGI(TAG, "Initializing AW9523 Driver IC...");
    // aw9523 reset
    bus_i2c_write(0x58, 0x7F, 1, 0x00, 1);
    vTaskDelay(30);
    // aw9523 default setting
    // Port Direction Config
    bus_i2c_write(0x58, 0x04, 1, 0b11011000, 1);
    bus_i2c_write(0x58, 0x05, 1, 0b01111100, 1);
    // Port Mode Config
    bus_i2c_write(0x58, 0x12, 1, 0b11111111, 1);
    bus_i2c_write(0x58, 0x13, 1, 0b11111111, 1);
    // Global Control
    bus_i2c_write(0x58, 0x11, 1, (1 << 4), 1);
    //Setup port output states
    bus_i2c_write(0x58, 0x02, 1, 0b00000111, 1);
    bus_i2c_write(0x58, 0x03, 1, 0b10000011, 1);
}
