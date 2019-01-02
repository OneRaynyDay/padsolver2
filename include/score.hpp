#pragma once

#include <vector>
#include "detail.hpp"
#include "state.hpp"
#include "action.hpp"

namespace pad {
// A node matches if and only if it is connected vertically/horizontally to two orbs of its color.
// Returns a vector of coordinates along horizontal or vertical axis.
// IMPORTANT: We assume scanned from the very top, so we only need to go down/rightwards on the board for scanning.
// IMPORTANT: We also modify the board by replacing the orbs with empties.
bool remove_match(Board& b, const Coord& coord) {
    int row = coord.first;
    int col = coord.second;
    const Orb& o = b[row][col];

    // Cannot match "empty" orbs.
    if(o == Orb::empty)
        return false;

    // Preliminary runs
    int r = row; 
    for(; r < consts::NUM_ROWS; r++)
        if(b[r][col] != o)
            break;
    
    int c = col;
    for(; c < consts::NUM_COLS; c++)
        if(b[row][c] != o)
            break;

    // Remove directly adjacent combo-able orbs of the same color (they count as 1 combo)
    bool ver_combo = (r - row >= consts::MIN_ORB_COMBO - 1);
    bool hor_combo = (c - col >= consts::MIN_ORB_COMBO - 1);

    if(ver_combo)
        for(int i = row + 1; i <= r; i++)
            remove_match(b, {i, col});

    if(hor_combo)
        for(int j = col + 1; j <= c; j++)
            remove_match(b, {row, j});        
   
    // Remove the orbs from the board
    if(ver_combo)
        for(int i = row; i <= r; i++)
            b[i][col] = Orb::empty;
    
    if(hor_combo)
        for(int j = col; j <= c; j++)
            b[row][j] = Orb::empty;

    return ver_combo || hor_combo;
}

/**
 * Scoring the board is actually quite an involved process. We need to simulate it due to the 
 * complexity of the scoring otherwise.
 *
 * For now, we score by the number of combos but future suggestions would be weighted total of
 * combo multipliers(reliant on color).
 */
int score(const Board& b) {
    int score = 0;
    // First, find all the orbs(that are not empty) in a single row or column, and do a down/right sweep and check if any direction is > 3.
    // And then, do a bfs on it to find all orbs that may be candidates. (The BFS portion will be explained later)
    
    // BFS: Find neighboring nodes of the same color iff this node matches for every node.
    return score;  
}
} // namespace pad
