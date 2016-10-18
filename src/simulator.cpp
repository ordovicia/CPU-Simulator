#include <cmath>
#include <sstream>
#include <exception>
#include <ncurses.h>
#include "util.hpp"
#include "simulator.hpp"

Simulator::Simulator(const std::string& binfile)
{
    m_binfile.open(binfile, std::ios::binary);
    if (m_binfile.fail()) {
        throw std::runtime_error{"File " + binfile + " couldn't be opened"};
    }

    initInstruction();

    constexpr size_t CODE_RESERVE_SIZE = 30000;
    m_codes.reserve(CODE_RESERVE_SIZE);
    while (not m_binfile.eof()) {
        Instruction r;
        m_binfile.read(reinterpret_cast<char*>(&r), sizeof r);
        m_codes.emplace_back(r);
    }

    m_state_hist.push(State{});
    m_state_iter = m_state_hist.deque.begin();
}

void Simulator::run()
{
    bool run = false;

    while (true) {
        if (not run or m_halt) {
            // Console output
            erase();
            m_screen.update();
            printState();
            printCode();

            // Input command
            addstr(">> ");
            refresh();

            char input[16];
            getnstr(input, 16);

            if (streq(input, "run") or streq(input, "r")) {
                run = true;
            } else if (streq(input, "reset")) {
                reset();
                run = false;
                continue;
            } else if (streqn(input, "break", 5)) {
                int b;
                sscanf(input + 5, "%d", &b);
                m_breakpoints.insert(b);

                continue;
            } else if (streqn(input, "b", 1)) {
                int b;
                sscanf(input + 1, "%d", &b);
                m_breakpoints.insert(b);

                continue;
            } else if (streq(input, "pb")) {
                printBreakPoints();
                getch();
                continue;
            } else if (streqn(input, "db", 2)) {
                int b;
                sscanf(input + 2, "%d", &b);
                m_breakpoints.erase(b);
                continue;
            } else if (streq(input, "step") or streq(input, "s")) {
                // break;
            } else if (streq(input, "prev") or streq(input, "p")) {
                if (m_state_iter == m_state_hist.deque.begin()) {
                    addstr("Error: Out of saved history");
                    refresh();
                    getch();
                    continue;
                } else {
                    m_state_iter--;
                }

                run = false;
                m_halt = false;
                continue;
            } else if (streq(input, "quit") or streq(input, "q")) {
                return;
            } else if (streq(input, "help") or streq(input, "h")) {
                printHelp();
                getch();
                continue;
            } else {
                continue;
            }
        }

        if (not m_halt) {  // next instruction
            try {
                Instruction inst = m_codes.at(m_state_iter->pc / 4);  // fetch
                auto opcode = decodeOpCode(inst);
                auto new_state = exec(opcode, inst);

                if (m_state_iter == std::prev(m_state_hist.deque.end())) {
                    m_state_iter = m_state_hist.push(new_state);
                    m_dynamic_inst_cnt++;
                } else {
                    m_state_iter++;
                }

                if (m_breakpoints.find(m_state_iter->pc) != m_breakpoints.end())
                    run = false;
            } catch (std::out_of_range e) {
                FAIL("Program counter out of range\n" << e.what());
            }
        }
    }
}

void Simulator::reset()
{
    m_halt = false;
    m_dynamic_inst_cnt = 0;
    m_breakpoints.clear();
    m_state_hist.deque.clear();
    m_state_hist.push(State{});
    m_state_iter = m_state_hist.deque.begin();

    for (auto& p : m_inst_cnt)
        p.second = 0;
}

OpCode Simulator::decodeOpCode(Instruction inst)
{
    return static_cast<OpCode>(bitset(inst, 0, 6));
}

Simulator::State Simulator::exec(OpCode opcode, Instruction inst)
{
    try {
        m_inst_cnt.at(opcode)++;
        return (m_inst_funcs.at(opcode))(inst);
    } catch (std::out_of_range e) {
        FAIL("Invalid instruction code\n" << e.what());
    }
}

Simulator::OperandR Simulator::decodeR(Instruction inst)
{
    return OperandR{
        bitset(inst, 6, 11),
        bitset(inst, 11, 16),
        bitset(inst, 16, 21),
        bitset(inst, 21, 26),
        bitset(inst, 26, 32)};
}

Simulator::OperandI Simulator::decodeI(Instruction inst)
{
    return OperandI{
        bitset(inst, 6, 11),
        bitset(inst, 11, 16),
        bitset(inst, 16, 32)};
}

Simulator::OperandJ Simulator::decodeJ(Instruction inst)
{
    return OperandJ{
        bitset(inst, 6, 11),
        bitset(inst, 11, 32)};
}

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

    m_screen.printBoarder('-');

    printw("hi  = %08x | lo  = %08x | dynamic instruction cnt = %d\n",
        m_state_iter->hi, m_state_iter->lo, m_dynamic_inst_cnt);

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
            addstr("      |\n");
            continue;
        }

        if (c == pc4)
            attrset(COLOR_PAIR(0) | A_REVERSE);

        printw("%5d | ", c * 4);
        printBitset(m_codes.at(c), 0, 32, true);

        attrset(COLOR_PAIR(0));
    }

    m_screen.printBoarder('=', false);
    refresh();
}

void Simulator::printBreakPoints() const
{
    for (auto b : m_breakpoints) {
        printw("%d, ", b);
    }
    refresh();
}

void Simulator::printHelp()
{
#define ADDSTR_STANDOUT(str)         \
    attrset(COLOR_PAIR(0) | A_BOLD); \
    addstr(str);
#define ADDSTR_NORMAL(str)  \
    attrset(COLOR_PAIR(0)); \
    addstr(str);

    ADDSTR_STANDOUT("run|r");
    ADDSTR_NORMAL(": run to the 'halt', ");
    ADDSTR_STANDOUT("reset");
    ADDSTR_NORMAL(": reset\n");
    ADDSTR_STANDOUT("(break|b) [int]");
    ADDSTR_NORMAL(": set breakpoint, ");
    ADDSTR_STANDOUT("pb");
    ADDSTR_NORMAL(": show breakpoints, ");
    ADDSTR_STANDOUT("db [int]");
    ADDSTR_NORMAL(": delete breakpoint\n");
    ADDSTR_STANDOUT("step|s");
    ADDSTR_NORMAL(": next instruction, ");
    ADDSTR_STANDOUT("prev|p");
    ADDSTR_NORMAL(": rewind to previous instruction\n");
    ADDSTR_STANDOUT("quit|q, help|h\n");
    attrset(COLOR_PAIR(0));

    refresh();
}
