#pragma once

#include "AnimationTypes.h"
#include "Vector3D.h"
#include "Animator.h"

using namespace ANIMATION_TYPES;

namespace TestFrames{

    V3D<uint32_t> red{255,0,0};
    V3D<uint32_t> green{0,255,0};
    V3D<uint32_t> blue{0,0,255};
    uint32_t maxValue{std::numeric_limits<uint32_t>::max()};

    Cell CreateCell(float x_percent, float y_percent, float z_percent, V3D<uint32_t> &color){
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

    AnimationFrame noFillFrame{
        .frame = {
            CreateCell(0,0,0,red),
            // CreateCell(0.5,0.5,0,green),
            CreateCell(1,1,0,blue)
        },
        .fillInterpolation = FillInterpolation::NO_FILL,
        .frameInterpolation = FrameInterpolation::SNAP,
        .delay = std::chrono::milliseconds(10000)
    };

    AnimationFrame closestColorFrame{
        .frame = {
            CreateCell(0,0,0,red),
            // CreateCell(0.5,0.5,0,green),
            CreateCell(1,1,0,blue)
        },
        .fillInterpolation = FillInterpolation::CLOSEST_COLOR,
        .frameInterpolation = FrameInterpolation::SNAP,
        .delay = std::chrono::milliseconds(10000)
    };

    AnimationFrame linearFillFrame{
        .frame = {
            CreateCell(0,0,0,red),
            // CreateCell(0.5,0.5,0,green),
            CreateCell(1,1,0,blue)
        },
        .fillInterpolation = FillInterpolation::LINEAR_WEIGHTED_DISTANCE,
        .frameInterpolation = FrameInterpolation::SNAP,
        .delay = std::chrono::milliseconds(10000)
    };

    AnimationFrame squareFillFrame{
        .frame = {
            CreateCell(0,0,0,red),
            // CreateCell(0.5,0.5,0,green),
            CreateCell(1,1,0,blue)
        },
        .fillInterpolation = FillInterpolation::SQUARE_WEIGHTED_DISTANCE,
        .frameInterpolation = FrameInterpolation::SNAP,
        .delay = std::chrono::milliseconds(10000)
    };

    AnimationFrame noFillFadeFrame{
        .frame = {
            CreateCell(0,0,0,red),
            // CreateCell(0.5,0.5,0,green),
            CreateCell(1,1,0,blue)
        },
        .fillInterpolation = FillInterpolation::NO_FILL,
        .frameInterpolation = FrameInterpolation::FADE,
        .delay = std::chrono::milliseconds(10000)
    };

    AnimationFrame closestColorFadeFrame{
        .frame = {
            CreateCell(0,0,0,red),
            // CreateCell(0.5,0.5,0,green),
            CreateCell(1,1,0,blue)
        },
        .fillInterpolation = FillInterpolation::CLOSEST_COLOR,
        .frameInterpolation = FrameInterpolation::FADE,
        .delay = std::chrono::milliseconds(10000)
    };

    AnimationFrame linearFillFadeFrame{
        .frame = {
            CreateCell(0,0,0,red),
            // CreateCell(0.5,0.5,0,green),
            CreateCell(1,1,0,blue)
        },
        .fillInterpolation = FillInterpolation::LINEAR_WEIGHTED_DISTANCE,
        .frameInterpolation = FrameInterpolation::FADE,
        .delay = std::chrono::milliseconds(10000)
    };

    AnimationFrame squareFillFadeFrame{
        .frame = {
            CreateCell(0,0,0,red),
            // CreateCell(0.5,0.5,0,green),
            CreateCell(1,1,0,blue)
        },
        .fillInterpolation = FillInterpolation::SQUARE_WEIGHTED_DISTANCE,
        .frameInterpolation = FrameInterpolation::FADE,
        .delay = std::chrono::milliseconds(10000)
    };

    std::vector<AnimationFrame> testAnimationSequence2{
        noFillFrame, // 0
        closestColorFrame, // 1
        linearFillFrame, // 2
        squareFillFrame, // 3
        noFillFadeFrame, // 4
        closestColorFadeFrame, // 5
        linearFillFadeFrame, // 6
        squareFillFadeFrame // 7
    };

    AnimationFrame testFrame1{
        .frame = {
            CreateCell(0,0,0,red),
            // CreateCell(0.5,0.5,0,green),
            CreateCell(1,1,0,blue)
        },
        .fillInterpolation = FillInterpolation::NO_FILL,
        .frameInterpolation = FrameInterpolation::FADE,
        .delay = std::chrono::milliseconds(10000)
    };

    AnimationFrame testFrame2{
        .frame = {
            CreateCell(0,1,0,red),
            // CreateCell(0.5,0.5,0,green),
            CreateCell(1,0,0,green)
        },
        .fillInterpolation = FillInterpolation::NO_FILL,
        .frameInterpolation = FrameInterpolation::FADE,
        .delay = std::chrono::milliseconds(10000)
    };

    AnimationFrame testFrame3{
        .frame = {
            CreateCell(0.5,0.5,0,red),
            // CreateCell(0.5,0.5,0,green),
            CreateCell(0,1,0,blue)
        },
        .fillInterpolation = FillInterpolation::NO_FILL,
        .frameInterpolation = FrameInterpolation::FADE,
        .delay = std::chrono::milliseconds(10000)
    };

    std::vector<AnimationFrame> testAnimationSequence1{
        testFrame1,
        testFrame2,
        testFrame3,
        testFrame1
    };
}