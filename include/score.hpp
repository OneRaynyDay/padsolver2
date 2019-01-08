#pragma once

#define DEBUG

#ifdef DEBUG
#include <iostream>
#include "display.hpp"
#endif

#include <cassert>
#include <vector>
#include "detail.hpp"
#include "state.hpp"
#include "action.hpp"

namespace pad {

namespace detail {
// In order for us to correctly compute the number of combos, we need to make a mask
// for orbs that are already visited and are identified as a match.
// To do this, it is in our best interest to have a lightweight mask object, shown below:
using ComboMask = std::array< std::array<bool, consts::NUM_COLS>, consts::NUM_ROWS >; 

ComboMask init_mask() {
    detail::ComboMask mask;
    for(int i = 0; i < consts::NUM_ROWS; i++) {
        for(int j = 0; j < consts::NUM_COLS; j++) {
            mask[i][j] = false;
        }
    } 
    return mask;
}
} // namespace detail

// A node matches if and only if it is connected vertically/horizontally to two orbs of its color.
// Returns a vector of coordinates along horizontal or vertical axis.
// IMPORTANT: We also modify the board by replacing the orbs with empties.
void remove_match(Board& b, detail::ComboMask& mask, const Coord& coord) {
    int row = coord.first;
    int col = coord.second;
    const Orb& o = b[row][col];

    // There is no extra match here. It's already been matched.
    if(o == Orb::empty) {
        return;
    }

    // Preliminary runs
    int r = row; 
    for(; r < consts::NUM_ROWS; r++)
        if(b[r][col] != o)
            break;
    
    int c = col;
    for(; c < consts::NUM_COLS; c++)
        if(b[row][c] != o)
            break;

    // Remove the orbs from the board
    if(r - row >= consts::MIN_ORB_COMBO)
        for(int i = row; i < r; i++)
            mask[i][col] = true;
    
    if(c - col >= consts::MIN_ORB_COMBO)
        for(int j = col; j < c; j++)
            mask[row][j] = true;
}

namespace detail {
// IMPORTANT: Assumes that the current coord is not empty.
void make_empty(Board& b, const detail::ComboMask& mask, const Coord& coord) {
    int row = coord.first;
    int col = coord.second;
    auto o = b[row][col];
    assert(o != Orb::empty); 
    // Remove this orb
    b[row][col] = Orb::empty;

    if(check_move( {row, col+1} ) == 0 && mask[row][col+1] && b[row][col+1] == o) {
        make_empty(b, mask, {row, col+1});
    }
    if(check_move( {row, col-1} ) == 0 && mask[row][col-1] && b[row][col-1] == o) {
        make_empty(b, mask, {row, col-1});
    }
    if(check_move( {row+1, col} ) == 0 && mask[row+1][col] && b[row+1][col] == o) {
        make_empty(b, mask, {row+1, col});
    }
    if(check_move( {row-1, col} ) == 0 && mask[row-1][col] && b[row-1][col] == o) {
        make_empty(b, mask, {row-1, col});
    }
}
} // namespace detail

// IMPORTANT: mask is assumed to be computed properly, i.e.
// every element in the mask should be empty after, and is actually
// a part of a combo.
int clear_combos(Board& b, const detail::ComboMask& mask) {
    int combos = 0;
    for(int i = 0; i < consts::NUM_ROWS; i++){
        for(int j = 0; j < consts::NUM_COLS; j++){
            // This orb has already been matched
            // or this is not an orb belonging to a combo.
            if(b[i][j] == Orb::empty || !mask[i][j])
                continue;
            // Perform a DFS restricted to the matched regions.
            detail::make_empty(b, mask, {i, j});
            combos++;
        }
    } 
    return combos;
}

// Skyfall is when the orbs are cleared and consequent orbs above it moves down.
// Since the board is a simple 5x6, we don't need to worry about cache coherence here.
void skyfall(Board& b) {
    for(int col = 0; col < consts::NUM_COLS; col++) {
        // Start from the bottom, and find the first orb available and move it there.
        int current_row = consts::NUM_ROWS - 1;
        for(int row = consts::NUM_ROWS - 1; row >= 0; row--) {
            // while the current orb is empty decrement row until you find one
            // that is not empty.
            if(b[row][col] == Orb::empty)
                continue;
            std::swap(b[row][col], b[current_row--][col]);
        }
    } 
}

/**
 * Scoring the board is actually quite an involved process. We need to simulate it due to the 
 * complexity of the scoring otherwise.
 *
 * For now, we score by the number of combos but future suggestions would be weighted total of
 * combo multipliers(reliant on color).
 */
int score(Board& b) {
    int score = 0;
    int combo;
    do {
        detail::ComboMask mask = detail::init_mask();
        for(int i = 0; i < consts::NUM_ROWS; i++) {
            for(int j = 0; j < consts::NUM_COLS; j++) {
                remove_match(b, mask, Coord {i, j});
            }
        } 
        // std::cout << display::board_string(b) << std::endl;
        combo = clear_combos(b, mask); 
        // std::cout << "and then after clearing combos : \n" << display::board_string(b) << std::endl;
        score += combo;
        skyfall(b);
        // std::cout << "and then after skyfalls : \n" << display::board_string(b) << std::endl;
    } while(combo);
    return score;  
}
} // namespace pad
