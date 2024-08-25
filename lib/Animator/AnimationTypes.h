#pragma once
#include "Vector3D.h"
#include <vector>
#include <chrono>

namespace ANIMATION_TYPES{
    // for cube spots which aren't defined in a key frame, 
    // you can have the controller automatically interpolate a color
    enum FillInterpolation{
        NO_FILL, // if not specified, the cube color will be black
        CLOSEST_COLOR, // The cube color will be the same color as the cube closest to it
        LINEAR_WEIGHTED_DISTANCE, // the cube color will be an average of all specified cube colors weighted by the linear distance to this cube
        SQUARE_WEIGHTED_DISTANCE // same as linear, but further colors have exponentially less impact on the color
    };

    enum FrameInterpolation{
        SNAP, // After the delay, snap to the next key frame
        FADE // over the course of the delay, fade to the next frame
    };

    struct Cell{
        V3D<uint32_t> position;
        V3D<uint32_t> color;
    };

    // this contains all of the information to specify exactly how a single frame should look and fade to the next frame
    struct AnimationFrame{
        std::vector<Cell> frame;
        FillInterpolation fillInterpolation;
        FrameInterpolation frameInterpolation;
        std::chrono::milliseconds delay;
    };
};