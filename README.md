# I2C-Tool

## Introduction
  - This tool is based on ESP32S3 EVB. https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32s3/esp32-s3-devkitc-1/index.html 
  - Console baudrate: 1.5M. 

## How to install and build? 
  - Install ESP IDF V5.4.2 https://docs.espressif.com/projects/esp-idf/en/v5.4.2/esp32s3/get-started/index.html 
```
    > idf.py -p /dev/ttyACM0 -b 1500000 flash monitor 
```
## How to test?
```
  > python ssd1306.py 
```

## I2C Command 
### 1. i2c.scan  
```
    syntax: 
        i2c.scan  

    return:
        success: OK [return hex string]
        fail: NG
```
### 2. i2c.detect  
```
    syntax: 
        i2c.detect [addr]  

    parameters:  
        - addr: hex string. (like 3C) 

    return: 
        success: OK 
        fail: NG 
```   
### 3. i2c.read 
```
    syntax: 
        i2c.read [addr] [len]

    parameters:
        - addr: hex string. 
        - len: hex string. (like AA)

    return:
        success: OK [return hex string]
        fail: NG
```
### 4. i2c.write 
```
    syntax: 
        i2c.write [addr] [len] [data] 

    parameters:
        - addr: hex string. 
        - len: hex string. (like AA)
        - data: hex string with comma. (like AA:BB:CC:DD) 
       
    return:
        success: OK 
        fail: NG
```
