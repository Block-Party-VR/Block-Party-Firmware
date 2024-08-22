/**
 * @file Cube.h
 * @brief An object to store the data of one cube
 */

#pragma once

#include "Color.h"

class Cube{
    public:
    Cube();
    Cube();

    void SetColor(const Color &color);

    uint16_t lastADCReading{0};

    Color color;
    bool isOccupied{false};
};