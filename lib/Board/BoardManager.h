/**
 * @file BoardManager.h
 * @brief Provides an easy way to manage a board
 */
#pragma once

#include "Board.h"
#include "BoardDriver.h"
#include "Vector3D.h"
#include "Animator.h"

template <const V3D<uint32_t> &BOARD_DIMS>
class BoardManager{
    public:
    BoardManager(BoardDriver<BOARD_WIDTH*BOARD_LENGTH> &boardDriver, Animator<BOARD_DIMS> &animator);

    ~BoardManager() = default;

    /**
     * @brief Initialize the board control. This hsould be called during setup
     */
    void Init();

    /**
     * @brief Read the board state and update the LED colors
     */
    void Update();

    /**
     * @brief Set the color of one cube
     * @param position the position of the cube
     * @param color the oclor you want the cube to be
     */
    void SetCubeColor(const V3D<uint32_t> &position, const V3D<uint32_t> &color);

    /**
     * @brief Set the color of one column of cubes. 
     * A column is defined as x,y,z where z is the normal vector direction to a plane. 
     * x,y is the column coordinate on the plane.
     * @param column the column vector
     * @param color the color you want the column to be
     */
    void SetColumnColors(const V3D<uint32_t> &column, const V3D<uint32_t> *color, uint32_t numColors);


    /**
     * @brief Fill a column along any axis with a color
     * @param column .z specifies the normal direction of the plane (see PLANE_NORMAL), and 
     * the x,y values specify the location of the column in that plane 
     * to fill. IE To fill one stack at 0,2 I would say give V3D<uint32_t>(0,2,PLANE_NORMAL::Z)
     * @param color the color you want to fill the column with
     */
    void FillColumnColor(const V3D<uint32_t> &column, const V3D<uint32_t> &color);

    /**
     * @returns true if the board has changed state
     */
    bool HasBoardChanged();

    /**
     * @brief Clear the board state changed flag
     */
    void ClearBoardChanged();

    /**
     * @brief Get the board occupation state returned in the format a,b,c,d....
     */
    void Board2StackString(String& messageBuffer);

    void FillColor(const V3D<uint32_t> &color){this->board.FillColor(color);}
    void PrintColorState(){this->board.PrintEntireBoard();}

    private:
    BoardDriver<BOARD_WIDTH*BOARD_LENGTH> &driver;
    Board<BOARD_DIMS> board{};
    Animator<BOARD_DIMS> &animator;

    void updateStackColors(const V3D<uint32_t> &column);

    uint32_t getColumnHeight(BOARD_TYPES::PLANE_NORMAL normal){
        switch(normal){
        case BOARD_TYPES::PLANE_NORMAL::X:
        return BOARD_DIMS.x;
        break;
        case BOARD_TYPES::PLANE_NORMAL::Y:
        return BOARD_DIMS.y;
        break;
        case BOARD_TYPES::PLANE_NORMAL::Z:
        return BOARD_DIMS.z;
        break;
        default:
        return 0;
    }
    }

    void updateColorsFromAnimator();

};
template <const V3D<uint32_t> &BOARD_DIMS>
void BoardManager<BOARD_DIMS>::updateColorsFromAnimator(){
    if(this->animator.interpolatedFrameHasChanged){
        this->board.UpdateAllColors(this->animator.GetInterpolatedFrame());
        this->animator.interpolatedFrameHasChanged = false;
    }
}

template <const V3D<uint32_t> &BOARD_DIMS>
BoardManager<BOARD_DIMS>::BoardManager(BoardDriver<BOARD_WIDTH*BOARD_LENGTH> &boardDriver, Animator<BOARD_DIMS> &animator):
    driver(boardDriver),
    animator(animator){}

template <const V3D<uint32_t> &BOARD_DIMS>
void BoardManager<BOARD_DIMS>::Init(){
    this->driver.Init();
}

