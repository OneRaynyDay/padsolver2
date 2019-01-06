#include <iostream>
#include "catch.hpp"
#include "../include/score.hpp"
#include "../include/display.hpp"

using namespace pad;
using detail::ComboMask;

// Refer to following link to run the simulation:
// http://pad.dawnglare.com/?s=VoJBsF0
static const std::string COMPLICATED_BOARD =
    "bhhhdhbhdhhhbhlllhddrbbhrrgggb";

TEST_CASE( "Compute scores (pre-skyfalls) correctly.", "[score]" ) {

    SECTION( "running a simple remove_match on b at valid coordinates" ) {
        {
            Board b = initialize(COMPLICATED_BOARD);
            ComboMask mask = detail::init_mask();
            // Removes the blue column
            remove_match(b, mask, Coord {0, 0});
            REQUIRE((mask[0][0] && mask[1][0] && mask[2][0]));
        }
        {
            Board b = initialize(COMPLICATED_BOARD);
            ComboMask mask = detail::init_mask();
            // Removes the heart cross in the corner
            remove_match(b, mask, Coord {0, 5});
            REQUIRE((mask[0][5] && mask[1][5] && mask[2][5] && mask[3][5]));
        }
        {
            Board b = initialize(COMPLICATED_BOARD);
            ComboMask mask = detail::init_mask();
            // Removes the light row
            remove_match(b, mask, Coord {2, 2});
            REQUIRE((mask[2][2] && mask[2][3] && mask[2][4]));
        }
        {
            Board b = initialize(COMPLICATED_BOARD);
            ComboMask mask = detail::init_mask();
            // Removes the blue column
            remove_match(b, mask, Coord {4, 2});
            REQUIRE((mask[4][2] && mask[4][3] && mask[4][4]));
        }
    }

    SECTION( "remove_match at coordinates neighboring combos of the same color" ) {
        Board b = initialize(COMPLICATED_BOARD);
        ComboMask mask = detail::init_mask();
        mask[0][1] = true;
        mask[0][2] = true;
        mask[0][3] = true;
        // Removes the heart orbs on the right side of the board, with the top row of hearts already comboed.
        remove_match(b, mask, Coord {1, 3});
        REQUIRE((mask[1][3] && mask[1][4] && mask[1][5]));
    }
    
    SECTION( "Run the entire thing through all coordinates" ) {
        Board b = initialize(COMPLICATED_BOARD);
        ComboMask mask = detail::init_mask();
        for(int i = 0; i < consts::NUM_ROWS; i++) {
            for(int j = 0; j < consts::NUM_COLS; j++) {
                remove_match(b, mask, Coord {i, j});
            }
        }
        int matched_orbs = 0;
        for(const auto& _m : mask){
            for(bool b : _m){
                matched_orbs += b;
            }
        }
        // 20 orbs are erased.
        REQUIRE(matched_orbs == 20);
        // There are 4 combos.
        REQUIRE(clear_combos(b, mask) == 4);
        int empty = 0;
        for(const auto& _b : b){
            for(auto orb : _b){
                empty += (orb == Orb::empty);
            }
        }
        REQUIRE(empty == 20);
    }
}

TEST_CASE( "Simulate skyfall", "[score]") {
    Board b = initialize(COMPLICATED_BOARD);
    ComboMask mask = detail::init_mask();
    for(int i = 0; i < consts::NUM_ROWS; i++) {
        for(int j = 0; j < consts::NUM_COLS; j++) {
            remove_match(b, mask, Coord {i, j});
        }
    }
    REQUIRE(clear_combos(b, mask) == 4);
    skyfall(b);
    // Do some spot checking
    for(int i = 0; i < 3; i++){
        REQUIRE( b[i][0] == Orb::empty );
    } 
    REQUIRE( b[3][0] != Orb::empty );
    for(int i = 0; i < 4; i++){
        REQUIRE( b[i][3] == Orb::empty );
    }
    REQUIRE( b[4][3] != Orb::empty );
}

TEST_CASE( "Get total combo of boards", "[score]") {
    Board b = initialize(COMPLICATED_BOARD);
    REQUIRE(score(b) == 7);
}
