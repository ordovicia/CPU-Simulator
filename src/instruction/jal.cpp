#include "simulator.hpp"

Simulator::State Simulator::jal(Instruction inst)
{
    auto new_state = *m_state_iter;

    auto op = decodeJ(inst);

    new_state.reg.at(31) = m_state_iter->pc + 4;
    new_state.pc = op.addr << 2;

    return new_state;
}
