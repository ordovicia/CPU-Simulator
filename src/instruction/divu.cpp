#include "simulator.hpp"

Simulator::State Simulator::divu(Instruction inst)
{
    auto new_state = *m_state_iter;

    auto op = decodeR(inst);

    uint32_t rs_unsigned = m_state_iter->reg.at(op.rs);
    uint32_t rt_unsigned = m_state_iter->reg.at(op.rt);

    new_state.pc += 4;
    new_state.reg.at(op.rd) = rs_unsigned / rt_unsigned;
    new_state.hi = rs_unsigned % rt_unsigned;

    return new_state;
}
