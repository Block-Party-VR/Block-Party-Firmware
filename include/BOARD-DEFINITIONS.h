/**
 * @file BOARD-DEFINITIONS.h
 * @brief This file contains the definitions for the board
*/

#pragma once

#include "PINOUT.h"
#include "CubeStack.h"

// define the physical dimensions of the board
#define BOARD_WIDTH 3
#define BOARD_LENGTH 3
#define BOARD_HEIGHT 3

// define the number of stacks
#define NUMBER_STACKS BOARD_WIDTH * BOARD_LENGTH

// define the CubeStacks
CubeStack stack1(STACK1_ADC_PIN, STACK1_LED_PIN, BOARD_HEIGHT);
CubeStack stack2(STACK2_ADC_PIN, STACK2_LED_PIN, BOARD_HEIGHT);
CubeStack stack3(STACK3_ADC_PIN, STACK3_LED_PIN, BOARD_HEIGHT);
CubeStack stack4(STACK4_ADC_PIN, STACK4_LED_PIN, BOARD_HEIGHT);
CubeStack stack5(STACK5_ADC_PIN, STACK5_LED_PIN, BOARD_HEIGHT);
CubeStack stack6(STACK6_ADC_PIN, STACK6_LED_PIN, BOARD_HEIGHT);
CubeStack stack7(STACK7_ADC_PIN, STACK7_LED_PIN, BOARD_HEIGHT);
CubeStack stack8(STACK8_ADC_PIN, STACK8_LED_PIN, BOARD_HEIGHT);
CubeStack stack9(STACK9_ADC_PIN, STACK9_LED_PIN, BOARD_HEIGHT);

// define the array of stacks
CubeStack * stacks[] = {
    &stack1, &stack2, &stack3,
    &stack4, &stack5, &stack6,
    &stack7, &stack8, &stack9
};