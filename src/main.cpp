#include <iostream>
#include <ncurses.h>
#include "simulator.hpp"

int main(int argc, char** argv)
{
    try {
        if (argc < 2) {
            std::cerr << "Usage: " << argv[0] << " [bin file]" << std::endl;
            return 1;
        }

        // ncurses setting
        initscr();
        nocbreak();
        echo();

        Simulator sim{argv[1]};
        sim.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown exception" << std::endl;
    }

    getch();
    endwin();

    return 0;
}
