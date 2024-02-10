/**
 * @file ColorManager.h
 * @brief Generate pretty colors for the board and make it do something when unity isn't controlling it
*/

#pragma once

#include "BoardLayout.h"
#include "Color.h"

class ColorManager{
    public:
    ColorManager(BoardLayout * board) :
        board(board)
    {}

    /**
     * @brief Allows the color manager to update the board colors
    */
    void Update();

    /**
     * @brief Enables or disables the color manager
     * @param enable true to enable, false to disable
    */
    void Enable(bool enable);


    private:
    BoardLayout * board;
    bool enabled{true};

    Color * colors[9][3] = {
        {new Color(255, 0, 0), new Color(0, 255, 0), new Color(0, 0, 255)},
        {new Color(255, 0, 0), new Color(0, 255, 0), new Color(0, 0, 255)},
        {new Color(255, 0, 0), new Color(0, 255, 0), new Color(0, 0, 255)},
        {new Color(255, 0, 0), new Color(0, 255, 0), new Color(0, 0, 255)},
        {new Color(255, 0, 0), new Color(0, 255, 0), new Color(0, 0, 255)},
        {new Color(255, 0, 0), new Color(0, 255, 0), new Color(0, 0, 255)},
        {new Color(255, 0, 0), new Color(0, 255, 0), new Color(0, 0, 255)},
        {new Color(255, 0, 0), new Color(0, 255, 0), new Color(0, 0, 255)},
        {new Color(255, 0, 0), new Color(0, 255, 0), new Color(0, 0, 255)}
    };
};