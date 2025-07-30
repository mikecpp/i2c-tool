set(COMMONLIB_PATH ../../modules/commonlib)
set(ESP32LIB_PATH ../../modules/esp32lib)

idf_component_register(
    SRCS 
        main.cpp
        I2CMaster.cpp
        Console.cpp
        CmdManager.cpp
    INCLUDE_DIRS
)
