#include <cmath>
#include <iostream>
#include <sstream>
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

    m_state_hist.emplace_front();
    m_state_iter = m_state_hist.begin();

    m_codes.reserve(CODE_INITIAL_SIZE);
    while (not m_binfile.eof()) {
        Instruction r;
        m_binfile.read(reinterpret_cast<char*>(&r), sizeof r);
        m_codes.emplace_back(r);
    }
}

void Simulator::run()
{
    bool run = false;

    while (true) {
        erase();
        printState(m_state_iter);
        printCode(m_state_iter);

        // Input command
        if (m_halt or not run) {
            addstr(">> ");
            refresh();

            int k = getch();
            switch (k) {
            case 'r':  // run to the 'halt', or reset
                if (run or m_halt) {
                    m_state_iter = m_state_hist.begin();
                    m_halt = false;
                    run = false;
                    continue;
                }
                run = true;
                break;
            case 's':  // next instruction
                break;
            case 'p':  // prev inst
                if (m_halt)
                    if (m_state_iter != m_state_hist.begin())
                        m_state_iter--;
                if (m_state_iter != m_state_hist.begin())
                    m_state_iter--;

                run = false;
                m_halt = false;
                continue;
            case 'q':  // quit
                return;
            case 'h':  // print help
                printHelp();
                getch();
                continue;
            default:
                continue;
            };
        }

        if (not m_halt) {
            try {
                Instruction inst = m_codes.at(m_state_iter->pc / 4);  // fetch
                auto opcode = decodeOpCode(inst);
                auto new_state = exec(opcode, inst, m_state_iter);
                if (m_state_iter == std::prev(m_state_hist.end())) {
                    m_state_iter
                        = m_state_hist.insert(m_state_hist.end(), new_state);
                } else {
                    m_state_iter++;
                }

                m_dynamic_inst_cnt++;
            } catch (std::out_of_range e) {
                FAIL("Program counter out of range\n" << e.what());
            }
        }
    }
}

OpCode Simulator::decodeOpCode(Instruction inst)
{
    return static_cast<OpCode>(bitset(inst, 0, 6));
}

Simulator::State Simulator::exec(
    OpCode opcode, Instruction inst, StateIter state_iter)
{
    try {
        m_inst_cnt.at(opcode)++;
        return (m_inst_funcs.at(opcode))(inst, state_iter);
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

uint32_t Simulator::bitset(uint32_t inst, int begin, int end)
{
    int len = end - begin;
    inst <<= begin;
    inst >>= (32 - len);
    return inst;
}

uint32_t Simulator::bitset64(uint64_t inst, int begin, int end)
{
    int len = end - begin;
    inst <<= begin;
    inst >>= (64 - len);
    return static_cast<uint32_t>(inst);
}

uint32_t Simulator::signExt5(uint32_t x)
{
    if (x & (1 << 4))
        return 0xffffffffu & x;
    else
        return x;
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

void Simulator::printState(StateIter state_iter)
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
        auto reg = state_iter->reg;

        for (size_t i = 0; i < REG_SIZE; i++) {
            printw("r%-2d = %08x", i, reg.at(i));
            if ((col8 and i % 8 == 7) or (not col8 and i % 4 == 3))
                addch('\n');
            else
                addstr(" | ");
        }

        printw("hi  = %08x | lo  = %08x\n", state_iter->hi, state_iter->lo);
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
        auto freg = state_iter->freg;

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
        addstr("============== + ============== + "
               "============== + ============== + "
               "============== + ============== + "
               "============== + ==============\n");
    else
        addstr("============== + ============== + "
               "============== + ==============\n");

    refresh();
}

void Simulator::printCode(StateIter state)
{
    int cwidth, cheight;
    getmaxyx(stdscr, cheight, cwidth);
    bool col8 = (cwidth > 14 * 8 + 3 * 7);
    int row_width = (cheight - (col8 ? 8 : 16) - 8) / 2;

    int pc4 = state->pc / 4;
    int min_code_idx
        = std::min(pc4 + row_width, static_cast<int>(m_codes.size()));

    for (int c = pc4 - row_width; c < pc4 + row_width; c++) {
        if (c < 0 or c >= min_code_idx) {
            addstr("        |\n");
            continue;
        }

        if (c == pc4)
            printw("> %5d | ", c * 4);
        else
            printw("%7d | ", c * 4);
        printBitset(m_codes.at(c), 0, 32, true);
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

void Simulator::printHelp()
{
    addstr("r: run to the 'halt' or reset, s: next instruction, p: prev inst, "
           "q: quit, h: help\n");
    getch();
    refresh();
}
