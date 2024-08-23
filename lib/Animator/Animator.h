#pragma once 

#include <cstdint>
#include <array>
#include <chrono>
#include <cstring>
#include <cmath>
#include <climits>

#include "Vector3D.h"


class Animation{
public:
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
    V3D position;
    V3D color;
};

// this contains all of the information to specify exactly how a single frame should look and fade to the next frame
struct AnimationFrame{
    std::vector<Cell> frame;
    FillInterpolation fillInterpolation;
    FrameInterpolation frameInterpolation;
    std::chrono::milliseconds delay;
};

typedef std::array<std::array<std::array<V3D, Z_SIZE>, Y_SIZE>, X_SIZE> Frame;


void StartAnimation(const std::vector<AnimationFrame> &animationSequence){
    this->animationSequence = animationSequence;
    this->animationIndex = 0;
    this->timeElapsed = std::chrono::milliseconds(0);

    if(animationSequence.size() == 0){
        return;
    }
    else if(animationSequence.size() == 1){
        this->uncompressFrame(animationSequence[0], this->startFrame);
        this->uncompressFrame(animationSequence[0], this->endFrame);
    }
    else{
        this->uncompressFrame(animationSequence[0], this->startFrame);
        this->uncompressFrame(animationSequence[1], this->endFrame);
    }
}

Frame &RunAnimation(std::chrono::milliseconds timePassed){
    auto delayTime = this->animationSequence[this->animationIndex].delay;
    this->timeElapsed += timePassed;
    Frame interpolatedFrame;
    
    // load in the next frame if we're done with this transition
    if(this->timeElapsed >= delayTime){
        this->incrimentAnimationIndex();
    }

    // don't do frame interpolations if we're doing snap fades
    if(this->animationSequence[this->animationIndex].frameInterpolation == FrameInterpolation::SNAP){
        return;
    }

    // linearly interpolate between the two uncompressed frames
    for(uint32_t x = 0; x < this->X_SIZE; x++){
        for(uint32_t y = 0; y < this->Y_SIZE; y++){
            for(uint32_t z = 0; z < this->Z_SIZE; z++){
                V3D startColor{this->startFrame[x][y][z]};
                V3D endColor{this->endFrame[x][y][z]};
                V3D difference{endColor - startColor};
                V3D interpolatedColor = this->timeElapsed.count() * difference / delayTime.count() + startColor;
                interpolatedFrame[x][y][z] = interpolatedColor;
            }
        }
    }
}

void SetLoop(bool isLooping){
    this->isLooping = isLooping;
}

private:
uint32_t X_SIZE{3};
uint32_t Y_SIZE{3};
uint32_t Z_SIZE{3};
bool isLooping{true};
// these are the uncompressed frames you get by following the key colors and interpolation instructions of an animation frame
Frame startFrame;
Frame endFrame;
std::chrono::milliseconds timeElapsed;

const std::vector<AnimationFrame> & animationSequence;
uint32_t animationIndex{0};

void incrimentAnimationIndex(){
    if(this->animationIndex < animationSequence.size() - 1){
        this->animationIndex++;
        this->timeElapsed = std::chrono::millis(0);
        this->uncompressFrame(this->animationSequence[this->animationIndex], this->startFrame);
        this->uncompressFrame(this->animationSequence[this->animationIndex + 1], this->endFrame);
    }
}

void uncompressFrame(const AnimationFrame &keyFrame, Frame &frameBuffer){
    for(uint32_t x = 0; x < X_SIZE; x++){
        for(uint32_t y = 0; y < Y_SIZE; y++){
            for(uint32_t z = 0; z < Z_SIZE; z++){
                frameBuffer[x][y][z] = getInterpolatedColor(keyFrame, V3D(x, y, z));
            }
        }
    }
}

