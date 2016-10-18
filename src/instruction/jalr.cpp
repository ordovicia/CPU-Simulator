#include "simulator.hpp"

Simulator::State Simulator::jalr(Instruction inst)
{
    auto new_state = *m_state_iter;

    auto op = decodeR(inst);

    new_state.reg.at(op.rd) = m_state_iter->pc + 4;
    new_state.pc = m_state_iter->reg.at(op.rs);

    return new_state;
}
