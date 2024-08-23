#pragma once

#include <cstdint>
#include <array>
#include <WString.h>

#include "Cube.h"
#include "Vector3D.h"

template <const V3D &BOARD_DIMS>
class Board{
    public:
    Board() = default;
    ~Board() = default;

    constexpr const V3D &GetSize() const{return BOARD_DIMS;}
    constexpr uint32_t GetNumberCubes() const{return BOARD_DIMS.x * BOARD_DIMS.y * BOARD_DIMS.z;}

    /**
     * @brief Returns a string in the format:
     *  !a,b,c,d,e,f,g,h,i;
     * Where each letter is the number of cubes in a given position's stack
     * @param stringBuffer the buffer to write the string into
     */
    void ToStackString(String& stringBuffer) const;

    /**
     * @returns Returns an array which contains how many cubes are in each z column on the board
     */
    std::array<uint32_t, BOARD_DIMS.x * BOARD_DIMS.y> &LinearizeBoard() const;

    /**
     * @brief fill the entire board with the given color
     * @param color the color to fill the board with
     */
    void FillColor(const V3D &color);

    /**
     * @brief Set the color of the cube at the given position. 
     * Even if the cube is not occupied, it will remember this color
     *  and turn that color if it becomes occupied.
     * @param position the position of the cube.
     * @param color the color you want the cube to be
     */
    void SetCubeColor(const V3D &position, const V3D &color);

    /**
     * @brief Set the occupation status of the cube at a given position
     * @param position the position of the cube
     * @param the occupation status of the cube
     * @post if the new occupation status of the cube is different than 
     * the old occupation status, this will enable boardStateHasChanged.
     */
    void SetCubeOccupation(const V3D &position, bool occupation);

    /**
     * @returns true if the board state has changed since this flag was last set to false
     */
    bool BoardStateChanged(){return this->boardStateHasChanged;}

    /**
     * @brief set the board state status.
     * @note you should probably only use this to clear the baordState flag
     */
    void SetStateChanged(bool boardState){this->boardStateHasChanged = boardState;}

    /**
     * @brief Get a column along any axis
     * @param column .z specifies the normal direction of the plane (see PLANE_NORMAL), and 
     * the x,y values specify the location of the column in that plane 
     * to fill. IE To fill one stack at 0,2 I would say give V3D(0,2,PLANE_NORMAL::Z)
     * @returns an array of cubes along that column
     */
    Cube ** SliceBoard(const V3D &column);

    private:
    // this is a 3d array of cubes to represent the board. Good luck visualizing it
    /*  _____________
       /            /|
      /            / |
     /            /  |
    /____________/   |
    |            |   | 
    |            |  /
    |            | /
    |____________|/
     */
    std::array<std::array<std::array<Cube, BOARD_DIMS.z>, BOARD_DIMS.y>, BOARD_DIMS.x> cubes;

    bool boardStateHasChanged;
};

template <const V3D &BOARD_DIMS>
void Board<BOARD_DIMS>::ToStackString(String &stringBuffer) const{
    std::array<uint32_t, BOARD_DIMS.x*BOARD_DIMS.y> linearizedBoard = this->LinearizeBoard();

    stringBuffer += String(linearizedBoard[0]);

    for(uint32_t i = 0; i < BOARD_DIMS.x * BOARD_DIMS.y; i++){
        stringBuffer += "," + String(linearizedBoard[i]);
    }
}

template <const V3D &BOARD_DIMS>
std::array<uint32_t, BOARD_DIMS.x * BOARD_DIMS.y> & Board<BOARD_DIMS>::LinearizeBoard() const{
    // convert the board into one array where each entry represents the height of one stack
    std::array<uint32_t, BOARD_DIMS.x * BOARD_DIMS.y> linearizedBoard;
    for(uint32_t x{0}; x < BOARD_DIMS.x; x++){
        for(uint32_t y{0}; y < BOARD_DIMS.y; y++){
            for(uint32_t z{0}; z < BOARD_DIMS.z; z++){
                bool isOccupied{this->cubes[x][y][z].isOccupied};
                linearizedBoard[x + y*3] += static_cast<uint32_t>(isOccupied);
            }
        }
    }
    return linearizedBoard;
}

template <const V3D &BOARD_DIMS>
void Board<BOARD_DIMS>::FillColor(const V3D &color){
    for(uint32_t x{0}; x < BOARD_DIMS.x; x++){
        for(uint32_t y{0}; y < BOARD_DIMS.y; y++){
            for(uint32_t z{0}; z < BOARD_DIMS.z; z++){
                this->cubes[x][y][z].color = color;
            }
        }
    }
}

template <const V3D &BOARD_DIMS>
void Board<BOARD_DIMS>::SetCubeColor(const V3D &position, const V3D &color){
    this->cubes[position.x][position.y][position.z].color = color;
}

template <const V3D &BOARD_DIMS>
void Board<BOARD_DIMS>::SetCubeOccupation(const V3D &position, bool occupation){
    bool oldOccupation{this->cubes[position.x][position.y][position.z].isOccupied};
    this->cubes[position.x][position.y][position.z].isOccupied = occupation;
    if(occupation != oldOccupation) this->boardStateHasChanged = true;
}

template <const V3D &BOARD_DIMS>
Cube ** Board<BOARD_DIMS>::SliceBoard(const V3D &column){
    uint32_t columnLength{0};
    V3D indexIncriment{};
    V3D position{};
    switch(column.z){
        case BOARD_TYPES::PLANE_NORMAL::X:
        columnLength = BOARD_DIMS.x;
        indexIncriment.x = 1;
        position.z = column.x;
        position.y = column.y;
        break;
        case BOARD_TYPES::PLANE_NORMAL::Y:
        columnLength = BOARD_DIMS.y;
        indexIncriment.y = 1;
        position.x = column.x;
        position.z = column.y;
        break;

        default:
        case BOARD_TYPES::PLANE_NORMAL::Z:
        columnLength = BOARD_DIMS.z;
        indexIncriment.z = 1;
        position.x = column.x;
        position.y = column.y;
        break;
    }

    Cube* columnSlice[columnLength];
    for(uint32_t i = 0; i < columnLength; i++){
        V3D cubePosition = indexIncriment * i + position;
        columnSlice[i] = &(this->cubes[cubePosition.x][cubePosition.y][cubePosition.z]);
    }

    return columnSlice;
}

