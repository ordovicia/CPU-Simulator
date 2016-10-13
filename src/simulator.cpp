#include <iostream>
#include <sstream>
#include "simulator.hpp"

Simulator::Simulator(const std::string& binfile)
{
    m_binfile.open(binfile, std::ios::binary);
    if (m_binfile.fail()) {
        throw std::runtime_error("File " + binfile + " couldn't be opened");
    }

    initInstruction();

    m_state_hist.emplace_front();
    m_state_hist_iter = m_state_hist.begin();
}

void Simulator::run()
{
    while (not m_binfile.eof()) {
        printState(m_state_hist_iter);

        Instruction inst = fetch();
        std::cout << "[instruction] ";
        printBitset(inst, 0, 32);

        auto opcode = decodeOpCode(inst);
        std::cout << " || ";
        printBitset(inst, 0, 6);

        auto new_state = exec(opcode, inst, m_state_hist_iter);
        m_state_hist.insert(m_state_hist.end(), new_state);
        m_state_hist_iter++;

        m_dynamic_inst_cnt++;
    }
}

Simulator::Instruction Simulator::fetch()
{
    Instruction r;
    m_binfile.read(reinterpret_cast<char*>(&r), sizeof r);
    return r;
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
        std::cout << bit;
    }

    if (endl)
        std::cout << std::endl;
}

void Simulator::printOperandR(const OperandR& op)
{
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
}

void Simulator::printOperandI(const OperandI& op)
{
    std::cout << " | ";
    printBitset(op.rs, 27, 32);
    std::cout << " | ";
    printBitset(op.rt, 27, 32);
    std::cout << " | ";
    printBitset(op.immediate, 16, 32, true);
}

void Simulator::printState(StateIter state_iter)
{
    printf("\e[2J\e[0;0H");

    // Program counter
    std::cout << "PC = " << state_iter->pc << std::endl;

    std::cout << "============== + ============== + "
                 " ============= + ============== + "
                 " ============= + ============== + "
                 " ============= + ==============" << std::endl;
    {  // Register
        auto reg = state_iter->reg;

        for (int i = 0; i < REG_SIZE; i++) {
            std::printf("r%-2d = %08x", i, reg.at(i));
            if (i % 8 == 7)
                std::cout << std::endl;
            else
                std::cout << " | ";
        }
    }

    std::cout << "-------------- + -------------- + "
                 " ------------- + -------------- + "
                 " ------------- + -------------- + "
                 " ------------- + --------------" << std::endl;

    {  // Floating point register
        auto freg = state_iter->freg;

        union FloatBit {
            float f;
            uint32_t b;
        };

        for (int i = 0; i < FREG_SIZE; i++) {
            FloatBit fb{freg.at(i)};
            uint32_t b = fb.b;
            std::printf("f%-2d = %08x", i, b);
            if (i % 8 == 7)
                std::cout << std::endl;
            else
                std::cout << " | ";
        }
    }

    std::cout << "============== + ============== + "
                 " ============= + ============== + "
                 " ============= + ============== + "
                 " ============= + ==============" << std::endl;
}
