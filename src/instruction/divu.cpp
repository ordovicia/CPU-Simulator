#include "simulator.hpp"

Simulator::State Simulator::divu(Instruction inst)
{
    auto new_state = *m_state_iter;

    auto op = decodeR(inst);

    uint32_t rs_u = m_state_iter->reg.at(op.rs);
    uint32_t rt_u = m_state_iter->reg.at(op.rt);

    new_state.pc += 4;
    new_state.lo = rs_u / rt_u;
    new_state.hi = rs_u % rt_u;

    return new_state;
}
