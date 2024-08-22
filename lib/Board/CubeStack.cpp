#include "CubeStack.h"

CubeStack::CubeStack(uint16_t ADCPin, uint16_t ledPin, uint8_t numLEDs){
    this->ADCPin = ADCPin;
    this->blockLights = *(new Adafruit_NeoPixel(numLEDs*2, ledPin, NEO_GRB + NEO_KHZ800));
    this->ledColors = new Color[numLEDs];
    this->numLEDs = numLEDs;

    // initialize the LED colors to off
    for(int i = 0; i < numLEDs; i++){
        this->ledColors[i] = *(new Color(0, 0, 0));
    }
};

uint32_t CubeStack::GetNumberCubes(Cube &cube){
    // read the ADC and return the number of cubes
    /*
    0 cubes: 1 : 4095-3400
    1 cube: 1/2 3400-2500
    2 cubes: 1/3 2500-1850
    3 cubes: 1/4 1850-0
    */
    uint16_t value = analogRead(cube.ADCPin);
    uint16_t lowPassADCRead = static_cast<uint16_t>((static_cast<float>(cube.lastADCReading) * 0.9) + (static_cast<float>(value) * 0.1));

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
    
    return stackHeight;
}

void CubeStack::SendLEDData(){
    // we always initialize before we do anything because other CubeStacks could be hogging the hardware
    // between our writes
    this->blockLights.begin();
    // set the LED colors
    for(int i = 0; i < this->numLEDs; i++){
        this->blockLights.setPixelColor(i*2, this->blockLights.Color(this->ledColors[i].red, this->ledColors[i].green, this->ledColors[i].blue));
        this->blockLights.setPixelColor((i*2 + 1), this->blockLights.Color(this->ledColors[i].red, this->ledColors[i].green, this->ledColors[i].blue));
    }
    this->blockLights.show();
}