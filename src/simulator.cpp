#include <cmath>
#include <sstream>
#include <exception>
#include <ncurses.h>
#include "util.hpp"
#include "simulator.hpp"

Simulator::Simulator(const std::string& binfile, bool run) : m_run(run)
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
    bool running = false;

    while (true) {
        if (m_run) {
            if (m_dynamic_inst_cnt % STATE_HIST_SIZE == 0) {
                erase();
                printw("running... "
                       "PC = %d, Dynamic inst cnt = %d\n",
                    m_state_iter->pc, m_dynamic_inst_cnt);
                refresh();
            }
        } else if (not running or m_halt) {
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
                running = true;
            } else if (streq(input, "reset")) {
                reset();
                running = false;
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

                running = false;
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
                    running = false;
            } catch (std::out_of_range e) {
                FAIL("Program counter out of range\n" << e.what());
            }
        } else if (m_run)
            return;
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

    printw("hi  = %08x | lo  = %08x | fpcond     = %d | "
           "dynamic instruction cnt = %d\n",
        m_state_iter->hi, m_state_iter->lo, m_state_iter->fpcond,
        m_dynamic_inst_cnt);

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

        printw("%6d | ", c * 4);
        printBitset(m_codes.at(c), 0, 32, true);

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
    PRINT_CMD_DESC("step|s", ": next instruction, ");
    PRINT_CMD_DESC("prev|p", ": rewind to previous instruction\n");
    PRINT_CMD_DESC("quit|q, help|h\n", "");

    refresh();
}
