#include "simulator.hpp"

Simulator::State Simulator::div(Instruction inst)
{
    auto new_state = *m_state_iter;

    auto op = decodeR(inst);

    int32_t rs_signed = m_state_iter->reg.at(op.rs);
    int32_t rt_signed = m_state_iter->reg.at(op.rt);

    new_state.pc += 4;
    new_state.reg.at(op.rd) = rs_signed / rt_signed;
    new_state.hi = rs_signed % rt_signed;

    return new_state;
}
