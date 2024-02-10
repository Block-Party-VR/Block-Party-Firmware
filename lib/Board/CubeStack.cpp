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

uint8_t CubeStack::GetNumberCubes(){
    // read the ADC and return the number of cubes
    /*
    0 cubes: 1 : 4095-3071
    1 cube: 1/2 3070-1706
    2 cubes: 1/3 1705-1195
    3 cubes: 1/4 1195-0
    */
    uint16_t value = analogRead(this->ADCPin);

    this->lowPassADCRead = static_cast<uint16_t>((static_cast<float>(this->lowPassADCRead) * 0.9) + (static_cast<float>(value) * 0.1));

    // temporary definitions to define value ranges:
    uint16_t zeroCubesHigh = 4095;
    uint16_t zeroCubesLow = 3071;
    uint16_t oneCubeLow = 1706;
    uint16_t twoCubesLow = 1000;
    uint16_t threeCubesLow = 0;

    uint8_t stackHeight = 0;

    if(this->lowPassADCRead >= zeroCubesLow && this->lowPassADCRead <= zeroCubesHigh){
        stackHeight = 0;
    }
    else if(this->lowPassADCRead >= oneCubeLow){
        stackHeight = 1;
    }
    else if(this->lowPassADCRead >= twoCubesLow){
        stackHeight = 2;
    }
    else if(this->lowPassADCRead >= threeCubesLow){
        stackHeight = 3;
    }
    if(this->lastStackHeight != stackHeight){
        this->lastStackHeight = stackHeight;
        this->SendLEDData();
    }
    
    return stackHeight;
}

void CubeStack::SetLEDColors(Color * colors, uint8_t numColors){
    // copy the colors into the ledColors array
    for(int i = 0; i < numColors; i++){
        this->ledColors[i].red = colors[i].red;
        this->ledColors[i].green = colors[i].green;
        this->ledColors[i].blue = colors[i].blue;
    }

    this->SendLEDData();
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