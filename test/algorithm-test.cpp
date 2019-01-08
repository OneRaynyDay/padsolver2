#include <iostream>
#include <set>
#include "catch.hpp"
#include "../include/display.hpp"
#include "../include/algorithm.hpp"

using namespace pad;

TEST_CASE( "Test DFS algorithm.", "[dfs]" ) {
    // Refer to the following link to run the simulation:
    // http://pad.dawnglare.com/?s=EYb3aJ0
    // As you can see, simply taking the heart orb at the bottom and dragging it
    // up, right, and then down will give 10 combos. It is 16 moves which should be
    // less than MAX_DEPTH for dfs.
    static const std::string COMPLICATED_BOARD =
        "brbbrrrgrggrglgllgldlddldhdhhd";
    using namespace dfs;
    SECTION( "running max_combo on this board should yield 10" ) {
        Board b = initialize(COMPLICATED_BOARD);
        REQUIRE(max_combos_possible(b) == 10);
    }
    SECTION( "running distance_from_others spot check" ) {
        {
            Board b = initialize(COMPLICATED_BOARD);
            auto candidates = distance_from_others(b);
            REQUIRE(candidates[0].second == 2);
            REQUIRE(candidates[1].second == 2);
            REQUIRE(candidates[2].second == 1);
        }
        {
            // http://pad.dawnglare.com/?s=TgMt1E0
            static const std::string RANDOM_BOARD =
                "LRHHLRBDGBRDHBLHBGBRRBGHLBBDBL";
            Board b = initialize(RANDOM_BOARD);
            auto candidates = distance_from_others(b);
            REQUIRE(candidates[0].second == 4);
            REQUIRE(candidates[2].second == 1);
            REQUIRE(candidates[3].second == 1);
            REQUIRE(candidates[consts::NUM_ROWS * consts::NUM_COLS - 1].second == 5);
            // After populating:
            auto arr = populate_coords(b);
            std::set<Coord> s(arr.begin(), arr.end());
            REQUIRE(s.find({4, 3}) != s.end());
            REQUIRE(s.find({4, 5}) != s.end());
            REQUIRE(s.find({0, 0}) != s.end());
            REQUIRE(s.find({0, 4}) != s.end());
        }
    }
    SECTION( "running it on the correct coordinate, for a single DFS w/ 1 origin" ) {
        Board b = initialize(COMPLICATED_BOARD);
        SolutionMap map;
        for(int k = 0; k < consts::MAX_COMBOS + 1; k++) {
            map.emplace_back(Coord {4, 1});
        }
        dfs_find(b, {4, 1}, 10, map, 12);
        // Require that we found a 10-combo, since it's possible.
        REQUIRE(map[10].size() != 0);
    }
}

TEST_CASE( "run from all coordinates.", "[dfs]" ) {
    // http://pad.dawnglare.com/?s=DnAuYk0
    static const std::string COMPLICATED_BOARD = 
        "HRHGGHRDGHLBLRRGHHDRBGDRRHRDBR";
    using namespace dfs;
    SECTION( "running max_combo on this board should yield 8" ) {
        Board b = initialize(COMPLICATED_BOARD);
        REQUIRE(max_combos_possible(b) == 8);
    }
    SECTION( "runnning entire DFS" ) {
        SolutionMap map = find_combos(initialize(COMPLICATED_BOARD), 15);
        // Require that we couldnt find a 10-combo, since it's impossible.
        REQUIRE(map[10].size() == 0);
        REQUIRE(map[7].size() != 0);
    }
}

