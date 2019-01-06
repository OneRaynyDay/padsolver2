#include <iostream>
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
    SECTION( "running it on the correct coordinate, for a single DFS w/ 1 origin" ) {
        Board b = initialize(COMPLICATED_BOARD);
        SolutionMap map;
        for(int k = 0; k < consts::MAX_COMBOS + 1; k++) {
            map.emplace_back(Coord {4, 1});
        }
        dfs_find(b, {4, 1}, map);
        std::cout << display::analyze_combos(map) << std::endl;
    }
}

TEST_CASE( "run from all coordinates.", "[dfs]" ) {
    static const std::string COMPLICATED_BOARD = 
        "HRHGGHRDGHLBLRRGHHDRBGDRRHRDBR";
    using namespace dfs;
    SECTION( "runnning entire DFS" ) {
        SolutionMap map = find_combos(initialize(COMPLICATED_BOARD));
        std::cout << display::analyze_combos(map) << std::endl;
    }
}

