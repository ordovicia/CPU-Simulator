#include "simulator.hpp"

Simulator::PreState Simulator::sll(Instruction inst)
{
    auto op = decodeR(inst);

    auto pre_state = makePreGPRegState(op.rd);

    m_reg.at(op.rd) = m_reg.at(op.rs) << op.shamt;
    m_pc += 4;

    return pre_state;
}
