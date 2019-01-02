#pragma once

#include <iostream>
#include "state.hpp"
#include "action.hpp"
#include "detail.hpp"

// A header file for analyzing the contents of a board

namespace pad {
namespace display {

std::string board_string(const Board& b) {
    std::string s; // empty initialization
    for (int i = 0; i < pad::consts::NUM_ROWS; i++) {
        for (int j = 0; j < pad::consts::NUM_COLS; j++) {
            s += pad::detail::get_value(pad::consts::ORB_TO_CHAR, b[i][j]);
            s += ' ';
        }
        s += '\n';
    }
    s.pop_back();
    return s;
}

} // namespace display
} // namespace pad
