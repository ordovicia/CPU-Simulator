#include "simulator.hpp"

Simulator::State Simulator::bc1f(Instruction inst)
{
    auto new_state = *m_state_iter;

    auto op = decodeJ(inst);

    if (not m_state_iter->fpcond)
        new_state.pc = op.addr << 2;
    else
        new_state.pc += 4;

    return new_state;
}
