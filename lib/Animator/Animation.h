#pragma once

#include "AnimationTypes.h"
#include "Vector3D.h"
#include "Animator.h"

using namespace ANIMATION_TYPES;

namespace AnimationHelpers{
    V3D<uint8_t> red{255,0,0};
    V3D<uint8_t> green{0,255,0};
    V3D<uint8_t> blue{0,0,255};
    V3D<uint8_t> cyan{0,255,255};
    V3D<uint8_t> magenta{255,0,255};

    Cell CreateCell(float x_percent, float y_percent, float z_percent, const V3D<uint8_t> &color){
        float continuousMaxValue{static_cast<float>(std::numeric_limits<uint32_t>::max())};
        Cell cell{
            .position = V3D<uint32_t>{
                static_cast<uint32_t>(continuousMaxValue*x_percent),
                static_cast<uint32_t>(continuousMaxValue*y_percent),
                static_cast<uint32_t>(continuousMaxValue*z_percent)
            },
            .color = color
        };

        return cell;
    }
}

namespace RotatingCubes{
    using namespace AnimationHelpers;

    AnimationFrame frame1{
        .frame = {
            CreateCell(0,0,0,red),
            CreateCell(1,0.5,0,green),
            CreateCell(0,1,0,blue)
        },
        .fillInterpolation = FillInterpolation::CLOSEST_COLOR,
        .frameInterpolation = FrameInterpolation::FADE,
        .delay = std::chrono::milliseconds(1000)
    };

    AnimationFrame frame2{
        .frame = {
            CreateCell(0,0.5,0,red),
            CreateCell(1,0,0,green),
            CreateCell(0.5,1,0,blue)
        },
        .fillInterpolation = FillInterpolation::CLOSEST_COLOR,
        .frameInterpolation = FrameInterpolation::FADE,
        .delay = std::chrono::milliseconds(500)
    };

    AnimationFrame frame3{
        .frame = {
            CreateCell(0,1,0,red),
            CreateCell(0.5,0,0,green),
            CreateCell(1,1,0,blue)
        },
        .fillInterpolation = FillInterpolation::CLOSEST_COLOR,
        .frameInterpolation = FrameInterpolation::FADE,
        .delay = std::chrono::milliseconds(1000)
    };

    AnimationFrame frame4{
        .frame = {
            CreateCell(0.5,1,0,red),
            CreateCell(0,0,0,green),
            CreateCell(1,0.5,0,blue)
        },
        .fillInterpolation = FillInterpolation::CLOSEST_COLOR,
        .frameInterpolation = FrameInterpolation::FADE,
        .delay = std::chrono::milliseconds(500)
    };

    AnimationFrame frame5{
        .frame = {
            CreateCell(1,1,0,red),
            CreateCell(0,0.5,0,green),
            CreateCell(1,0,0,blue)
        },
        .fillInterpolation = FillInterpolation::CLOSEST_COLOR,
        .frameInterpolation = FrameInterpolation::FADE,
        .delay = std::chrono::milliseconds(1000)
    };

    AnimationFrame frame6{
        .frame = {
            CreateCell(1,0.5,0,red),
            CreateCell(0,1,0,green),
            CreateCell(0.5,0,0,blue)
        },
        .fillInterpolation = FillInterpolation::CLOSEST_COLOR,
        .frameInterpolation = FrameInterpolation::FADE,
        .delay = std::chrono::milliseconds(500)
    };

    AnimationFrame frame7{
        .frame = {
            CreateCell(1,0,0,red),
            CreateCell(0.5,1,0,green),
            CreateCell(0,0,0,blue)
        },
        .fillInterpolation = FillInterpolation::CLOSEST_COLOR,
        .frameInterpolation = FrameInterpolation::FADE,
        .delay = std::chrono::milliseconds(1000)
    };

    AnimationFrame frame8{
        .frame = {
            CreateCell(0.5,0,0,red),
            CreateCell(1,1,0,green),
            CreateCell(0,0.5,0,blue)
        },
        .fillInterpolation = FillInterpolation::CLOSEST_COLOR,
        .frameInterpolation = FrameInterpolation::FADE,
        .delay = std::chrono::milliseconds(500)
    };

