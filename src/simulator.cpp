#include <cmath>
#include <iostream>
#include <sstream>
#include <ncurses.h>
#include "simulator.hpp"

Simulator::Simulator(const std::string& binfile)
{
    initscr();

    m_binfile.open(binfile, std::ios::binary);
    if (m_binfile.fail()) {
        throw std::runtime_error("File " + binfile + " couldn't be opened");
    }

    initInstruction();

    m_state_hist.emplace_front();
    m_state = m_state_hist.begin();

    m_codes.reserve(CODE_INITIAL_SIZE);
    while (not m_binfile.eof()) {
        Instruction r;
        m_binfile.read(reinterpret_cast<char*>(&r), sizeof r);
        m_codes.emplace_back(r);
    }
}

Simulator::~Simulator()
{
    getch();
    endwin();
}

void Simulator::run()
{
    while (not m_finish) {
        erase();
        printState(m_state);
        printCode(m_state);

        Instruction inst = m_codes.at(m_state->pc);  // fetch
        auto opcode = decodeOpCode(inst);
        auto new_state = exec(opcode, inst, m_state);
        m_state_hist.insert(m_state_hist.end(), new_state);
        m_state++;

        m_dynamic_inst_cnt++;
    }
}

OpCode Simulator::decodeOpCode(Instruction inst)
{
    return static_cast<OpCode>(bitset(inst, 0, 6));
}

Simulator::State Simulator::exec(
    OpCode opcode, Instruction inst, StateIter state_iter)
{
    m_inst_cnt.at(opcode)++;
    return (m_inst_funcs.at(opcode))(inst, state_iter);
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

void Simulator::printOperandR(const OperandR& op)
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

void Simulator::printOperandI(const OperandI& op)
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
    addstr(
        "============== + ============== + ============== + ============== + "
        "============== + ============== + ============== + ==============\n");

    {  // Register
        auto reg = state_iter->reg;

        for (size_t i = 0; i < REG_SIZE; i++) {
            printw("r%-2d = %08x", i, reg.at(i));
            if (i % 8 == 7)
                addch('\n');
            else
                addstr(" | ");
        }
    }

    addstr(
        "-------------- + -------------- + -------------- + -------------- + "
        "-------------- + -------------- + -------------- + --------------\n");

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
            if (i % 8 == 7)
                addch('\n');
            else
                addstr(" | ");
        }
    }

    addstr(
        "============== + ============== + ============== + ============== + "
        "============== + ============== + ============== + ==============\n");
    refresh();
}

void Simulator::printCode(StateIter state)
{
    int cwidth, cheight;
    getmaxyx(stdscr, cheight, cwidth);
    (void)cwidth;
    int print_len = (cheight - 11) / 2 - 1;

    int pc = state->pc;

    int begin = std::max(pc - print_len, 0);
    int end = std::min(pc + print_len, static_cast<int>(m_codes.size()));

    for (int c = begin; c < end; c++) {
        if (c == pc)
            printw("> %5d | ", c);
        else
            printw("%7d | ", c);
        printBitset(m_codes.at(c), 0, 32, true);
    }

    addstr(
        "============== + ============== + ============== + ============== + "
        "============== + ============== + ============== + ==============\n");
    refresh();
}
