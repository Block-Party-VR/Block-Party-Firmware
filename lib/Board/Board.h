/**
 * @file Board.h
 * @brief The Board class is meant to store data about each cube on the board and 
 * provide an easy way to organize and access that data in a 3-dimensional context.
 */
#pragma once

#include <cstdint>
#include <array>
#include <WString.h>

#include "BoardTypes.h"
#include "Vector3D.h"

template <const V3D &BOARD_DIMS>
class Board{
    public:
    Board() = default;
    ~Board() = default;

    constexpr const V3D &GetSize() const{return BOARD_DIMS;}
    constexpr uint32_t GetNumberCubes() const{return BOARD_DIMS.x * BOARD_DIMS.y * BOARD_DIMS.z;}

    constexpr uint32_t GetMaxDimension(){return std::max(std::max(BOARD_DIMS.x, BOARD_DIMS.y), BOARD_DIMS.z);}
    /**
     * @brief Returns a string in the format:
     *  !a,b,c,d,e,f,g,h,i;
     * Where each letter is the number of cubes in a given position's stack
     * @param stringBuffer the buffer to write the string into
     */
    void ToStackString(String& stringBuffer) const;

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
     * @brief Get a column along any axis read into the sliceBuffer
     * @param column .z specifies the normal direction of the plane (see PLANE_NORMAL), and 
     * the x,y values specify the location of the column in that plane 
     * to fill. IE To fill one stack at 0,2 I would say give V3D(0,2,PLANE_NORMAL::Z)
     * @param sliceBuffer an array of pointers to the cubes along that column
     * @returns the number of elements written into the slice buffer
     * @note That array is stored locally and will be overwritten everytime this function is called. 
     * Also, any unused spots at the end of the array will be nullptrs
     * @warning allocate the size of the slice buffer using GetMaxDimension if you don't know what you're doing!
     */
    uint32_t SliceBoard(const V3D &column, BOARD_TYPES::Cube ** sliceBuffer);

    void PrintEntireBoard() const;
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
    std::array<std::array<std::array<BOARD_TYPES::Cube, BOARD_DIMS.z>, BOARD_DIMS.y>, BOARD_DIMS.x> cubes;

    bool boardStateHasChanged;
};

template <const V3D &BOARD_DIMS>
void Board<BOARD_DIMS>::ToStackString(String &stringBuffer) const{
    std::array<uint32_t, BOARD_DIMS.x * BOARD_DIMS.y> linearizedBoard;
    for(uint32_t x{0}; x < BOARD_DIMS.x; x++){
        for(uint32_t y{0}; y < BOARD_DIMS.y; y++){
            uint32_t boardIndex{x + y*3};
            linearizedBoard[boardIndex] = 0;
            for(uint32_t z{0}; z < BOARD_DIMS.z; z++){
                linearizedBoard[boardIndex] += this->cubes[x][y][z].isOccupied;
            }
        }
    }

    stringBuffer += String(linearizedBoard[0]);

    for(uint32_t i = 0; i < BOARD_DIMS.x * BOARD_DIMS.y; i++){
        stringBuffer += "," + String(linearizedBoard[i]);
    }
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
uint32_t Board<BOARD_DIMS>::SliceBoard(const V3D &column, BOARD_TYPES::Cube ** sliceBuffer){
    uint32_t columnLength{0};
    V3D indexIncrimentVector{};
    V3D indexVector{};

    switch(column.z){
        case BOARD_TYPES::PLANE_NORMAL::X:
        columnLength = BOARD_DIMS.x;
        indexIncrimentVector.x = 1;
        indexVector.z = column.x;
        indexVector.y = column.y;
        break;
        case BOARD_TYPES::PLANE_NORMAL::Y:
        columnLength = BOARD_DIMS.y;
        indexIncrimentVector.y = 1;
        indexVector.x = column.x;
        indexVector.z = column.y;
        break;

        default:
        case BOARD_TYPES::PLANE_NORMAL::Z:
        columnLength = BOARD_DIMS.z;
        indexIncrimentVector.z = 1;
        indexVector.x = column.x;
        indexVector.y = column.y;
        break;
    }

    for(uint32_t i = 0; i < columnLength; i++){
        if(indexVector.x >= BOARD_DIMS.x || indexVector.y >= BOARD_DIMS.y || indexVector.z >= BOARD_DIMS.z){
            Serial.println("Board::SliceBoard: Index Out of Bounds:" + String(indexVector.x) + "," + String(indexVector.y) + "," + String(indexVector.z));
            return 0;
        }
        sliceBuffer[i] = &(this->cubes[indexVector.x][indexVector.y][indexVector.z]);
        indexVector += indexIncrimentVector;
    }
    return columnLength;
}

template <const V3D &BOARD_DIMS>
void Board<BOARD_DIMS>::PrintEntireBoard() const{
    for(uint32_t x = 0; x < BOARD_DIMS.x; x++){
        for(uint32_t y = 0; y < BOARD_DIMS.y; y++){
            for(uint32_t z = 0; z < BOARD_DIMS.z; z++){
                const BOARD_TYPES::Cube &cube = this->cubes[x][y][z];
                Serial.print("Cube X:" + String(x) + ",Y:" + String(y) + ",Z:" + String(z));
                Serial.print("\tColor R:" + String(cube.color.x) + ",G:" + String(cube.color.y) + ",B:" + String(cube.color.z));
                Serial.println("\tOccupied? " + String(cube.isOccupied));
            }
        }
    }
}

