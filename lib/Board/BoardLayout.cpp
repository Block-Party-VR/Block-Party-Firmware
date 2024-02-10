#include "BoardLayout.h"

uint8_t BoardLayout::GetNumberStacks(){
    return this->boardWidth * this->boardHeight;
}

void BoardLayout::SetStackColors(uint8_t stackNum, Color * colors){
    CubeStack * stack = this->stacks[stackNum];
    stack->SetLEDColors(colors, this->boardHeight);
}

bool BoardLayout::BoardStateHasChanged(){
    uint16_t boardState[this->boardWidth * this->boardLength];
    this->GetBoardState(boardState);

    // compare the board state to the last board state
    for(int i = 0; i < (this->boardWidth * this->boardLength); i++){
        uint16_t stackState = boardState[i];
        uint16_t lastStackState = (this->lastBoardState)[i];
        if(stackState != lastStackState){
            // copy the board state into the last board state
            for(int k = 0; k < (this->boardWidth * this->boardLength); k++){
                this->lastBoardState[k] = boardState[k];
            }
            return true;
        }
    }
    return false;
}

void BoardLayout::GetBoardState(uint16_t * boardStateBuffer){
    for(int i = 0; i < (this->boardLength * this->boardWidth); i++){
        CubeStack * stack = this->stacks[i];
        stack->SendLEDData(); // Enable this if you want to constantly stream LED data
        boardStateBuffer[i] = stack->GetNumberCubes();
    }
}

