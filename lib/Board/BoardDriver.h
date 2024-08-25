/**
 * @file BoardDriver.h
 * @brief the board driver provides helpful methods to read the physical board's state, 
 * filter those results, and send color data to the board
 */
#pragma once

#include <cstdint>
#include <array>

#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

#include "BoardTypes.h"
#include "BoardTypes.h"

template <uint32_t NUM_STACKS>
class BoardDriver{
public:

    /**
     * @param stacks a reference to an array which contains information about the pins which control each cube stack.
     * @param pixelController a reference to a pre-constructed neopixel controller which will allow this driver to control neopixels
     */
    BoardDriver(
        std::array<BOARD_TYPES::CubeStack, NUM_STACKS> &stacks, 
        Adafruit_NeoPixel &pixelController
        );
    ~BoardDriver() = default;

    /**
     * @brief Initializes all of the pins necessary to read/write to the board
     */
    void Init();

    /**
     * @param numXStacks the width of the board along the x dimension.
     * @param X_COORD the x coordinate of the stack you want information about
     * @param Y_COORD the y coordinate of the stack you want information about
     * @returns Get the number of cubes present in a stack
     */
    uint32_t GetNumberCubes(uint32_t numXStacks, uint32_t X_COORD, uint32_t Y_COORD);

    /**
     * @param stackIndex the index of the stack that you want to know the height of
     * @returns Get the number of cubes present in a stack
     */
    uint32_t GetNumberCubes(uint32_t stackIndex);

    /**
     * @brief stream data to a single cube stack on the board
     * @param numXStacks the width of the board along the x dimension.
     * @param X_COORD the x coordinate of the stack you want information about
     * @param Y_COORD the y coordinate of the stack you want information about
     * @param cubes an array of cube pointers which contain the color data you want to stream
     * @param numCubes the number of cubes in the cubes array
     */
    void UpdateStackLEDs(
        uint32_t numXStacks, 
        uint32_t X_COORD, 
        uint32_t Y_COORD, 
        BOARD_TYPES::Cube* cubes[], 
        uint32_t numCubes
        );

    /**
     * @brief stream data to a single cube stack on the board
     * @param stackIndex the index of the stack you want to update the color of
     * @param cubes an array of cube pointers which contain the color data you want to stream
     * @param numCubes the number of cubes in the cubes array
     */
    void UpdateStackLEDs(
        uint32_t stackIndex, 
        BOARD_TYPES::Cube* cubes[], 
        uint32_t numCubes
        );


private:
    std::array<BOARD_TYPES::CubeStack, NUM_STACKS> &stacks;
    Adafruit_NeoPixel &pixelController;
    std::array<uint16_t, NUM_STACKS> filteredReadings;

    uint32_t xy2StackIndex(uint32_t x_coord, uint32_t y_coord, uint32_t numXStacks){
        return x_coord + y_coord*numXStacks;
    }
};

template<uint32_t NUM_STACKS>
void BoardDriver<NUM_STACKS>::Init(){
    for(uint32_t i = 0; i < NUM_STACKS; i++){
        pinMode(this->stacks[i].ledPin, OUTPUT);
        filteredReadings[i] = 0;
    }

    // begin doesn't really do anything besides setting the pinmode
    this->pixelController.begin();
}

template<uint32_t NUM_STACKS>
BoardDriver<NUM_STACKS>::BoardDriver(
    std::array<BOARD_TYPES::CubeStack, NUM_STACKS> &stacks, 
    Adafruit_NeoPixel &pixelController
    ): 
stacks(stacks),
pixelController(pixelController)
{
    for(uint32_t i = 0; i < NUM_STACKS; i++){
        this->filteredReadings[i] = 0;
    }
}

template<uint32_t NUM_STACKS>
void BoardDriver<NUM_STACKS>::UpdateStackLEDs(
    uint32_t numXStacks, 
    uint32_t X_COORD, 
    uint32_t Y_COORD, 
    BOARD_TYPES::Cube* cubes[], 
    uint32_t numCubes
    ){
    this->UpdateStackLEDs(this->xy2StackIndex(X_COORD, Y_COORD, numXStacks), cubes, numCubes);
}

template<uint32_t NUM_STACKS>
void BoardDriver<NUM_STACKS>::UpdateStackLEDs(
    uint32_t stackIndex, 
    BOARD_TYPES::Cube* cubes[], 
    uint32_t numCubes
    ){
    this->pixelController.setPin(this->stacks[stackIndex].ledPin);
    for(int i = 0; i < numCubes; i++){
        V3D<uint32_t> color{cubes[i]->color};
        this->pixelController.setPixelColor(i*2, this->pixelController.Color(color.x, color.y, color.z));
        this->pixelController.setPixelColor((i*2 + 1), this->pixelController.Color(color.x, color.y, color.z));
    }
    this->pixelController.show();
}

template<uint32_t NUM_STACKS>
uint32_t BoardDriver<NUM_STACKS>::GetNumberCubes(
    uint32_t numXStacks, 
    uint32_t X_COORD, 
    uint32_t Y_COORD
    ){
    return this->GetNumberCubes(this->xy2StackIndex(X_COORD, Y_COORD, numXStacks));
}

template<uint32_t NUM_STACKS>
uint32_t BoardDriver<NUM_STACKS>::GetNumberCubes(uint32_t stackIndex){
    // read the ADC and return the number of cubes
    /*
    0 cubes: 1 : 4095-3400
    1 cube: 1/2 3400-2500
    2 cubes: 1/3 2500-1850
    3 cubes: 1/4 1850-0
    */
    uint16_t value = analogRead(this->stacks[stackIndex].adcPin);
    uint16_t lowPassADCRead = 
        static_cast<uint16_t>(
            (static_cast<float>(this->filteredReadings[stackIndex]) * 0.9)
             + (static_cast<float>(value) * 0.1)
            );
    // temporary definitions to define value ranges:
    uint16_t zeroCubesHigh = 4095;
    uint16_t zeroCubesLow = 3400;
    uint16_t oneCubeLow = 2500;
    uint16_t twoCubesLow = 1850;
    uint16_t threeCubesLow = 0;

    uint8_t stackHeight = 0;

    if(lowPassADCRead >= zeroCubesLow && lowPassADCRead <= zeroCubesHigh){
        stackHeight = 0;
    }
    else if(lowPassADCRead >= oneCubeLow){
        stackHeight = 1;
    }
    else if(lowPassADCRead >= twoCubesLow){
        stackHeight = 2;
    }
    else if(lowPassADCRead >= threeCubesLow){
        stackHeight = 3;
    }
    
    this->filteredReadings[stackIndex] = lowPassADCRead;
    return stackHeight;
}