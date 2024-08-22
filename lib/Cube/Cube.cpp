#include "Cube.h"

Cube::Cube(uint8_t ADCPin, uint8_t ledPin) 
    : ADCPin(ADCPin),
      ledPin(ledPin)
{
    Color black(0,0,0);
    this->SetColor(black);
}

Cube::Cube(uint8_t ADCPin, uint8_t ledPin, const Color &color)
    : ADCPin(ADCPin),
      ledPin(ledPin)
{
    this->SetColor(color);
}

void Cube::SetColor(const Color &color){
    this->color.red = color.red;
    this->color.green = color.green;
    this->color.blue = color.blue;
}