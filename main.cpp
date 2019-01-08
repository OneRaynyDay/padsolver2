#include <iostream>
#include "include/algorithm.hpp"
#include "include/action.hpp"
#include "include/state.hpp"
#include "include/display.hpp"

using namespace pad;

int main(){
    {
        std::string s;
        for(int i = 0; i < consts::NUM_ROWS; i++){
            for(int j = 0; j < consts::NUM_COLS; j++){
                s.push_back(detail::get_value(consts::ORB_TO_CHAR, Orb(i*j % 6)));
            }
        }
        Board b = initialize(s);
        std::cout << display::board_string(b) << std::endl;

        // populating
        {
            Coord c {0, 0};
            std::cout << "Populating with coordinate: (" << c.first << ", " << c.second << ")..." << std::endl;
            auto v = populate(b, c);
            for(const auto& p : v) {
                std::cout << std::endl;
                std::cout << display::board_string(p.first) << std::endl;
            }
        }
        // populating
        {
            Coord c {consts::NUM_ROWS - 1, consts::NUM_COLS - 1};
            std::cout << "Populating with coordinate: (" << c.first << ", " << c.second << ")..." << std::endl;
            auto v = populate(b, c);
            for(const auto& p : v) {
                std::cout << std::endl;
                std::cout << display::board_string(p.first) << std::endl;
            }
        }
    }
    {
        std::string s("HLHBLGGHGRRDRHBGLDDRGRHRLRLRDL");
        auto map = dfs::find_combos(initialize(s), 15);
        std::cout << display::analyze_combos(map) << std::endl; 
    }
    return 0;
}
