#pragma once

#include <cstdint>
#include <cmath>

class V3D{
    public:
    constexpr V3D(uint32_t x=0, uint32_t y=0, uint32_t z=0): 
    x(x), 
    y(y), 
    z(z){}

    V3D& operator=(const V3D &other){
        this->x = other.x;
        this->y = other.y;
        this->z = other.z;
        return *this;
    }

    V3D& operator+(const V3D &other){
        V3D vector{};
        vector.x = this->x + other.x;
        vector.y = this->y + other.y;
        vector.z = this->z + other.z;
        return vector;
    }

    V3D& operator-(const V3D &other){
        V3D vector{};
        vector.x = this->x - other.x;
        vector.y = this->y - other.y;
        vector.z = this->z - other.z;
        return vector;
    }

    V3D operator/(const uint32_t scalar){
        V3D vector{};
        vector.x = this->x / scalar;
        vector.y = this->y / scalar;
        vector.z = this->z / scalar;
        return vector;
    }

    V3D operator*(const uint32_t scalar){
        V3D vector{};
        vector.x = this->x * scalar;
        vector.y = this->y * scalar;
        vector.z = this->z * scalar;
        return vector;
    }

    bool operator==(const V3D &other){
        return this->x == other.x && this->y == other.y && this->z == other.z;
    }

    float magnitude(){
        return std::sqrt(this->x * this->x + this->y * this->y + this->z * this-> z);
    }
    uint32_t x;
    uint32_t y;
    uint32_t z;
};