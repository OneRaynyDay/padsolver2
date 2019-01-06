#pragma once

#define DEBUG

#ifdef DEBUG
#include <iostream>
#include "display.hpp"
#endif

#include <sstream>
#include "action.hpp"
#include "state.hpp"
#include "score.hpp"

namespace pad {

class Solution {
public:
    Solution(Coord coord) : origin(coord), action() {}
    int size() const {
        return action.size();
    }
    Coord get_origin() const {
        return origin;
    }
    void push_action(const Action& a) {
        action.push_back(a);
    }
    Action pop_action() {
        Action a = action.back();
        action.pop_back();
        return a;
    }
    std::string to_string() const {
        std::ostringstream ss;
        ss << "(" << origin.first << ", " << origin.second << ") : ";
        for(const Action& a : action) {
            ss << detail::get_value(consts::ACTION_TO_CHAR, a);
        }
        return ss.str();
    }
    std::vector<Action> get_all_action() const {
        return action;
    }
private:    
    std::vector<Action> action;
    Coord origin;
};

namespace dfs {

/**
 * Now that we have the mechanics of the game down, and populating next-moves down,
 * the current naive algorithm is just to perform DFS down to some fixed depth, and
 * check the max score.
 *
 * TODO: Implement parallel version
 */

// We restrict the depth to this so it doesn't run forever:
static const int MAX_DEPTH = 10; // 10 moves max

// We want our solutions to be saved in a simple form: # of combos -> actions
using SolutionMap = std::vector<Solution>;

// Copy the board down the call
inline void dfs(const Board& b, const Coord& c, SolutionMap& map, Solution& cur_sol, const Action& prev_action, int depth) {
    // std::cout << depth << std::endl;
    if(depth > MAX_DEPTH)
        return; 

    int cur_score = 0;
    // A depth of 0 should not be able to update any solutions.
    if(depth) {
        Board board_copy(b);
        cur_score = score(board_copy);
        // We found a solution with lower size
        if(map[cur_score].size() == 0 || cur_sol.size() < map[cur_score].size()) {
            map[cur_score] = cur_sol;
        }
    }

    // We cannot get any higher than MAX_COMBOS, so no point in DFS'ing further.
    if(cur_score == consts::MAX_COMBOS)
        return;

    auto next_boards = populate(b, c);
    for(const auto& p : next_boards) {
        const Board& next_b = p.first; 
        const Action& next_a = p.second;
        // if action taken is the opposite as the one previously, we know it's suboptimal, so prune it.
        // this pesky removal turns this into a 3^k problem instead of 4^k.
        if(depth != 0 && opposite_actions(prev_action, next_a))
            continue; // skip this one.

        // We are changing the "cur_sol" and then flipping it back here:
        cur_sol.push_action(next_a);
        dfs(next_b, change_coords(c, next_a), map, cur_sol, next_a, depth+1); 
        cur_sol.pop_action();
    }
}

inline void dfs_find(const Board& b, const Coord& c, SolutionMap& map) {
    Solution s(c); 
    // Action::up here is just a stub.
    dfs(b, c, map, s, Action::up, 0);
}

SolutionMap find_combos(const Board& b) {
    SolutionMap aggregate;
    for(int k = 0; k < consts::MAX_COMBOS + 1; k++) {
        aggregate.emplace_back(Coord {0, 0});
    }

    for(int i = 0; i < consts::NUM_ROWS; i++) {
        for(int j = 0; j < consts::NUM_COLS; j++) {
            SolutionMap map;
            // Fill the map with MAX_COMBOS entries all with origins at i,j
            for(int k = 0; k < consts::MAX_COMBOS + 1; k++) {
                map.emplace_back(Coord {i, j});
            }
            dfs_find(b, {i, j}, map);
            for(int k = 0; k < consts::MAX_COMBOS + 1; k++) {
                // Invalid move, 0 moves is not allowed.
                if(!map[k].size())
                    continue;
                if(map[k].size() < aggregate[k].size() || aggregate[k].size() == 0)
                    aggregate[k] = map[k];
            }
        }
    }
    return aggregate;
}

} // namespace dfs
} // namespace pad
