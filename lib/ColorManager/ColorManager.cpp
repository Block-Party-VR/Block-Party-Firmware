#include "ColorManager.h"

void ColorManager::Update(){
    if(!(this->enabled)){
        return;
    }
    // go through our colors and have them fade from r->g->b->r
    for(uint8_t i = 0; i < 9; i++){
        for(uint8_t j = 0; j < 3; j++){
            Color * color = this->colors[i][j];
            // fade from red to green
            if(color->red > 0 && color->green >= 0 && color->blue == 0){
                color->red--;
                color->green++;
            }
            // fade from green to blue
            else if(color->green > 0 && color->blue >= 0 && color->red == 0){
                color->green--;
                color->blue++;
            }
            // fade from blue to red
            else if(color->blue > 0 && color->red >= 0 && color->green == 0){
                color->blue--;
                color->red++;
            }
        }
    }

    // set the colors
    for(uint8_t i = 0; i < 9; i++){
        Color temp_colors[3] = {*(this->colors[i][0]), *(this->colors[i][1]), *(this->colors[i][2])};
        this->board->SetStackColors(i, temp_colors);
    }
}

void ColorManager::Enable(bool enable){
    this->enabled = enable;

    if(this->enabled == false){
        // set all the colors to black
        Color black(0, 0, 0);
        Color temp_colors[3] = {black, black, black};
        // set the colors
        for(uint8_t i = 0; i < 9; i++){
            this->board->SetStackColors(i, temp_colors);
        }
    }
}