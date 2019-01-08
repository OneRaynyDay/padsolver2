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

// Retrieves information on how many orbs of each type there are.
inline std::map<Orb, int> get_freq_orbs(const Board& b) {
    std::map<Orb, int> freq;
    for(const auto& _b : b) {
        for(const auto& o : _b) {
            freq[o]++;
        }
    }
    return freq;
}

// IMPORTANT: This is an upper bound, not a tight bound!
// We simply count how many nodes of each color there are and then we return the greatest number
// of combos possible.
//
// This isn't a repeated subroutine so we don't need to worry about performance yet.
//
// Important argument: You cannot have more than MAX_COMBOS/2 of the same color.
// This is because that means there are less than 3*4 orbs of other colors, and thus
// consequent combos of the same color will be connected at least once if you have more than
// 3*6 orbs of the same color.
int max_combos_possible(const Board& b) {
    int max_combos = 0;
    std::map<Orb, int> freq = get_freq_orbs(b);
    for(const auto& p : freq) {
        max_combos += std::min(p.second / consts::MIN_ORB_COMBO, consts::MAX_COMBOS / 2); 
    }

    return max_combos;
}

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
static const int MAX_DEPTH = 15; // 10 moves max

// We want our solutions to be saved in a simple form: # of combos -> actions
using SolutionMap = std::vector<Solution>;

// Only populate 4 starting spots.
static const int NUM_TO_POPULATE = 5;

// This distance is the manhattan distance.
std::vector<std::pair<Coord, int>> distance_from_others(const Board& b) {
    std::map<Orb, std::vector<Coord>> coord_map;
    std::vector<std::pair<Coord, int>> candidates;
    // Scan to get the coordinates
    for(int i = 0; i < consts::NUM_ROWS; i++) {
        for(int j = 0; j < consts::NUM_COLS; j++) {
            auto o = b[i][j];
            coord_map[o].emplace_back(i, j);
        }
    }
    // Process all candidates. 
    for(int i = 0; i < consts::NUM_ROWS; i++) {
        for(int j = 0; j < consts::NUM_COLS; j++) {
            auto o = b[i][j];
            int min_dist = consts::NUM_ROWS * consts::NUM_COLS;
            for(const auto& c : coord_map[o]){
                // Same orb doesn't apply.
                if(c.first == i && c.second == j)
                    continue;

                min_dist = std::min(min_dist, std::abs(i - c.first) + std::abs(j - c.second));
            }
            candidates.emplace_back(Coord {i , j}, min_dist);
        }
    }
    return candidates;
}

/**
 * In order to populate, we need a couple of orbs that will be used as the starting point and we need to
 * pick them in a relatively smart way.
 *
 * Some ideas:
 * 1. Pick orbs that are rly far away from the other orbs
 * 2. Choose orbs that can actually be made into combos
 */
std::array<Coord, consts::NUM_ORBS> populate_coords(const Board& b, int num_to_populate = NUM_TO_POPULATE) {
    std::array<Coord, consts::NUM_ORBS> top;
    auto freq = get_freq_orbs(b);
    auto candidates = distance_from_others(b);
    std::sort(candidates.begin(), candidates.end(), [] (const auto& a, const auto& b) -> bool {
        return a.second > b.second;
    });
    for(int i = 0; i < num_to_populate; i++) {
        top[i] = candidates[i].first;
    }
    return top;
}

// Copy the board down the call
inline void dfs(const Board& b, const Coord& c, const int max_combos, SolutionMap& map, Solution& cur_sol, const Action& prev_action, int depth, int max_depth) {
    if(depth > max_depth)
        return; 

    int cur_score = 0;
    // A depth of 0 should not be able to update any solutions.
    if(depth) {
        Board board_copy(b);
        cur_score = pad::score(board_copy);
        // We found a solution with lower size
        if(map[cur_score].size() == 0 || cur_sol.size() < map[cur_score].size()) {
            map[cur_score] = cur_sol;
        }
    }

    // We cannot get any higher than MAX_COMBOS, so no point in DFS'ing further.
    if(cur_score == max_combos)
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
        dfs(next_b, change_coords(c, next_a), max_combos, map, cur_sol, next_a, depth+1, max_depth); 
        cur_sol.pop_action();
    }
}

inline void dfs_find(const Board& b, const Coord& c, const int max_combos, SolutionMap& map, int max_depth) {
    Solution s(c); 
    // Action::up here is just a stub.
    dfs(b, c, max_combos, map, s, Action::up, 0, max_depth);
}

SolutionMap find_combos(const Board& b, int max_depth = MAX_DEPTH, bool smart_populate = false) {
    int max_combos = max_combos_possible(b);

    SolutionMap aggregate;
    for(int k = 0; k < consts::MAX_COMBOS + 1; k++) {
        aggregate.emplace_back(Coord {0, 0});
    }

    std::vector<Coord> starting_points;
    if(smart_populate) { 
        auto coords = populate_coords(b);
        for(const auto& c : coords) {
            starting_points.push_back(c);
        }
    }
    else {
        for(int i = 0; i < consts::NUM_ROWS; i++) {
            for(int j = 0; j < consts::NUM_COLS; j++) {
                starting_points.emplace_back(i, j);
            }
        }
    }
    // TODO: So far we populate all possible origins, but it is a much better idea
    // to start with a select few, i.e. 4 or 5. This requires a good heuristic.
    for(const Coord& c : starting_points) {
        SolutionMap map;
        // Fill the map with MAX_COMBOS entries all with origins at i,j
        for(int k = 0; k < consts::MAX_COMBOS + 1; k++) {
            map.emplace_back(c);
        }
        dfs_find(b, c, max_combos, map, max_depth);
        for(int k = 0; k < consts::MAX_COMBOS + 1; k++) {
            // Invalid move, 0 moves is not allowed.
            if(!map[k].size())
                continue;
            if(map[k].size() < aggregate[k].size() || aggregate[k].size() == 0)
                aggregate[k] = map[k];
        }
    }
    return aggregate;
}

} // namespace dfs
} // namespace pad
