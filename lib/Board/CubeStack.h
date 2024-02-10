/**
 * @brief this manages a single cube stack and the lighting / detecting of how many cubes
*/

#pragma once

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "Color.h"


class CubeStack{
    public:
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
    uint8_t GetNumberCubes();

    /**
     * @brief Set the led color array and then send the data to the LED strip
     * @param colors the array of colors to set the LEDs to
     * @param numColors the number of colors in the array
    */
   void SetLEDColors(Color * colors, uint8_t numColors);

    /**
     * @brief sends the LED data to the LED strip
    */
    void SendLEDData();

   private:
    

    uint8_t ADCPin;
    // we will probably need a pointer to a fastled object here
    Adafruit_NeoPixel blockLights;

    uint16_t lowPassADCRead{0};

    // store the Color of each LED
    Color * ledColors;
    uint8_t numLEDs;
    uint8_t lastStackHeight{0};
};