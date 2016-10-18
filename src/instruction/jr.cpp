#include "simulator.hpp"

Simulator::State Simulator::jr(Instruction inst)
{
    auto new_state = *m_state_iter;

    auto op = decodeR(inst);

    new_state.pc = m_state_iter->reg.at(op.rs);

    return new_state;
}
