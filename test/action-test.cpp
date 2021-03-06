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

TEST_CASE( "Opposites are indeed opposites.", "[opposite_actions]" ) {
    REQUIRE( opposite_actions( Action::up, Action::down ) );
    REQUIRE( !opposite_actions( Action::up, Action::right ) );
    REQUIRE( !opposite_actions( Action::up, Action::left ) );
    REQUIRE( !opposite_actions( Action::up, Action::up ) );
    REQUIRE( opposite_actions( Action::right, Action::left ) );
    REQUIRE( !opposite_actions( Action::right, Action::right ) );
    REQUIRE( !opposite_actions( Action::right, Action::down ) );
    REQUIRE( !opposite_actions( Action::right, Action::up ) );
}

TEST_CASE( "Change coordinates (only on proper coordinates).", "[change_coords]") {
    REQUIRE( change_coords( {0, 0}, Action::down ) == Coord {1, 0} );
    REQUIRE( change_coords( {1, 0}, Action::up ) == Coord {0, 0} );
    REQUIRE( change_coords( {0, 0}, Action::right ) == Coord {0, 1} );
    REQUIRE( change_coords( {0, 1}, Action::left ) == Coord {0, 0} );
}

TEST_CASE( "Check move correctness.", "[check_move]" ) {
    REQUIRE( check_move( {0, 0} ) == 0 );
    REQUIRE( check_move( {-1, 0} ) == 1 );
    REQUIRE( check_move( {0, -1} ) == 2 );
}

TEST_CASE( "Move needs to swap between two orbs.", "[move]" ) {
    std::string s;
    for(int i = 0; i < consts::NUM_ROWS; i++){
        for(int j = 0; j < consts::NUM_COLS; j++){
            s.push_back(
                    detail::get_value(consts::ORB_TO_CHAR, Orb::empty)
            );
        }
    }
    s[0] = detail::get_value(consts::ORB_TO_CHAR, Orb::light);
    s[1] = detail::get_value(consts::ORB_TO_CHAR, Orb::dark);

    Board pre_b = initialize(s);
    REQUIRE(pre_b[0][0] == Orb::light);
    REQUIRE(pre_b[0][1] == Orb::dark);

    Board post_b = move(pre_b, {0, 0}, {0, 1});
    REQUIRE(post_b[0][1] == Orb::light);
    REQUIRE(post_b[0][0] == Orb::dark);
}

TEST_CASE( "Populate will only populate 2 things in corners.", "[populate]" ) {
    std::string s;
    for(int i = 0; i < consts::NUM_ROWS; i++){
        for(int j = 0; j < consts::NUM_COLS; j++){
            s.push_back(
                    detail::get_value(consts::ORB_TO_CHAR, Orb::empty)
            );
        }
    }
    Board b = initialize(s);

    {
        Coord c {consts::NUM_ROWS - 1, consts::NUM_COLS - 1};
        auto v = populate(b, c);
        REQUIRE( v.size() == 2 );
    }

    {
        Coord c {consts::NUM_ROWS / 2, consts::NUM_COLS / 2};
        auto v = populate(b, c);
        REQUIRE( v.size() == 4 );
    }
}
