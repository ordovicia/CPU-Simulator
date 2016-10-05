#include <iostream>
#include <sstream>
#include "simulator.hpp"

Simulator::Simulator(const std::string& binfile)
{
    m_binfile.open(binfile, std::ios::binary);
    if (m_binfile.fail()) {
        throw std::runtime_error("File " + binfile + " couldn't be opened");
    }

    m_inst_funcs.emplace(OpCode::NOP, [this](Instruction inst) { nop(inst); });
    m_inst_funcs.emplace(OpCode::ADD, [this](Instruction inst) { add(inst); });
    m_inst_funcs.emplace(OpCode::ADDI, [this](Instruction inst) { addi(inst); });
    m_inst_funcs.emplace(OpCode::SUB, [this](Instruction inst) { sub(inst); });

    m_inst_cnt.emplace(OpCode::NOP, 0);
    m_inst_cnt.emplace(OpCode::ADD, 0);
    m_inst_cnt.emplace(OpCode::ADDI, 0);
    m_inst_cnt.emplace(OpCode::SUB, 0);
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

void Simulator::nop(Instruction) {}

void Simulator::add(Instruction inst)
{
    OperandR op = decodeR(inst);
    m_reg.at(op.rd) = m_reg.at(op.rs) + m_reg.at(op.rt);
    m_pc++;
}

void Simulator::addi(Instruction inst)
{
    OperandI op = decodeI(inst);
    m_reg.at(op.rt) = m_reg.at(op.rs) + op.immediate;
    m_pc++;
}

void Simulator::sub(Instruction inst)
{
    OperandR op = decodeR(inst);
    m_reg.at(op.rd) = m_reg.at(op.rs) - m_reg.at(op.rt);
    m_pc++;
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
