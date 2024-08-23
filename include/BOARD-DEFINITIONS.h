/**
 * @file BOARD-DEFINITIONS.h
 * @brief This file contains the definitions for the board
*/

#pragma once
#include <cstdint>
#include <array>

#include "PINOUT.h"
#include "BoardTypes.h"
#include "Vector3D.h"

// define the physical dimensions of the board
static constexpr uint32_t BOARD_WIDTH{3};
static constexpr uint32_t BOARD_LENGTH{3};
static constexpr uint32_t BOARD_HEIGHT{3};

static constexpr V3D BOARD_DIMENSIONS{BOARD_WIDTH, BOARD_LENGTH, BOARD_HEIGHT};

// define the number of stacks
static constexpr uint32_t NUMBER_STACKS{BOARD_WIDTH * BOARD_LENGTH};

// define the CubeStacks
static BOARD_TYPES::CubeStack stack1{
    .adcPin=STACK1_ADC_PIN, 
    .ledPin=STACK1_LED_PIN
};
static BOARD_TYPES::CubeStack stack2{
    .adcPin=STACK2_ADC_PIN, 
    .ledPin=STACK2_LED_PIN
};
static BOARD_TYPES::CubeStack stack3{
    .adcPin=STACK3_ADC_PIN, 
    .ledPin=STACK3_LED_PIN
};
static BOARD_TYPES::CubeStack stack4{
    .adcPin=STACK4_ADC_PIN, 
    .ledPin=STACK4_LED_PIN
};
static BOARD_TYPES::CubeStack stack5{
    .adcPin=STACK5_ADC_PIN, 
    .ledPin=STACK5_LED_PIN
};
static BOARD_TYPES::CubeStack stack6{
    .adcPin=STACK6_ADC_PIN, 
    .ledPin=STACK6_LED_PIN
};
static BOARD_TYPES::CubeStack stack7{
    .adcPin=STACK7_ADC_PIN, 
    .ledPin=STACK7_LED_PIN
};
static BOARD_TYPES::CubeStack stack8{
    .adcPin=STACK8_ADC_PIN, 
    .ledPin=STACK8_LED_PIN
};
static BOARD_TYPES::CubeStack stack9{
    .adcPin=STACK9_ADC_PIN, 
    .ledPin=STACK9_LED_PIN
};

static std::array<BOARD_TYPES::CubeStack, NUMBER_STACKS> stacks{
    stack1, stack2, stack3, 
    stack4, stack5, stack6, 
    stack7, stack8, stack9
};