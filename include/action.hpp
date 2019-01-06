#pragma once

#include <vector>
#include "detail.hpp"
#include "state.hpp"

#define CHECK_BOUND

/**
 * Action on a board is defined as up/down/left/right from a specific point in the board.
 * Invalid moves should be handled appropriately.
 */

namespace pad {

Board initialize(const std::string& board_string) {
    Board b;
    // Iterate through board_string
    auto i = 0;
    for(auto& b_ : b) {
        for (auto& o : b_) {
            o = detail::get_value(consts::CHAR_TO_ORB, board_string[i++]);
        }
    }
    return b;
}

// Simply changes the coords, DOES NOT PERFORM ERROR CHECKING!
Coord change_coords(const Coord& coord, Action action_enum) noexcept {
    // Do a quick check on the coordinate being out of bounds:
    auto action = detail::enum_value(action_enum);
    auto direction = (-1 + 2*(action & 0x1));
    auto orient = !(action & 0x2);
    Coord new_coord = { coord.first + (orient * direction),
                        coord.second + (!orient * direction) };
    return new_coord;
}

// Exceptions are expensive, so just return the following:
// 0 - no errors
// 1 - row out of bound
// 2 - column out of bound
inline int check_move(const Coord& coord) noexcept {
    if (coord.first < 0 || coord.first >= consts::NUM_ROWS) 
        return 1;

    if (coord.second < 0 || coord.second >= consts::NUM_COLS) 
        return 2;

    return 0;
}

// Assumption: The new_coord is checked to be valid
Board move(const Board& board, const Coord& old_coord, const Coord& new_coord) {
#ifdef CHECK_BOUND
    int check = check_move(new_coord);
    if (check) 
        throw std::logic_error("CHECK_BOUND detected out of bounds coordinate at move().");
#endif 
    Board new_board = board;
    std::swap(new_board[old_coord.first][old_coord.second], new_board[new_coord.first][new_coord.second]);
    return new_board;
}

// Main driver for populating the next moves.
// Generates 2-4 moves (2 if it's in a corner, 3 at an edge, 4 elsewhere) in a vector.
std::vector<Board> populate(const Board& board, const Coord& coord) {
    std::vector<Board> v;
    for(const Action& action : consts::ACTIONS) {
        Coord new_coord = change_coords(coord, action);
        if (check_move(new_coord) == 0) {
            v.push_back(move(board, coord, new_coord));
        }
    }
    return v;
}

} // namespace pad
