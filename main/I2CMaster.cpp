#include "I2CMaster.h"
#include "esp_log.h"
#include "esp_timer.h"

I2CMaster::I2CMaster(i2c_port_t i2c_port, gpio_num_t sda_pin, gpio_num_t scl_pin, uint32_t clk_speed_hz)
    : i2c_port_(i2c_port), sda_pin_(sda_pin), scl_pin_(scl_pin), clk_speed_hz_(clk_speed_hz) 
{
    
}

I2CMaster::~I2CMaster() 
{
    i2c_driver_delete(i2c_port_);
}

int I2CMaster::init() 
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = sda_pin_,
        .scl_io_num = scl_pin_,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master = {
            .clk_speed = clk_speed_hz_,
        },
        .clk_flags = 0,
    };

    esp_err_t ret = i2c_param_config(i2c_port_, &conf);
    if (ret != ESP_OK) {
        return ret;
    }

    ret = i2c_driver_install(i2c_port_, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
    if (ret != ESP_OK) {
        return ret;
    }
    return ESP_OK;
}

bool I2CMaster::detect(uint8_t slave_addr) 
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (slave_addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_port_, cmd, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    return ret == ESP_OK;
}

int I2CMaster::write(uint8_t slave_addr, const uint8_t* data, size_t len) 
{ 
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (slave_addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write(cmd, const_cast<uint8_t*>(data), len, true);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_port_, cmd, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    # if 0
    printf("i2c_write: addr=0x%02X, len=%zu, data=", slave_addr, len);
    for (size_t i = 0; i < len; ++i) {
        printf("%02X ", data[i]);
    }
    printf("\n");
    #endif 

    return ret;
}

int I2CMaster::read(uint8_t slave_addr, uint8_t* data, size_t len) 
{ 
    if (len == 0) {
        return ESP_OK;
    }
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (slave_addr << 1) | I2C_MASTER_READ, true);
    if (len > 1) {
        i2c_master_read(cmd, data, len - 1, I2C_MASTER_ACK);
    }
    i2c_master_read_byte(cmd, data + len - 1, I2C_MASTER_NACK);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_port_, cmd, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    # if 0
    printf("i2c_read: addr=0x%02X, len=%zu, data=", slave_addr, len);
    for (size_t i = 0; i < len; ++i) {
        printf("%02X ", data[i]);
    }
    printf("\n");
    #endif 

    return ret;
}
