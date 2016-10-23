#include <iostream>
#include <ncurses.h>
#include <getopt.h>
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
        bool run = false;
        std::string binfile;
        while ((result = getopt(argc, argv, "rf:")) != -1) {
            switch (result) {
            case 'r':
                run = true;
                break;
            case 'f':
                binfile = optarg;
                break;
            case '?':
            default:
                break;
            }
        }

        if (binfile.empty()) {
            std::cerr << "No binfile given" << std::endl;
            return 1;
        }

        // ncurses setting
        initscr();
        nocbreak();
        echo();
        start_color();
        init_pair(0, COLOR_WHITE, COLOR_BLACK);
        // bkgd(COLOR_PAIR(0));

        Simulator sim{binfile, run};
        sim.run();

        endwin();
    } catch (const std::exception& e) {
        FAIL(e.what());
    } catch (...) {
        FAIL("Unknown exception");
    }

    return 0;
}
