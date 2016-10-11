#include <iostream>
#include <sstream>
#include "simulator.hpp"

Simulator::Simulator(const std::string& binfile)
{
    m_binfile.open(binfile, std::ios::binary);
    if (m_binfile.fail()) {
        throw std::runtime_error("File " + binfile + " couldn't be opened");
    }

    initInst();
}

void Simulator::run()
{
    while (not m_binfile.eof()) {
        std::cout << std::endl;
        printRegister();

        Instruction inst = fetch();
        std::cout << "[instruction] ";
        printBitset(inst, 0, 32);

        auto opcode = decodeOpCode(inst);
        std::cout << " || ";
        printBitset(inst, 0, 6);

        m_dynamic_inst_cnt++;
        exec(opcode, inst);
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

void Simulator::exec(OpCode opcode, Instruction inst)
{
    m_inst_cnt.at(opcode)++;
    (m_inst_funcs.at(opcode))(inst);
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

void Simulator::printRegister()
{
    std::cout << "[register] ";
    for (int i = 0; i < REG_SIZE; i++) {
        if (i != 0 and i % 8 == 0)
            std::cout << "           ";
        std::printf("r%02d = %08x", i, m_reg.at(i));
        if (i % 8 == 7)
            std::cout << std::endl;
        else
            std::cout << ' ';
    }

    union FloatBit {
        float f;
        uint32_t b;
    };

    for (int i = 0; i < FREG_SIZE; i++) {
        if (i % 8 == 0)
            std::cout << "           ";
        FloatBit fb{m_freg.at(i)};
        uint32_t b = fb.b;
        std::printf("f%02d = %08x", i, b);
        if (i % 8 == 7)
            std::cout << std::endl;
        else
            std::cout << ' ';
    }
}
