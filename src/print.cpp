#include <cmath>
#include <iomanip>
#include <sstream>
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
              - REG_NUM / col_num - (REG_NUM % col_num == 0 ? 0 : 1)
              - FREG_NUM / col_num - (FREG_NUM % col_num == 0 ? 0 : 1)
              - 12) / 2;
}

void Simulator::Screen::printBoarder(char c, bool p) const
{
    for (int i = 0; i < col_num; i++) {
        printw("%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
            c, c, c, c, c, c, c, c, c, c, c, c, c, c);
        if (i != col_num - 1) {
            if (p)
                addstr(" + ");
            else
                printw("%c%c%c", c, c, c);
        } else {
            addch('\n');
        }
    }
}

void Simulator::printState() const
{
    {  // status
        std::ostringstream oss;
        oss << '[' << m_binfile_name << "] Static/Dynamic inst cnt = "
            << std::setw(7) << m_codes.size() << '/'
            << std::setw(12) << m_dynamic_inst_cnt << ' ';

        auto len = oss.str().size();

        namespace SC = std::chrono;
        auto end = SC::high_resolution_clock::now();
        auto sec = SC::duration_cast<SC::seconds>(end - m_start_time).count();
        auto min = sec / 60;
        sec -= min * 60;

        auto digits = [](decltype(min) x) {
            if (x <= 0)
                return 1;
            int digits = 0;
            while (x) {
                x /= 10;
                digits++;
            }
            return digits;
        };

        auto min_digits = digits(min);

        int padding = m_screen.width - static_cast<int>(len + min_digits + 4);
        if (padding >= 0) {
            oss << std::string(padding, ' ')
                << min << ':' << std::setw(2) << std::setfill('0') << sec;
        }

        auto str = oss.str();
        str.resize(m_screen.width - 1);
        addstr(str.c_str());
        addch('\n');
    }

    m_screen.printBoarder('=', false);
    auto cn = m_screen.col_num;

    {  // Register
        int i;
        for (i = 0; i < REG_NUM; i++) {
            printw("r%-2d 0x%08x", i, m_reg.at(i));
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
        int i;
        for (i = 0; i < FREG_NUM; i++) {
            uint32_t b = ftou(m_freg.at(i));
            printw("f%-2d 0x%08x", i, b);
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

    int64_t pc_idx = m_pc / 4;
    int64_t max_code_idx
        = std::min(pc_idx + cwl, static_cast<int64_t>(m_codes.size()));

    for (int64_t c = pc_idx - cwl; c < pc_idx + cwl; c++) {
        if (c < 0 or c >= max_code_idx) {
            addstr("          |\n");
            continue;
        }

        if (c == pc_idx)
            attrset(COLOR_PAIR(0) | A_REVERSE);

        printw("%c %7lld | ",
            (m_breakpoints.count(c * 4) != 0 ? 'b' : ' '), c * 4);
        auto code = m_codes.at(c);
        printBitset(code);
        addstr(" | ");

        auto asm_ = disasm(code);
        asm_.resize(m_screen.width - 49);
        addstr(asm_.c_str());
        addch('\n');

        if (c == pc_idx)
            attrset(COLOR_PAIR(0));
    }

    m_screen.printBoarder('=', false);
    refresh();
}

void Simulator::printBreakPoints() const
{
    if (m_breakpoints.size() == 0)
        addstr("No breakpoint");
    for (auto b : m_breakpoints)
        printw("%lld, ", b);
    refresh();
}

void Simulator::printHelp() const
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
    PRINT_CMD_DESC("(step|s) <int>", ": next instruction, ");
    PRINT_CMD_DESC("prev|p", ": rewind to previous instruction\n");
    PRINT_CMD_DESC("log|l", ": dump statistics log, ");
    PRINT_CMD_DESC("quit|q, help|h\n", "");

    refresh();
}
