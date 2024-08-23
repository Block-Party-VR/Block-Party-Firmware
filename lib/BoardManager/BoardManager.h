#pragma once

#include "Board.h"
#include "BoardDriver.h"
#include "Vector3D.h"

template <V3D &BOARD_DIMS>
class BoardManager{
    public:
    BoardManager(BoardDriver<BOARD_WIDTH*BOARD_LENGTH> &boardDriver):
    driver(boardDriver){};

    ~BoardManager() = default;

    void Init();

    void Update();

    void SetCubeColor(const V3D &position, const V3D &color);

    void SetColumnColor(const V3D &position, const V3D &color);


    /**
     * @brief Fill a column along any axis with a color
     * @param column .z specifies the normal direction of the plane (see PLANE_NORMAL), and 
     * the x,y values specify the location of the column in that plane 
     * to fill. IE To fill one stack at 0,2 I would say give V3D(0,2,PLANE_NORMAL::Z)
     * @param color the color you want to fill the column with
     */
    void FillColumnColor(const V3D &column, const V3D &color);

    bool HasBoardChanged(){return this->hasBoardChanged;}

    private:
    BoardDriver<BOARD_WIDTH*BOARD_LENGTH> &driver;
    Board<BOARD_DIMS> board{};

    bool hasBoardChanged{false};

    void updateBoardColors(const V3D &column);

};

template <V3D &BOARD_DIMS>
void BoardManager<BOARD_DIMS>::Init(){
    this->driver.Init();
}

template <V3D &BOARD_DIMS>
void BoardManager<BOARD_DIMS>::Update(){
    // update the occupied cubes on the board and the cube colors
    for(uint32_t x = 0; x <  BOARD_DIMS.x; x++){
        for(uint32_t y = 0; y < BOARD_DIMS.y; y++){
            uint32_t numCubes{this->driver.GetNumberCubes(i)};
            for(uint32_t z = 0; z < BOARD_DIMS.z; z++){
                V3D cubePosition{x, y, z};
                bool isOccupied{z < numCubes};
                if(this->board.SetCubeOccupation(cubePosition, i < numCubes) != isOccupied){
                    this->board.boardStateHasChanged = true;
                }
                cubePosition.z = Board::PLANE_NORMAL::Z;
                this->driver.UpdateStackLEDs(i, this->board.SliceBoard(cubePosition), BOARD_DIMS.z);
            }
        }
    }

    // update the colors
    for(uint32_t i = 0; i < BOARD_DIMS.x * BOARD_DIMS.y; i++){
        
    }

}

template <V3D &BOARD_DIMS>
void BoardManager<BOARD_DIMS>::updateBoardColors(const V3D &column){
    Cube * cubeSlice{this->board.SliceBoard(column)};
    uint32_t numCubes{0};

    switch(column.z){
        case Board::PLANE_NORMAL::X:
        numCubes = BOARD_DIMS.x;
        break;
        case Board::PLANE_NORMAL::Y:
        numCubes = BOARD_DIMS.y;
        break;
        case Board::PLANE_NORMAL::Z:
        numCubes = BOARD_DIMS.z;
        break;
        default:
        break;
    }

    this->driver.UpdateStackLEDs(BOARD_DIMS.x, cubeSlice, numCubes);
}

template <V3D &BOARD_DIMS>
void BoardManager<BOARD_DIMS>::SetCubeColor(const V3D &position, const V3D &color){
    this->board.SetCubeColor(position, color);
    V3D slice{position.x, position.y, Board::PLANE_NORMAL::Z};
    this->updateBoardColors(slice);
}