#include "simulator.hpp"

Simulator::State Simulator::mflo(Instruction inst)
{
    auto new_state = *m_state_iter;

    auto op = decodeR(inst);

    new_state.pc += 4;
    new_state.reg.at(op.rd) = m_state_iter->lo;

    return new_state;
}
