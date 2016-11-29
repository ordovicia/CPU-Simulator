#include <cmath>
#include <sstream>
#include <ncurses.h>
#include "util.hpp"
#include "simulator.hpp"

void Simulator::printBitset(uint32_t bits, int begin, int end, bool endl)
{
    for (int b = begin; b < end; b++) {
        uint32_t bit = (bits << b) >> 31;
        printw("%d", bit);
    }

    if (endl)
        addch('\n');
}

void Simulator::Screen::update()
{
    getmaxyx(stdscr, height, width);
    col_num = (width + 2) / 17;
    code_window_len
        = (height
              - REG_SIZE / col_num - (REG_SIZE % col_num == 0 ? 0 : 1)
              - FREG_SIZE / col_num - (FREG_SIZE % col_num == 0 ? 0 : 1)
              - 12) / 2;
}

void Simulator::Screen::printBoarder(char c, bool p) const
{
    std::ostringstream iss;
    for (int i = 0; i < 14; i++)
        iss << c;

    for (int i = 0; i < col_num; i++) {
        addstr(iss.str().c_str());
        if (i != col_num - 1) {
            if (p)
                addstr(" + ");
            else
                printw("%c%c%c", c, c, c);
        } else
            addch('\n');
    }
}

void Simulator::printState() const
{
    printw("[%s] Static/Dynamic inst cnt = %zu/%d\n",
        m_binfile_name.c_str(), m_codes.size(), m_dynamic_inst_cnt);

    m_screen.printBoarder('=', false);
    auto cn = m_screen.col_num;

    {  // Register
        auto reg = m_state_iter->reg;

        int i;
        for (i = 0; i < REG_SIZE; i++) {
            printw("r%-2d = %08x", i, reg.at(i));
            if (i % cn + 1 == cn)
                addch('\n');
            else
                addstr(" | ");
        }
        if (i % cn != 0) {
            for (; i % cn + 1 != cn; i++)
                addstr("               | ");
            addch('\n');
        }
    }

    m_screen.printBoarder('-');

    {  // Floating point register
        auto freg = m_state_iter->freg;

        int i;
        for (i = 0; i < FREG_SIZE; i++) {
            uint32_t b = ftob(freg.at(i));
            printw("f%-2d = %08x", i, b);
            if (i % cn + 1 == cn)
                addch('\n');
            else
                addstr(" | ");
        }
        if (i % cn != 0) {
            for (; i % cn + 1 != cn; i++)
                addstr("               | ");
            addch('\n');
        }
    }

    m_screen.printBoarder('=', false);
    refresh();
}

void Simulator::printCode() const
{
    auto cwl = m_screen.code_window_len;

    int pc4 = m_state_iter->pc / 4;
    int max_code_idx = std::min(pc4 + cwl, static_cast<int>(m_codes.size()));

    for (int c = pc4 - cwl; c < pc4 + cwl; c++) {
        if (c < 0 or c >= max_code_idx) {
            addstr("       |\n");
            continue;
        }

        if (c == pc4)
            attrset(COLOR_PAIR(0) | A_REVERSE);
        else
            attrset(COLOR_PAIR(0));

        printw("%6d | ", c * 4);
        auto code = m_codes.at(c);
        printBitset(code);

        auto asm_ = disasm(code);
        asm_.resize(m_screen.width - 42);
        addstr(asm_.c_str());
        addch('\n');
    }

    m_screen.printBoarder('=', false);
    refresh();
}

void Simulator::printBreakPoints() const
{
    if (m_breakpoints.size() == 0)
        addstr("No breakpoint");
    for (auto b : m_breakpoints)
        printw("%d, ", b);
    refresh();
}

void Simulator::printHelp()
{
#define PRINT_CMD_DESC(cmd, desc)    \
    attrset(COLOR_PAIR(0) | A_BOLD); \
    addstr(cmd);                     \
    attrset(COLOR_PAIR(0));          \
    addstr(desc);

    PRINT_CMD_DESC("run|r", ": run to the 'halt', ");
    PRINT_CMD_DESC("reset", ": reset\n");
    PRINT_CMD_DESC("(break|b) [int]", ": set breakpoint, ");
    PRINT_CMD_DESC("pb", ": show breakpoints, ");
    PRINT_CMD_DESC("db [int]", ": delete breakpoint\n");
    PRINT_CMD_DESC("pm [int]", ": show memory\n");
    PRINT_CMD_DESC("step|s", ": next instruction, ");
    PRINT_CMD_DESC("prev|p", ": rewind to previous instruction\n");
    PRINT_CMD_DESC("quit|q, help|h\n", "");

    refresh();
}
