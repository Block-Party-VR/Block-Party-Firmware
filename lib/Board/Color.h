/**
 * @file Color.h
 * @brief This file contains the color struct
*/

#pragma once
#include <Arduino.h>

// store a color
struct Color{
    public:
    // create a constructor for this struct
    Color(uint8_t red, uint8_t green, uint8_t blue) :
        red(red),
        green(green),
        blue(blue)
    {}
    Color() = default;
    uint8_t red{0};
    uint8_t green{0};
    uint8_t blue{0};
};