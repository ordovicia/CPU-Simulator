#include "simulator.hpp"

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
