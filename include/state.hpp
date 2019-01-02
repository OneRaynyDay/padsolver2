#pragma once
#include <utility>
#include <array>
#include <map>
#include "detail.hpp"

namespace pad {

namespace consts {
} // namespace consts

/**
 * A player will have one of the 4 actions (up/down/left/right)
 * In order to calculate which new position he will be in use the following formula:
 * (action & 0x2) : decide whether to move horizontally or vertically
 * move rows : !(action & 0x2) & (-1 + 2*(action & 0x1))
 * move columns : (action & 0x2) & (-1 + 2*(action & 0x1))
 */
enum class Action : std::uint8_t {
    up = 0, // 00
    down = 1, // 01
    left = 2, // 10
    right = 3, // 11
};

/**
 * The state of the board is dictated by a 5x6 board(most of the time) of orbs consisting of 7 different colors.
 */
enum class Orb : std::uint8_t { 
    light = 0,
    dark = 1,
    red = 2,
    blue = 3,
    green = 4,
    heart = 5,
    empty = 6, // For skyfall implementation.
};

namespace consts {

// A board is always static 5x6:
static const int NUM_ROWS = 5; 
static const int NUM_COLS = 6;

// Need at least MIN_ORB_COMBO to make a combo in any direction.
static const int MIN_ORB_COMBO = 3;

// Because one cannot simply cast from int to enum so easily, we must make a static array to explicitly map
// the actions to their enums.
static const std::array<Action, 4> ACTIONS = {
    Action::up,
    Action::down,
    Action::left,
    Action::right,
};

static const std::map<Orb, char> ORB_TO_CHAR = {
    {Orb::light, 'l'},
    {Orb::dark,  'd'},
    {Orb::red,   'r'},
    {Orb::blue,  'b'},
    {Orb::green, 'g'},
    {Orb::heart, 'h'},
    {Orb::empty, 'e'},
};

static const std::map<char, Orb> CHAR_TO_ORB = {
    {'l',   Orb::light},
    {'d',   Orb::dark},
    {'r',   Orb::red},
    {'g',   Orb::green},
    {'b',   Orb::blue},
    {'h',   Orb::heart},
    {'e',   Orb::empty},
};

} // namespace consts

using Board = std::array< std::array<Orb, consts::NUM_COLS>, consts::NUM_ROWS >;

// A player's cursor will be a 2d tuple of <row, col>
using Coord = std::pair<int, int>;

} // namespace pad
