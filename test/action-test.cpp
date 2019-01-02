#include <iostream>
#include "catch.hpp"
#include "../include/action.hpp"

using namespace pad;

TEST_CASE( "Initialize state correctly (from string).", "[state]" ) {
    std::string s;
    for(int i = 0; i < consts::NUM_ROWS; i++){
        for(int j = 0; j < consts::NUM_COLS; j++){
            s.push_back(
                    detail::get_value(consts::ORB_TO_CHAR, Orb(j))
            );
        }
    }
    
    Board b = initialize(s);
    for(int j = 0; j < consts::NUM_COLS; j++){
        for(int i = 0; i < consts::NUM_ROWS; i++){
            REQUIRE( b[i][j] == Orb(j) );
        }
    }
}