V3D &getInterpolatedColor(const AnimationFrame &keyFrame, V3D position){
    switch(keyFrame.fillInterpolation){
        case FillInterpolation::NO_FILL:
            return noFillInterpolate(keyFrame, position);
        case FillInterpolation::CLOSEST_COLOR:
            return closestColorInterpolate(keyFrame, position);
        case FillInterpolation::LINEAR_WEIGHTED_DISTANCE:
            return linearInterpolate(keyFrame, position);
        case FillInterpolation::SQUARE_WEIGHTED_DISTANCE:
            return squareInterpolate(keyFrame, position);
        default:
            return V3D{};
    }
}

V3D &keyFrame2BoardCoords(V3D &keyFramePosition){
    V3D returnValue{};
    float maxValue{static_cast<float>(std::numeric_limits<uint32_t>::max())};
    // scale the key frame values down to be within board coordinates
    float keyFrame_X = static_cast<float>(this->X_SIZE) * static_cast<float>(keyFramePosition.x) / maxValue;
    float keyFrame_Y = static_cast<float>(this->Y_SIZE) * static_cast<float>(keyFramePosition.y) / maxValue;
    float keyFrame_Z = static_cast<float>(this->Z_SIZE) * static_cast<float>(keyFramePosition.z) / maxValue;

    // carefully quantize the float values back into ints with a precise rounding operation
    if(keyFrame_X - std::floor(keyFrame_X) < 0.5f){
        returnValue.x = static_cast<uint32_t>(keyFrame_X);
    }
    else{
        returnValue.x = static_cast<uint32_t>(keyFrame_X) + 1;
    }

    if(keyFrame_Y - std::floor(keyFrame_Y) < 0.5f){
        returnValue.y = static_cast<uint32_t>(keyFrame_Y);
    }
    else{
        returnValue.y = static_cast<uint32_t>(keyFrame_Y) + 1;
    }

    if(keyFrame_Z - std::floor(keyFrame_Z) < 0.5f){
        returnValue.z = static_cast<uint32_t>(keyFrame_Z);
    }
    else{
        returnValue.z = static_cast<uint32_t>(keyFrame_Z) + 1;
    }
    
    return returnValue;
}

V3D &noFillInterpolate(const AnimationFrame &keyFrame, V3D position){
    V3D returnColor{};
    for(Cell cell : keyFrame.frame){
        if(keyFrame2BoardCoords(cell.position) == position){
            returnColor = cell.color;
        }
    }

    return returnColor;
}

V3D &closestColorInterpolate(const AnimationFrame &keyFrame, V3D position){
    V3D returnColor{};
    float closestDistance = (keyframe.frame[0].position - position).mag();
    for(Cell cell : keyFrame.frame){
        float distance = (keyFrame2BoardCoords(cell.position) - position).mag();
        if(distance < closestDistance){
            returnColor = cell.color;
            closestDistance = distance;
        }
    }

    return returnColor;
}

V3D &linearInterpolate(const AnimationFrame &keyFrame, V3D position){
    V3D returnColor{};

    for(Cell cell : keyFrame.frame){
        uint32_t distance = static_cast<uint32_t>((keyFrame2BoardCoords(cell.position) - position).mag());
        if(distance == 0) distance = 1;
        returnColor = returnColor + cell.color / distance;
    }

    returnColor = returnColor / keyFrame.frame.size();

    return returnColor;
}

V3D &squareInterpolate(const AnimationFrame &keyFrame, V3D position){
    V3D returnColor{};

    for(Cell cell : keyFrame.frame){
        uint32_t distance = static_cast<uint32_t>((keyFrame2BoardCoords(cell.position) - position).mag());
        distance *= distance;
        if(distance == 0) distance = 1;
        returnColor = returnColor + cell.color / distance;
    }

    returnColor = returnColor / keyFrame.frame.size();

    return returnColor;
}
};

// let's make some test animation frames

namespace TestFrames{
    V3D red{255,0,0};
    V3D green{0,255,0};
    V3D blue{0,0,255};
    uint32_t maxValue{std::numeric_limits<uint32_t>::max()};

