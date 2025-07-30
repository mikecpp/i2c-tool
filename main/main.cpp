#include <stdio.h>
#include <string>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <chrono>
#include "esp_task_wdt.h"
#include "driver/uart.h"
#include "CmdManager.h"
#include "Console.h"
#include "I2CMaster.h"

#define SW_VERSION "1.1.0"

#define SDA_PIN GPIO_NUM_7
#define SCL_PIN GPIO_NUM_6

#define BUFFER_SIZE         1024
#define UART_BAUD_RATE      1500000

static I2CMaster i2c(I2C_NUM_0, SDA_PIN, SCL_PIN);

static Console _console;

static uint8_t hexToByte(const char* s) 
{
    return (uint8_t) strtol(s, nullptr, 16);
}

static int i2c_scanCmd(int argc, char* argv[]) 
{
    std::string found;

    for (uint8_t addr = 0x03; addr <= 0x77; ++addr) {
        if (i2c.detect(addr)) {
            char buf[8];
            sprintf(buf, "%02X:", addr); 
            found += buf;
        }
    }
    
    if (!found.empty()) {
        if (found.back() == ':') found.pop_back();
        _console.printf("OK %s\n", found.c_str());
    } else {
        _console.printf("NG\n");
        _console.printf("syntax: i2c.scan\n");
    }

    return 0;
}

static int i2c_detectCmd(int argc, char* argv[]) 
{
    if (argc < 2) {
        _console.printf("NG\n");
        _console.printf("syntax: i2c.detect [addr]\n");
        return -1;
    }

    uint8_t addr = hexToByte(argv[1]);

    if (i2c.detect(addr))
        _console.printf("OK\n");
    else {
        _console.printf("NG\n");
        _console.printf("syntax: i2c.detect [addr]\n");
    }

    return 0;
}

static int i2c_readCmd(int argc, char* argv[]) 
{
    if (argc < 3) {
        _console.printf("NG\n");
        _console.printf("syntax: i2c.read [addr] [len]\n");
        return -1;
    }

    uint8_t addr = hexToByte(argv[1]);
    uint8_t len = hexToByte(argv[2]);
    uint8_t rdata[BUFFER_SIZE] = {0};

    if (i2c.read(addr, rdata, len) == ESP_OK) 
    {
        _console.printf("OK ");
        for (int i = 0; i < len; ++i) {
            _console.printf("%02X", rdata[i]);
            if (i < len - 1) _console.printf(":");
        }
        _console.printf("\n");
        return 0;
    }
    _console.printf("NG\n");
    _console.printf("syntax: read [addr] [len]\n");

    return -1;
}

static int i2c_writeCmd(int argc, char* argv[]) 
{
    if (argc < 4) {
        _console.printf("NG\n");
        _console.printf("syntax: write [addr] [len] [data]\n");
        return -1;
    }

    uint8_t addr = hexToByte(argv[1]);
    uint8_t len = hexToByte(argv[2]);
    uint8_t wdata[BUFFER_SIZE] = {0};
    int idx = 0;

    char buf[BUFFER_SIZE];
    strncpy(buf, argv[3], sizeof(buf));
    buf[sizeof(buf)-1] = 0;
    char* token = strtok(buf, ":"); 

    while (token && idx < len) {
        wdata[idx++] = hexToByte(token);
        token = strtok(nullptr, ":");
    }

    if (i2c.write(addr, wdata, len) == ESP_OK) {
        _console.printf("OK\n");
        return 0;
    }

    _console.printf("NG\n");
    _console.printf("syntax: write [addr] [len] [data]\n");

    return -1;
}

static int set_watchdog()
{
    esp_task_wdt_deinit();

    esp_task_wdt_config_t twdt_config = {
        .timeout_ms     = 1000, 
        .idle_core_mask = 0,
        .trigger_panic  = true
    };

    if (esp_task_wdt_init(&twdt_config) != ESP_OK) 
        return -1;

    return 0;
}

extern "C" void app_main(void)
{
    uart_set_baudrate(UART_NUM_0, UART_BAUD_RATE);

    if (set_watchdog() != 0) {
        printf("Failed to set watchdog timer!\n");
        return;
    }

    if (i2c.init() != ESP_OK) {
        printf("I2C init failed!\n");
        return;
    }

    CmdManager manager;
    manager.registerCommand("i2c.scan",   i2c_scanCmd,   "Scan I2C bus");
    manager.registerCommand("i2c.detect", i2c_detectCmd, "Detect I2C device at address");
    manager.registerCommand("i2c.read",   i2c_readCmd,   "Read from I2C device");
    manager.registerCommand("i2c.write",  i2c_writeCmd,  "Write to I2C device");

    printf("I2C Tool v%s\n", SW_VERSION);

    while (true) {
        _console.printf("> ");
        std::string input = _console.readLine();
        manager.execute(input);
    }
}
