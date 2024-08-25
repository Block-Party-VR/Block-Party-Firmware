#pragma once

#include <cstdint>
#include <cmath>

class V3D{
    public:
    constexpr V3D(const V3D& other):
    x(other.x),
    y(other.y),
    z(other.z){}
    
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

    V3D& operator+=(const V3D &other){
        this->x += other.x;
        this->y += other.y;
        this->z += other.z;
        return *this;
    }

    V3D& operator-=(const V3D &other){
        this->x -= other.x;
        this->y -= other.y;
        this->z -= other.z;
        return *this;
    }

    V3D& operator/=(const uint32_t scalar){
        if(scalar == 0){
            return *this;
        }
        this->x /= scalar;
        this->y /= scalar;
        this->z /= scalar;
        return *this;
    }

    V3D& operator*=(const uint32_t scalar){
        this->x *= scalar;
        this->y *= scalar;
        this->z *= scalar;
        return *this;
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