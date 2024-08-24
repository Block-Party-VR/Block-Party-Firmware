/**
 * @file BoardManager.h
 * @brief Provides an easy way to manage a board
 */
#pragma once

#include "Board.h"
#include "BoardDriver.h"
#include "Vector3D.h"

template <const V3D &BOARD_DIMS>
class BoardManager{
    public:
    BoardManager(BoardDriver<BOARD_WIDTH*BOARD_LENGTH> &boardDriver);

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
    void SetCubeColor(const V3D &position, const V3D &color);

    /**
     * @brief Set the color of one column of cubes. 
     * A column is defined as x,y,z where z is the normal vector direction to a plane. 
     * x,y is the column coordinate on the plane.
     * @param column the column vector
     * @param color the color you want the column to be
     */
    void SetColumnColors(const V3D &column, const V3D *color);


    /**
     * @brief Fill a column along any axis with a color
     * @param column .z specifies the normal direction of the plane (see PLANE_NORMAL), and 
     * the x,y values specify the location of the column in that plane 
     * to fill. IE To fill one stack at 0,2 I would say give V3D(0,2,PLANE_NORMAL::Z)
     * @param color the color you want to fill the column with
     */
    void FillColumnColor(const V3D &column, const V3D &color);

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
    String &Board2StackString();

    private:
    BoardDriver<BOARD_WIDTH*BOARD_LENGTH> &driver;
    Board<BOARD_DIMS> board{};

    bool hasBoardChanged{false};

    void updateBoardColors(const V3D &column);

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

};

template <const V3D &BOARD_DIMS>
BoardManager<BOARD_DIMS>::BoardManager(BoardDriver<BOARD_WIDTH*BOARD_LENGTH> &boardDriver):
    driver(boardDriver){}

template <const V3D &BOARD_DIMS>
void BoardManager<BOARD_DIMS>::Init(){
    this->driver.Init();
}

template <const V3D &BOARD_DIMS>
void BoardManager<BOARD_DIMS>::Update(){
    // update the occupied cubes on the board and the cube colors
    for(uint32_t x = 0; x <  BOARD_DIMS.x; x++){
        for(uint32_t y = 0; y < BOARD_DIMS.y; y++){
            uint32_t stackIndex{y * BOARD_DIMS.x + x};
            uint32_t numCubes{this->driver.GetNumberCubes(stackIndex)};
            for(uint32_t z = 0; z < BOARD_DIMS.z; z++){
                V3D cubePosition{x, y, z};
                this->board.SetCubeOccupation(cubePosition, z < numCubes);
                cubePosition.z = BOARD_TYPES::PLANE_NORMAL::Z;
                this->driver.UpdateStackLEDs(stackIndex, this->board.SliceBoard(cubePosition), BOARD_DIMS.z);
            }
        }
    }
}

template <const V3D &BOARD_DIMS>
void BoardManager<BOARD_DIMS>::updateBoardColors(const V3D &column){
    BOARD_TYPES::Cube ** cubeSlice{this->board.SliceBoard(column)};
    uint32_t numCubes{this->getColumnHeight(static_cast<BOARD_TYPES::PLANE_NORMAL>(column.z))};
    this->driver.UpdateStackLEDs(BOARD_DIMS.x, cubeSlice, numCubes);
}

template <const V3D &BOARD_DIMS>
void BoardManager<BOARD_DIMS>::SetCubeColor(const V3D &position, const V3D &color){
    this->board.SetCubeColor(position, color);
    V3D slice{position.x, position.y, BOARD_TYPES::PLANE_NORMAL::Z};
    this->updateBoardColors(slice);
}

template <const V3D &BOARD_DIMS>
void BoardManager<BOARD_DIMS>::SetColumnColors(const V3D &column, const V3D *color){
    uint32_t columnHeight{this->getColumnHeight(static_cast<BOARD_TYPES::PLANE_NORMAL>(column.z))};

    V3D position = column;
    for(uint32_t z = 0; z < columnHeight; z++){
        position.z = z;
        this->board.SetCubeColor(position, color[z]);

    }

    this->updateBoardColors(column);
}

template <const V3D &BOARD_DIMS>
void BoardManager<BOARD_DIMS>::FillColumnColor(const V3D &column, const V3D &color){
    uint32_t columnHeight{this->getColumnHeight(column.z)};

    V3D position = column;
    for(uint32_t z = 0; z < columnHeight; z++){
        position.z = z;
        this->board.SetCubeColor(position, color);
    }

    this->updateBoardColors(column);
}

template <const V3D &BOARD_DIMS>
bool BoardManager<BOARD_DIMS>::HasBoardChanged(){return this->hasBoardChanged;}

template <const V3D &BOARD_DIMS>
void BoardManager<BOARD_DIMS>::ClearBoardChanged(){this->hasBoardChanged = false;}

template <const V3D &BOARD_DIMS>
String &BoardManager<BOARD_DIMS>::Board2StackString(){
    String message{};
    this->board.ToStackString(message);
    return message;
}
