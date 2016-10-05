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
        Instruction inst = fetch();
        OpCode opcode = decodeOpCode(inst);
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

Simulator::OpCode Simulator::decodeOpCode(Instruction inst)
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
        bitset(inst, 21, 26)};
}

Simulator::OperandI Simulator::decodeI(Instruction inst)
{
    return OperandI{
        bitset(inst, 6, 11),
        bitset(inst, 11, 16),
        bitset(inst, 16, 32)};
}