    AnimationFrame frame9{
        .frame = {
            CreateCell(0,0,0,red),
            CreateCell(1,0.5,0,green),
            CreateCell(0,1,0,blue)
        },
        .fillInterpolation = FillInterpolation::CLOSEST_COLOR,
        .frameInterpolation = FrameInterpolation::FADE,
        .delay = std::chrono::milliseconds(1)
    };

    std::vector<AnimationFrame> rotating{
        frame1, // 0
        frame2, // 1
        frame3, // 2
        frame4, // 3
        frame5, // 4
        frame6, // 5
        frame7, // 6
        frame8, // 7
        frame9, // 8
    };
}

namespace RisingCubes{
    using namespace AnimationHelpers;

    AnimationFrame frame1{
        .frame = {
            CreateCell(0,0,0,cyan),
            CreateCell(0,1,0.5,green),
            CreateCell(1,0,1,blue),
            CreateCell(0.5,0.5,0.5,red),
            CreateCell(1,1,0,magenta)
        },
        .fillInterpolation = FillInterpolation::LINEAR_WEIGHTED_DISTANCE,
        .frameInterpolation = FrameInterpolation::FADE,
        .delay = std::chrono::milliseconds(800)
    };

    AnimationFrame frame2{
        .frame = {
            CreateCell(0,0,0.5,cyan),
            CreateCell(0,1,1,green),
            CreateCell(1,0,0.5,blue),
            CreateCell(0.5,0.5,0,red),
            CreateCell(1,1,0.5,magenta)
        },
        .fillInterpolation = FillInterpolation::LINEAR_WEIGHTED_DISTANCE,
        .frameInterpolation = FrameInterpolation::FADE,
        .delay = std::chrono::milliseconds(800)
    };

    AnimationFrame frame3{
        .frame = {
            CreateCell(0,0,1,cyan),
            CreateCell(0,1,0.5,green),
            CreateCell(1,0,0,blue),
            CreateCell(0.5,0.5,0.5,red),
            CreateCell(1,1,1,magenta)
        },
        .fillInterpolation = FillInterpolation::LINEAR_WEIGHTED_DISTANCE,
        .frameInterpolation = FrameInterpolation::FADE,
        .delay = std::chrono::milliseconds(800)
    };

    AnimationFrame frame4{
        .frame = {
            CreateCell(0,0,0.5,cyan),
            CreateCell(0,1,0,green),
            CreateCell(1,0,0.5,blue),
            CreateCell(0.5,0.5,1,red),
            CreateCell(1,1,0.5,magenta)
        },
        .fillInterpolation = FillInterpolation::LINEAR_WEIGHTED_DISTANCE,
        .frameInterpolation = FrameInterpolation::FADE,
        .delay = std::chrono::milliseconds(800)
    };

    AnimationFrame frame5{
        .frame = {
            CreateCell(0,0,0,cyan),
            CreateCell(0,1,0.5,green),
            CreateCell(1,0,1,blue),
            CreateCell(0.5,0.5,0.5,red),
            CreateCell(1,1,0,magenta)
        },
        .fillInterpolation = FillInterpolation::LINEAR_WEIGHTED_DISTANCE,
        .frameInterpolation = FrameInterpolation::FADE,
        .delay = std::chrono::milliseconds(1)
    };

    AnimationFrame frame0{
        .frame = {
            CreateCell(0.0,0.0,0.0,V3D<uint8_t>(255.0,255.0,255.0)),
            CreateCell(0.0,0.5,0.0,V3D<uint8_t>(0.0,255.0,0.0)),
            CreateCell(0.0,1.0,0.0,V3D<uint8_t>(0.0,255.0,0.0)),
            CreateCell(0.0,0.0,0.5,V3D<uint8_t>(0.0,0.0,255.0)),
            CreateCell(0.0,0.0,1.0,V3D<uint8_t>(0.0,0.0,255.0)),
            CreateCell(0.5,0.0,0.0,V3D<uint8_t>(255.0,0.0,0.0)),
            CreateCell(1.0,0.0,0.0,V3D<uint8_t>(255.0,0.0,0.0))
        },
        .fillInterpolation = FillInterpolation::NO_FILL,
        .frameInterpolation = FrameInterpolation::FADE,
        .delay = std::chrono::milliseconds(1000)
    };

    std::vector<AnimationFrame> rising{
        frame0, frame0
        // frame1, // 0
        // frame2, // 1
        // frame3, // 2
        // frame4, // 3
        // frame5
    };
}