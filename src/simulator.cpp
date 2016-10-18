#include <cmath>
#include <exception>
#include <ncurses.h>
#include "util.hpp"
#include "simulator.hpp"

Simulator::Simulator(const std::string& binfile, int print_step)
    : m_print_step(print_step)
{
    m_binfile.open(binfile, std::ios::binary);
    if (m_binfile.fail()) {
        throw std::runtime_error{"File " + binfile + " couldn't be opened"};
    }

    initInstruction();

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
        if (m_dynamic_inst_cnt % m_print_step == 0) {
            erase();
            printState();
            printCode();
        }

        // Input command
        if (m_halt or not run) {
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
            } else if (streqn(input, "dp", 2)) {
                int b;
                sscanf(input + 2, "%d", &b);
                m_breakpoints.erase(b);
                continue;
            } else if (streq(input, "step") or streq(input, "s")) {
                // break;
            } else if (streq(input, "prev") or streq(input, "p")) {
                if (m_state_iter == m_state_hist.deque.begin()) {
                    addstr("Out of saved history\n");
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

        if (not m_halt) {
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

    refresh();
}

void Simulator::printState() const
{
    int cwidth, cheight;
    getmaxyx(stdscr, cheight, cwidth);
    (void)cheight;
    bool col8 = (cwidth > 14 * 8 + 3 * 7);

    if (col8)
        addstr("============== + ============== + "
               "============== + ============== + "
               "============== + ============== + "
               "============== + ==============\n");
    else
        addstr("============== + ============== + "
               "============== + ==============\n");

    {  // Register
        auto reg = m_state_iter->reg;

        for (size_t i = 0; i < REG_SIZE; i++) {
            printw("r%-2d = %08x", i, reg.at(i));
            if ((col8 and i % 8 == 7) or (not col8 and i % 4 == 3))
                addch('\n');
            else
                addstr(" | ");
        }
    }

    if (col8)
        addstr("-------------- + -------------- + "
               "-------------- + -------------- + "
               "-------------- + -------------- + "
               "-------------- + --------------\n");
    else
        addstr("-------------- + -------------- + "
               "-------------- + --------------\n");

    {  // Floating point register
        auto freg = m_state_iter->freg;

        union FloatBit {
            float f;
            uint32_t b;
        };

        for (size_t i = 0; i < FREG_SIZE; i++) {
            FloatBit fb{freg.at(i)};
            uint32_t b = fb.b;
            printw("f%-2d = %08x", i, b);
            if ((col8 and i % 8 == 7) or (not col8 and i % 4 == 3))
                addch('\n');
            else
                addstr(" | ");
        }
    }

    if (col8)
        addstr("-------------- + -------------- + "
               "-------------- + -------------- + "
               "-------------- + -------------- + "
               "-------------- + --------------\n");
    else
        addstr("-------------- + -------------- + "
               "-------------- + --------------\n");

    printw("hi  = %08x | lo  = %08x dynamic isnt cnt = %d\n",
        m_state_iter->hi, m_state_iter->lo, m_dynamic_inst_cnt);

    if (col8)
        addstr("============== + ============== + "
               "============== + ============== + "
               "============== + ============== + "
               "============== + ==============\n");
    else
        addstr("============== + ============== + "
               "============== + ==============\n");

    refresh();
}

void Simulator::printCode() const
{
    int cwidth, cheight;
    getmaxyx(stdscr, cheight, cwidth);
    bool col8 = (cwidth > 14 * 8 + 3 * 7);
    int row_width = (cheight - (col8 ? 8 : 16) - 12) / 2;

    int pc4 = m_state_iter->pc / 4;
    int min_code_idx
        = std::min(pc4 + row_width, static_cast<int>(m_codes.size()));

    for (int c = pc4 - row_width; c < pc4 + row_width; c++) {
        if (c < 0 or c >= min_code_idx) {
            addstr("      |\n");
            continue;
        }

        if (c == pc4)
            attrset(COLOR_PAIR(0) | A_REVERSE);

        printw("%5d | ", c * 4);
        printBitset(m_codes.at(c), 0, 32, true);

        attrset(COLOR_PAIR(0));
    }

    if (col8)
        addstr("============================================="
               "============================================="
               "===========================================\n");
    else
        addstr("================================="
               "================================\n");

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
    addstr("run|r: run to the 'halt', "
           "reset: reset\n"
           "(break|b) [int]: set breakpoint, "
           "pb: show breakpoints, dp [int]: delete breakpoint\n"
           "step|s: next instruction, "
           "prev|p: rewind to previous instruction\n"
           "quit|q, help|h\n");
    refresh();
}
