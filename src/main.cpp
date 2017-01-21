#include <iostream>
#include <getopt.h>
#include "util.hpp"
#include "simulator.hpp"

bool g_ncurses = false;
void endwin_()
{
    if (g_ncurses)
        endwin();
}

int main(int argc, char** argv)
{
    try {
        if (argc < 2) {
            std::cerr << "Invalid usage. Read 'README.md'" << std::endl;
            return 1;
        }

        int result;
        bool interactive = true, output_memory = false,
             prev_disable = false, disasm = false;
        std::string binfile;
        std::string infile;

        while ((result = getopt(argc, argv, "rmndf:i:")) != -1) {
            switch (result) {
            case 'r':
                interactive = false;
                break;
            case 'm':
                output_memory = true;
                break;
            case 'n':
                prev_disable = true;
                break;
            case 'd':
                disasm = true;
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

        if (not disasm) {
            // ncurses setting
            initscr();
            nocbreak();
            echo();
            start_color();
            init_pair(0, COLOR_WHITE, COLOR_BLACK);
            g_ncurses = true;
            std::atexit(endwin_);
        }

        Simulator sim{binfile, infile, interactive, output_memory, prev_disable};
        if (disasm)
            sim.disasm();
        else
            sim.run();

        return 0;
    } catch (const std::exception& e) {
        FAIL(e.what());
    } catch (...) {
        FAIL("# Error: Unknown exception");
    }

    return 1;
}
