#pragma once

#include <stdint.h>
#include <cstdio>
#include <string>

using namespace std;

class Console {
public:
    Console() = default;
    int read(uint8_t* data, int length);
    void write(const uint8_t* data, int length);
    string readLine();
    void printf(const char* fmt, ...);
};
