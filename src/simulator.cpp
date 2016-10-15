#include <cmath>
#include <iostream>
#include <sstream>
#include <ncurses.h>
#include "simulator.hpp"

#define FAIL(msg)                  \
    endwin();                      \
    std::cerr << msg << std::endl; \
    std::exit(1);

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
            case 'r':  // run to the end
                run = true;
                break;
            case 's':  // next instruction
                break;
            case 'p':  // prev inst
                if (m_halt) {
                    m_halt = false;
                    if (m_state_iter != m_state_hist.begin())
                        m_state_iter--;
                }
                if (m_state_iter != m_state_hist.begin())
                    m_state_iter--;
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
                Instruction inst = m_codes.at(m_state_iter->pc);  // fetch
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

void Simulator::printOperandR(const OperandR& /* op */)
{
    /*
    std::cout << " | ";
    printBitset(op.rs, 27, 32);
    std::cout << " | ";
    printBitset(op.rt, 27, 32);
    std::cout << " | ";
    printBitset(op.rd, 27, 32);
    std::cout << " | ";
    printBitset(op.shamt, 27, 32);
    std::cout << " | ";
    printBitset(op.fanct, 26, 32, true);
    */
}

void Simulator::printOperandI(const OperandI& /* op */)
{
    /*
    std::cout << " | ";
    printBitset(op.rs, 27, 32);
    std::cout << " | ";
    printBitset(op.rt, 27, 32);
    std::cout << " | ";
    printBitset(op.immediate, 16, 32, true);
    */
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
    int row_len = (cheight - (col8 ? 8 : 16) - 6) / 2;

    int pc = state->pc;

    for (int c = pc - row_len; c < pc + row_len; c++) {
        if (c < 0 or c >= pc + row_len) {
            addstr("        |\n");
            continue;
        }

        if (c == pc)
            printw("> %5d | ", c);
        else
            printw("%7d | ", c);
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
    addstr("r: run to the 'halt', s: next instruction, p: prev inst, "
           "q: quit, h: help\n");
    getch();
    refresh();
}
