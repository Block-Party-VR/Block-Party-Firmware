/**
 * @file BoardTypes.h
 * @brief Some types to that you'll need to define and control the board
 */
#pragma once

#include <cstdint>
#include "Vector3D.h"

namespace BOARD_TYPES{
    struct CubeStack{
        uint8_t adcPin;
        uint8_t ledPin;
    };

    enum PLANE_NORMAL : uint32_t{
        X = 0,
        Y,
        Z
    };

    struct Cube{
        V3D<uint32_t> color;
        bool isOccupied{false};
    };
};