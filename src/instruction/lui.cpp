#include "simulator.hpp"

Simulator::State Simulator::lui(Instruction inst)
{
    auto new_state = *m_state_iter;

    auto op = decodeI(inst);

    new_state.pc += 4;
    new_state.reg.at(op.rt) = op.immediate << 16;

    return new_state;
}
