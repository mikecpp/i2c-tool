#include "Console.h"
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

using namespace std;
static char _buffer[1024];

int Console::read(uint8_t* data, int length) 
{
    return fread(data, 1, length, stdin);
}

void Console::write(const uint8_t* data, int length) 
{
    fwrite(data, 1, length, stdout);
    fflush(stdout);
}

string Console::readLine() 
{
    string line;
    size_t bytesRead;

    while (true) {
        bytesRead = fread(_buffer, 1, sizeof(_buffer), stdin);
        write(reinterpret_cast<const uint8_t*>(_buffer), bytesRead); // Echo input
        if (bytesRead > 0) {
            for (size_t i = 0; i < bytesRead; ++i) {
                if (_buffer[i] == '\r' || _buffer[i] == '\n') {
                    return line;
                }
                line += _buffer[i];
            }
        } 
        else {
            vTaskDelay(pdMS_TO_TICKS(1));
        }
    }
}

void Console::printf(const char* fmt, ...) 
{
    va_list args;
    
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    fflush(stdout);
}
