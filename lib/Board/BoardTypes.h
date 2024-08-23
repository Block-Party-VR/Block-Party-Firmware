#pragma once

#include <cstdint>

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
};