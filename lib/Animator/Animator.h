#pragma once 

#include <cstdint>
#include <array>
#include <vector>
#include <chrono>
#include <cstring>
#include <cmath>
#include <climits>

#include "Vector3D.h"
#include "AnimationTypes.h"

using namespace ANIMATION_TYPES;

template <const V3D<uint32_t> &BOARD_DIMS>
class Animator{

public:
typedef std::array<std::array<std::array<V3D<uint32_t>, BOARD_DIMS.z>, BOARD_DIMS.y>, BOARD_DIMS.x> Frame;


void StartAnimation(const std::vector<AnimationFrame> *animationSequence);

void RunAnimation(const std::chrono::milliseconds& timePassed);

void SetLoop(bool isLooping);

Frame &GetInterpolatedFrame(){return this->interpolatedFrame;}

bool isEnabled{true};
bool interpolatedFrameHasChanged{false};

private:
bool isLooping{true};
// these are the uncompressed frames you get by following the key colors and interpolation instructions of an animation frame
Frame startFrame;
Frame interpolatedFrame;
Frame endFrame;
std::chrono::milliseconds timeElapsed;

const std::vector<AnimationFrame> *animationSequence;
uint32_t animationIndex{0};

void incrimentAnimationIndex();

void uncompressFrame(const AnimationFrame &keyFrame, Frame &frameBuffer);

void copyFrame(Frame &copyFrom, Frame &copyTo){
    std::memcpy(&copyTo, &copyFrom, sizeof(Frame));
}

V3D<uint32_t> getInterpolatedColor(const AnimationFrame &keyFrame, V3D<uint32_t> position);

V3D<uint32_t> keyFrame2BoardCoords(const V3D<uint32_t> &keyFramePosition);

V3D<uint32_t> noFillInterpolate(const AnimationFrame &keyFrame, V3D<uint32_t> position);

V3D<uint32_t> closestColorInterpolate(const AnimationFrame &keyFrame, V3D<uint32_t> position);

V3D<uint32_t> linearInterpolate(const AnimationFrame &keyFrame, V3D<uint32_t> position);

V3D<uint32_t> squareInterpolate(const AnimationFrame &keyFrame, V3D<uint32_t> position);

void PrintUncompressedFrame(){      
    for(uint32_t x = 0; x < BOARD_DIMS.x; x++){
        for(uint32_t y = 0; y < BOARD_DIMS.y; y++){
            for(uint32_t z = 0; z < BOARD_DIMS.z; z++){
                auto color = this->startFrame[x][y][z];
                Serial.print("Cube X:" + String(x) + ",Y:" + String(y) + ",Z:" + String(z));
                Serial.println("\tColor R:" + String(color.x) + ",G:" + String(color.y) + ",B:" + String(color.z));
            }
        }
    }
}

};

template <const V3D<uint32_t> &BOARD_DIMS>
void Animator<BOARD_DIMS>::StartAnimation(const std::vector<AnimationFrame> *animationSequence){

    this->animationSequence = animationSequence;
    this->animationIndex = 0;
    this->timeElapsed = std::chrono::milliseconds(0);

    if(animationSequence->size() == 0){
        return;
    }
    else if(animationSequence->size() == 1){
        AnimationFrame frame{((*this->animationSequence)[0])};
        this->uncompressFrame(frame, this->startFrame);
        this->copyFrame(this->startFrame, this->interpolatedFrame);
        this->copyFrame(this->startFrame, this->endFrame);
    }
    else{
        this->uncompressFrame((*this->animationSequence)[0], this->startFrame);
        this->copyFrame(this->startFrame, this->interpolatedFrame);
        this->uncompressFrame((*this->animationSequence)[1], this->endFrame);
    }
    this->interpolatedFrameHasChanged = true;
}

template <const V3D<uint32_t> &BOARD_DIMS>
void Animator<BOARD_DIMS>::RunAnimation(const std::chrono::milliseconds& timePassed){
    if(!(this->isEnabled)){
        return;
    }

    auto delayTime = (*this->animationSequence)[this->animationIndex].delay;
    this->timeElapsed += timePassed;
    
    // load in the next frame if we're done with this transition
    if(this->timeElapsed >= delayTime){
        this->incrimentAnimationIndex();
    }

    // don't do frame interpolations if we're doing snap fades
    if((*this->animationSequence)[this->animationIndex].frameInterpolation == FrameInterpolation::SNAP){
        return;
    }

    // linearly interpolate between the two uncompressed frames
    for(uint32_t x = 0; x < BOARD_DIMS.x; x++){
        for(uint32_t y = 0; y < BOARD_DIMS.y; y++){
            for(uint32_t z = 0; z < BOARD_DIMS.z; z++){
                V3D<float> startColor{this->startFrame[x][y][z]};
                V3D<float> endColor{this->endFrame[x][y][z]};
                V3D<float> difference{endColor}; 
                difference -= startColor;

                V3D<float> interpolatedColor{difference};
                interpolatedColor *= this->timeElapsed.count();
                interpolatedColor /= delayTime.count();
                interpolatedColor += startColor;

                this->interpolatedFrame[x][y][z] = interpolatedColor;
            }
        }
    }

    this->interpolatedFrameHasChanged = true;
}