template <const V3D<uint32_t> &BOARD_DIMS>
void BoardManager<BOARD_DIMS>::Update(){
    this->updateColorsFromAnimator();
    // update the occupied cubes on the board and the cube colors
    for(uint32_t x = 0; x <  BOARD_DIMS.x; x++){
        for(uint32_t y = 0; y < BOARD_DIMS.y; y++){
            uint32_t stackIndex{y * BOARD_DIMS.x + x};
            uint32_t numCubes{this->driver.GetNumberCubes(stackIndex)};
            for(uint32_t z = 0; z < BOARD_DIMS.z; z++){
                V3D<uint32_t> cubePosition{x, y, z};
                // update the cube's occupation
                this->board.SetCubeOccupation(cubePosition, z < numCubes);
            }

            // create the column vector for the slice direction
            V3D<uint32_t> sliceVector{x,y,BOARD_TYPES::PLANE_NORMAL::Z};
            // create a cube slice array buffer
            BOARD_TYPES::Cube* sliceBuffer[BOARD_DIMS.z];
            // have the board slice get read into our buffer
            uint32_t sliceSize = this->board.SliceBoard(sliceVector, sliceBuffer);
            if(sliceSize < BOARD_DIMS.z){
                return;
            }
            // send the board slice to the driver to update its LED colors
            this->driver.UpdateStackLEDs(stackIndex, sliceBuffer, BOARD_DIMS.z);
        }
    }
}

template <const V3D<uint32_t> &BOARD_DIMS>
void BoardManager<BOARD_DIMS>::updateStackColors(const V3D<uint32_t> &column){
    // the only column type allowed here is z.
    V3D<uint32_t> sliceVector{column.x, column.y, BOARD_TYPES::Z};
    // create a buffer for slice board to write the cube slice into
    BOARD_TYPES::Cube * cubeSlice[BOARD_DIMS.z];
    uint32_t sliceSize = this->board.SliceBoard(column, cubeSlice);

    uint32_t numCubes{this->getColumnHeight(static_cast<BOARD_TYPES::PLANE_NORMAL>(column.z))};

    if(sliceSize < numCubes){
        return;
    }

    this->driver.UpdateStackLEDs(BOARD_DIMS.x, cubeSlice, numCubes);
}

template <const V3D<uint32_t> &BOARD_DIMS>
void BoardManager<BOARD_DIMS>::SetCubeColor(const V3D<uint32_t> &position, const V3D<uint32_t> &color){
    this->board.SetCubeColor(position, color);
    V3D<uint32_t> slice{position.x, position.y, BOARD_TYPES::PLANE_NORMAL::Z};
    this->updateStackColors(slice);
}

template <const V3D<uint32_t> &BOARD_DIMS>
void BoardManager<BOARD_DIMS>::SetColumnColors(const V3D<uint32_t> &column, const V3D<uint32_t> *color, uint32_t numColors){
    uint32_t columnHeight{this->getColumnHeight(static_cast<BOARD_TYPES::PLANE_NORMAL>(column.z))};

    // create a cube pointer buffer and store a board slice into it
    BOARD_TYPES::Cube * slicedBoard[columnHeight];
    uint32_t sliceLength{this->board.SliceBoard(column, slicedBoard)};

    uint32_t maxIndex{std::min(numColors, columnHeight)};

    if(sliceLength < maxIndex){
        return;
    }
    
    for(uint32_t i = 0; i < maxIndex; i++){
        slicedBoard[i]->color = color[i];
    }
}

template <const V3D<uint32_t> &BOARD_DIMS>
void BoardManager<BOARD_DIMS>::FillColumnColor(const V3D<uint32_t> &column, const V3D<uint32_t> &color){
    uint32_t columnHeight{this->getColumnHeight(column.z)};

    V3D<uint32_t> colors[columnHeight];
    for(uint32_t i = 0; i < columnHeight; i++){
        colors[i] = color;
    }

    this->SetColumnColors(column, colors);
}

template <const V3D<uint32_t> &BOARD_DIMS>
bool BoardManager<BOARD_DIMS>::HasBoardChanged(){return this->board.BoardStateChanged();}

template <const V3D<uint32_t> &BOARD_DIMS>
void BoardManager<BOARD_DIMS>::ClearBoardChanged(){this->board.SetStateChanged(false);}

template <const V3D<uint32_t> &BOARD_DIMS>
void BoardManager<BOARD_DIMS>::Board2StackString(String& messageBuffer){
    this->board.ToStackString(messageBuffer);
}
