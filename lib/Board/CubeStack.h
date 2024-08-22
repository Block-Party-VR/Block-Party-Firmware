/**
 * @brief this manages a single cube stack and the lighting / detecting of how many cubes
*/

#pragma once

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "Color.h"
#include "Cube.h"


namespace CubeStack{
    /**
     * @brief Construct a new Cube Stack object
     * @param ADCPin the pin that the ADC is connected to
     * @param ledPin the pin that the LED is connected to
    */
    CubeStack(uint16_t ADCPin, uint16_t ledPin, uint8_t numLEDs);

    /**
     * @brief Returns the number of cubes in the stack
     * @return the number of cubes in the stack
    */
    uint32_t GetNumberCubes(Cube &cube);

    /**
     * @brief sends the LED data to the LED strip
    */
    void SendLEDData();
};