    Animation::Cell &CreateCell(float x_percent, float y_percent, float z_percent, V3D &color){
        float continuousMaxValue{static_cast<float>(std::numeric_limits<uint32_t>::max())};
        Animation::Cell cell{
            .position = V3D{
                static_cast<uint32_t>(continuousMaxValue*x_percent),
                static_cast<uint32_t>(continuousMaxValue*y_percent),
                static_cast<uint32_t>(continuousMaxValue*z_percent)
            },
            .color = color
        };

        return cell;
    }

    Animation::AnimationFrame noFillFrame{
        .frame = {
            CreateCell(0,0,0,red),
            CreateCell(0.5,0.5,0.5,green),
            CreateCell(1,1,1,blue)
        };
        .fillInterpolation = FillInterpolation::NO_FILL;
        .frameInterpolation = FrameInterpolation::SNAP;
        .delay = std::chrono::millis(10000);
    }

    Animation::AnimationFrame closestColorFrame{
        .frame = {
            CreateCell(0,0,0,red),
            CreateCell(0.5,0.5,0.5,green),
            CreateCell(1,1,1,blue)
        };
        .fillInterpolation = FillInterpolation::CLOSEST_COLOR;
        .frameInterpolation = FrameInterpolation::SNAP;
        .delay = std::chrono::millis(10000);
    }

    Animation::AnimationFrame linearFillFrame{
        .frame = {
            CreateCell(0,0,0,red),
            CreateCell(0.5,0.5,0.5,green),
            CreateCell(1,1,1,blue)
        };
        .fillInterpolation = FillInterpolation::LINEAR_WEIGHTED_DISTANCE;
        .frameInterpolation = FrameInterpolation::SNAP;
        .delay = std::chrono::millis(10000);
    }

    Animation::AnimationFrame squareFillFrame{
        .frame = {
            CreateCell(0,0,0,red),
            CreateCell(0.5,0.5,0.5,green),
            CreateCell(1,1,1,blue)
        };
        .fillInterpolation = FillInterpolation::SQUARE_WEIGHTED_DISTANCE;
        .frameInterpolation = FrameInterpolation::SNAP;
        .delay = std::chrono::millis(10000);
    }

    Animation::AnimationFrame noFillFadeFrame{
        .frame = {
            CreateCell(0,0,0,red),
            CreateCell(0.5,0.5,0.5,green),
            CreateCell(1,1,1,blue)
        };
        .fillInterpolation = FillInterpolation::NO_FILL;
        .frameInterpolation = FrameInterpolation::FADE;
        .delay = std::chrono::millis(10000);
    }

    Animation::AnimationFrame closestColorFadeFrame{
        .frame = {
            CreateCell(0,0,0,red),
            CreateCell(0.5,0.5,0.5,green),
            CreateCell(1,1,1,blue)
        };
        .fillInterpolation = FillInterpolation::CLOSEST_COLOR;
        .frameInterpolation = FrameInterpolation::FADE;
        .delay = std::chrono::millis(10000);
    }

    Animation::AnimationFrame linearFillFadeFrame{
        .frame = {
            CreateCell(0,0,0,red),
            CreateCell(0.5,0.5,0.5,green),
            CreateCell(1,1,1,blue)
        };
        .fillInterpolation = FillInterpolation::LINEAR_WEIGHTED_DISTANCE;
        .frameInterpolation = FrameInterpolation::FADE;
        .delay = std::chrono::millis(10000);
    }

    Animation::AnimationFrame squareFillFadeFrame{
        .frame = {
            CreateCell(0,0,0,red),
            CreateCell(0.5,0.5,0.5,green),
            CreateCell(1,1,1,blue)
        };
        .fillInterpolation = FillInterpolation::SQUARE_WEIGHTED_DISTANCE;
        .frameInterpolation = FrameInterpolation::FADE;
        .delay = std::chrono::millis(10000);
    }

    std::vector<AnimationFrame> testAnimationSequence{
        noFillFrame,
        closestColorFrame,
        linearFillFrame,
        squareFillFrame,
        noFillFadeFrame,
        closestFillFadeFrame,
        linearFillFadeFrame,
        squareFillFadeFrame
    };
}


