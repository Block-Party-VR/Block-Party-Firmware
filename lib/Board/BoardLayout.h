/**
 * @brief This is the full board manager which handles the state of every stack on the board
*/

#pragma once

#include "CubeStack.h"
#include "Color.h"

class BoardLayout{
    public:
    /**
     * @brief BoardLayout COnstructor
    */
    BoardLayout(uint8_t boardWidth, uint8_t boardLength, uint8_t boardHeight, CubeStack ** stacks) :
        boardWidth(boardWidth),
        boardLength(boardLength),
        boardHeight(boardHeight),
        stacks(stacks)
    {
      this->lastBoardState = new uint16_t[boardWidth * boardLength];
    }

    /**
     * @brief Check if our board state has changed
     * @return true if the board state has changed, false otherwise
    */
    bool BoardStateHasChanged();

    /**
     * @brief Get the Number of Stacks
     * @return the number of stacks
    */
    uint8_t GetNumberStacks();

    /**
     * @brief Set the LED Colors
     * @param stackNum the stack index you would like to address.
     * From top left to bottom right, the stack numbers are as follows:
     * | 0 1 2 |
     * | 3 4 5 |
     * | 6 7 8 |
     * @param Colors the array of colors to set the LEDs in a stack to
    */
    void SetStackColors(uint8_t stackNum, Color * colors);

    /**
     * @brief Get the board population state
     * @param boardStateBuffer the buffer to write the board state to. It must be at least boardWidth * boardLength in length
     */
    void GetBoardState(uint16_t * boardStateBuffer);

    private:
    uint8_t boardWidth;
    uint8_t boardLength;
    uint8_t boardHeight;
    /*
    An array of arrays of stacks
    [ [stack1, stack2, stack3],
      [stack4, stack5, stack6],
      [stack7, stack8, stack9] ]
    etc
    */
    CubeStack ** stacks;

    // records the last known board state
    uint16_t * lastBoardState;

};