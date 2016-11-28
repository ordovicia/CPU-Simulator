#include <iostream>
#include <ncurses.h>
#include <getopt.h>
#include "util.hpp"
#include "simulator.hpp"

void endwin_() { endwin(); }

int main(int argc, char** argv)
{
    try {
        if (argc < 2) {
            std::cerr << "Usage: " << argv[0] << " [bin file]" << std::endl;
            return 1;
        }

        int result;
        bool run = false, output_memory = false;
        std::string binfile;
        std::string infile;

        while ((result = getopt(argc, argv, "rmf:i:")) != -1) {
            switch (result) {
            case 'r':
                run = true;
                break;
            case 'm':
                output_memory = true;
                break;
            case 'f':
                binfile = optarg;
                break;
            case 'i':
                infile = optarg;
                break;
            case '?':
            default:
                break;
            }
        }

        if (binfile.empty())
            FAIL("# No binfile given");

        // ncurses setting
        initscr();
        nocbreak();
        echo();
        start_color();
        init_pair(0, COLOR_WHITE, COLOR_BLACK);

        std::atexit(endwin_);

        Simulator sim{binfile, infile, run, output_memory};
        sim.run();

        return 0;
    } catch (const std::exception& e) {
        endwin();
        FAIL(e.what());
    } catch (...) {
        endwin();
        FAIL("# Unknown exception");
    }

    return 1;
}
