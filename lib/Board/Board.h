#pragma once

#include <cstdint>
#include <array>

#include "Cube.h"
#include "Color.h"

template <uint32_t X_SIZE, uint32_t Y_SIZE, uint32_t Z_SIZE>
class Board{
    public:
    Board();
    ~Board() = default;

    constexpr uint32_t GetXSize() const{return X_SIZE;}
    constexpr uint32_t GetYSize() const{return Y_SIZE;}
    constexpr uint32_t GetZSize() const {return Z_SIZE;}
    constexpr uint32_t GetNumberCubes() const{return X_SIZE * Y_SIZE * Z_SIZE;}

    /**
     * @brief Returns a string in the format:
     *  !a,b,c,d,e,f,g,h,i;
     * Where each letter is the number of cubes in a given position's stack
     * @param stringBuffer the buffer to write the string into
     */
    void ToStackString(String& stringBuffer) const;

    /**
     * @brief Returns a bool array representing the board
     */
    std::array<uint32_t, X_SIZE * Y_SIZE> &LinearizeBoard() const;

    void Update();

    void FillColor(const Color &color);

    void SetCubeColor(uint32_t x_coord, uint32_t y_cord, uint32_t z_cord, const Color &color);

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
    std::array<std::array<std::array<Cube, Z_SIZE>, Y_SIZE>, X_SIZE> cubes;

    bool boardStateHasChanged;
};

template <uint32_t X_SIZE, uint32_t Y_SIZE, uint32_t Z_SIZE>
Board<X_SIZE, Y_SIZE, Z_SIZE>::Board(){
    this->FillColor(Color(0,0,0));
}

template <uint32_t X_SIZE, uint32_t Y_SIZE, uint32_t Z_SIZE>
void Board<X_SIZE, Y_SIZE, Z_SIZE>::ToStackString(String &stringBuffer) const{
    std::array<uint32_t, X_SIZE*Y_SIZE> linearizedBoard = this->LinearizeBoard();

    stringBuffer += "!" + String(linearizedBoard[0]);

    for(uint32_t i = 0; i < X_SIZE * Y_SIZE; i++){
        stringBuffer += "," + String(linearizedBoard[i]);
    }

    stringBuffer += ";";
}

template <uint32_t X_SIZE, uint32_t Y_SIZE, uint32_t Z_SIZE>
std::array<uint32_t, X_SIZE * Y_SIZE> & Board<X_SIZE, Y_SIZE, Z_SIZE>::LinearizeBoard() const{
    // convert the board into one array where each entry represents the height of one stack
    std::array<uint32_t, X_SIZE*Y_SIZE> linearizedBoard;
    for(uint32_t x{0}; x < X_SIZE; x++){
        for(uint32_t y{0}; y < Y_SIZE; y++){
            for(uint32_t z{0}; z < Z_SIZE; z++){
                bool isOccupied{this->cubes[x][y][z].isOccupied};
                linearizedBoard[x + y*3] += static_cast<uint32_t>(isOccupied);
            }
        }
    }
    return linearizedBoard;
}

template <uint32_t X_SIZE, uint32_t Y_SIZE, uint32_t Z_SIZE>
void Board<X_SIZE, Y_SIZE, Z_SIZE>::Update(){

}

template <uint32_t X_SIZE, uint32_t Y_SIZE, uint32_t Z_SIZE>
void Board<X_SIZE, Y_SIZE, Z_SIZE>::FillColor(const Color &color){
    for(uint32_t x{0}; x < X_SIZE; x++){
        for(uint32_t y{0}; y < Y_SIZE; y++){
            for(uint32_t z{0}; z < Z_SIZE; z++){
                this->cubes[x][y][z].SetColor(color);
            }
        }
    }
}

template <uint32_t X_SIZE, uint32_t Y_SIZE, uint32_t Z_SIZE>
void Board<X_SIZE, Y_SIZE, Z_SIZE>::SetCubeColor(uint32_t x_coord, uint32_t y_cord, uint32_t z_cord, const Color &color){
    this->cubes[x][y][z].SetColor(color);
}

