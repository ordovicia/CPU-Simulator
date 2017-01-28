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
            std::cerr << "# Error: Invalid usage. Read 'README.md'" << std::endl;
            return 1;
        }

        int result;
        bool interactive = true, output_memory = false,
             prev_disable = false, disasm = false, quit_run = false;
        int32_t memory_num = 1000000;
        std::string binfile;
        std::string infile;
        std::string outfile = "out.log";

        while ((result = getopt(argc, argv, "rmndqs:f:i:o:")) != -1) {
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
            case 'q':
                quit_run = true;
                break;
            case 's':
                memory_num = std::atoi(optarg);
                if (memory_num < 0) {
                    std::cerr << "# Error: Invalid memory size" << std::endl;
                    return 1;
                }
                break;
            case 'f':
                binfile = optarg;
                break;
            case 'i':
                infile = optarg;
                break;
            case 'o':
                outfile = optarg;
                break;
            case '?':
            default:
                break;
            }
        }

        if (binfile.empty())
            FAIL("# Error: No binfile given");

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

        Simulator sim{binfile, infile, outfile,
            static_cast<size_t>(memory_num),
            interactive, output_memory, prev_disable, quit_run};
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
