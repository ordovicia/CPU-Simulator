#include "simulator.hpp"

Simulator::State Simulator::div(Instruction inst)
{
    auto new_state = *m_state_iter;

    auto op = decodeR(inst);

    auto rs_s = static_cast<int32_t>(m_state_iter->reg.at(op.rs));
    auto rt_s = static_cast<int32_t>(m_state_iter->reg.at(op.rt));

    new_state.pc += 4;
    new_state.lo = rs_s / rt_s;
    new_state.hi = rs_s % rt_s;

    return new_state;
}