template <const V3D<uint32_t> &BOARD_DIMS>
void Animator<BOARD_DIMS>::SetLoop(bool isLooping){
    this->isLooping = isLooping;
}

template <const V3D<uint32_t> &BOARD_DIMS>
void Animator<BOARD_DIMS>::incrimentAnimationIndex(){
    if(this->animationIndex < this->animationSequence->size() - 2){
        this->animationIndex++;
        this->timeElapsed = std::chrono::milliseconds(0);
        this->uncompressFrame((*this->animationSequence)[this->animationIndex], this->startFrame);
        this->copyFrame(this->startFrame, this->interpolatedFrame);
        this->uncompressFrame((*this->animationSequence)[this->animationIndex + 1], this->endFrame);
    }
    else{
        this->StartAnimation(this->animationSequence);
    }

    this->interpolatedFrameHasChanged = true;
}

template <const V3D<uint32_t> &BOARD_DIMS>
void Animator<BOARD_DIMS>::uncompressFrame(const AnimationFrame &keyFrame, Frame &frameBuffer){
    for(uint32_t x = 0; x < BOARD_DIMS.x; x++){
        for(uint32_t y = 0; y < BOARD_DIMS.y; y++){
            for(uint32_t z = 0; z < BOARD_DIMS.z; z++){
                frameBuffer[x][y][z] = getInterpolatedColor(keyFrame, V3D<uint32_t>(x, y, z));
            }
        }
    }
}

template <const V3D<uint32_t> &BOARD_DIMS>
V3D<uint32_t> Animator<BOARD_DIMS>::getInterpolatedColor(const AnimationFrame &keyFrame, V3D<uint32_t> position){
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
            V3D<uint32_t> black{};
            return black;
    }
}

template <const V3D<uint32_t> &BOARD_DIMS>
V3D<uint32_t> Animator<BOARD_DIMS>::keyFrame2BoardCoords(const V3D<uint32_t> &keyFramePosition){
    V3D<uint32_t> returnValue{};
    float maxValue{static_cast<float>(std::numeric_limits<uint32_t>::max())};
    // scale the key frame values down to be within board coordinates
    float keyFrame_X = static_cast<float>(BOARD_DIMS.x - 1) * static_cast<float>(keyFramePosition.x) / maxValue;
    float keyFrame_Y = static_cast<float>(BOARD_DIMS.y - 1) * static_cast<float>(keyFramePosition.y) / maxValue;
    float keyFrame_Z = static_cast<float>(BOARD_DIMS.z - 1) * static_cast<float>(keyFramePosition.z) / maxValue;

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

template <const V3D<uint32_t> &BOARD_DIMS>
V3D<uint32_t> Animator<BOARD_DIMS>::noFillInterpolate(const AnimationFrame &keyFrame, V3D<uint32_t> position){
    V3D<uint32_t> returnColor{};
    for(Cell cell : keyFrame.frame){
        if(keyFrame2BoardCoords(cell.position) == position){
            returnColor = cell.color;
        }
    }

    return returnColor;
}

template <const V3D<uint32_t> &BOARD_DIMS>
V3D<uint32_t> Animator<BOARD_DIMS>::closestColorInterpolate(const AnimationFrame &keyFrame, V3D<uint32_t> cubePosition){
    Serial.print("X:" + String(cubePosition.x) + ",Y:" + String(cubePosition.y) + ",Z:" + String(cubePosition.z));
    V3D<uint32_t> returnColor{keyFrame.frame[0].color};
    V3D<uint32_t> distance{keyFrame.frame[0].position};
    distance -= cubePosition;
    float closestDistance = distance.magnitude();

    for(Cell cell : keyFrame.frame){
        distance = keyFrame2BoardCoords(cell.position);
        distance -= cubePosition;
        float euclidDistance = distance.magnitude();
        if(euclidDistance < closestDistance){
            returnColor = cell.color;
            closestDistance = euclidDistance;
        }
    }
    return returnColor;
}

template <const V3D<uint32_t> &BOARD_DIMS>
V3D<uint32_t> Animator<BOARD_DIMS>::linearInterpolate(const AnimationFrame &keyFrame, V3D<uint32_t> position){
    V3D<uint32_t> returnColor{};

    for(Cell cell : keyFrame.frame){
        V3D<uint32_t> vectorDistance{keyFrame2BoardCoords(cell.position)};
        vectorDistance -= position;
        float distance = vectorDistance.magnitude();
        if(distance == 0) return cell.color;
        returnColor += cell.color;
        returnColor /= distance;
    }

    returnColor /= keyFrame.frame.size();

    return returnColor;
}

template <const V3D<uint32_t> &BOARD_DIMS>
V3D<uint32_t> Animator<BOARD_DIMS>::squareInterpolate(const AnimationFrame &keyFrame, V3D<uint32_t> position){
    V3D<uint32_t> returnColor{};

    for(Cell cell : keyFrame.frame){
        V3D<uint32_t> vectorDistance{keyFrame2BoardCoords(cell.position)};
        vectorDistance -= position;
        uint32_t distance = static_cast<uint32_t>(vectorDistance.magnitude());
        distance *= distance;
        if(distance == 0) return cell.color;
        returnColor += cell.color;
        returnColor /= distance;
    }

    returnColor /= keyFrame.frame.size();

    return returnColor;
}