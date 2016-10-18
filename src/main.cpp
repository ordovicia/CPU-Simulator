#include <iostream>
#include <getopt.h>
#include <ncurses.h>
#include "util.hpp"
#include "simulator.hpp"

int main(int argc, char** argv)
{
    try {
        if (argc < 2) {
            std::cerr << "Usage: " << argv[0] << " [bin file]" << std::endl;
            return 1;
        }

        int result;

        std::string binfile;
        int print_step = 1;

        while ((result = getopt(argc, argv, "f:m:")) != -1) {
            switch (result) {
            case 'f':
                binfile = optarg;
                break;
            case 'm':
                print_step = std::atoi(optarg);
                break;
            case '?':
            default:
                break;
            }
        }

        // ncurses setting
        initscr();
        nocbreak();
        echo();
        start_color();
        init_pair(0, COLOR_WHITE, COLOR_BLACK);
        // bkgd(COLOR_PAIR(0));

        Simulator sim{binfile, print_step};
        sim.run();

        endwin();
    } catch (const std::exception& e) {
        FAIL(e.what());
    } catch (...) {
        FAIL("Unknown exception");
    }

    return 0;
}
