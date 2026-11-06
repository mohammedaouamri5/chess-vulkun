#pragma once

#include "Vector.hpp"
#include <array>
#include <iostream>

namespace KIND {
const int8_t KING = 100;
const int8_t QUIN = 15;
const int8_t KNIGHT = 2;
const int8_t RUKE = 5;
const int8_t BESHOP = 3;
const int8_t PAWN = 1;
} // namespace KIND

namespace DIR {
const Vector LEFT = {-1, 0};
const Vector UP = {0, -1};
const Vector DOWN = {0, 1};
const Vector RIGHT = {1, 0};
} // namespace DIR

namespace MOVES {
const std::array<Vector, 4> RUCK = {DIR::LEFT, DIR::UP, DIR::DOWN, DIR::RIGHT};
const std::array<Vector, 4> BESHOP = {
    DIR::DOWN + DIR::LEFT, DIR::DOWN + DIR::RIGHT, DIR::UP + DIR::LEFT,
    DIR::UP + DIR::RIGHT};
const std::array<Vector, 4> KNIGHT = {
    (DIR::UP + DIR::RIGHT * 2), (DIR::UP + DIR::LEFT * 2),
    (DIR::UP * 2 + DIR::RIGHT), (DIR::UP * 2 + DIR::LEFT)};
} // namespace MOVES
enum COLOR { WHITE = 1, BLACK = -1 };
const int8_t NaN = 91;

