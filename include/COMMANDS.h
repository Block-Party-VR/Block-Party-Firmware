#pragma once

#include <cstdint>

/**
 * @brief These are the serial commands that the board supports
 */
enum Commands : uint8_t{
  BoardState = 0,
  PING = 1,
  SetStackColors = 2,
  GoToIdle = 3
};