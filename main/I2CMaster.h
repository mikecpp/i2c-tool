#pragma once

#include <cstdint>
#include "esp_err.h"
#include "driver/i2c.h"

#define I2C_MASTER_FREQ_HZ          1000000
#define I2C_MASTER_TX_BUF_DISABLE   0
#define I2C_MASTER_RX_BUF_DISABLE   0
#define I2C_MASTER_TIMEOUT_MS       1000

class I2CMaster {
public:
    I2CMaster(i2c_port_t i2c_port, gpio_num_t sda_pin, gpio_num_t scl_pin, uint32_t clk_speed_hz = I2C_MASTER_FREQ_HZ);
    ~I2CMaster();
    
    int init();
    bool detect(uint8_t slave_addr);
    int write(uint8_t slave_addr, const uint8_t* data, size_t len);         
    int read(uint8_t slave_addr, uint8_t* data, size_t len);               

private:
    i2c_port_t i2c_port_;
    gpio_num_t sda_pin_;
    gpio_num_t scl_pin_;
    uint32_t clk_speed_hz_;
};
