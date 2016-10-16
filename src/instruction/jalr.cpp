#include "simulator.hpp"

Simulator::State Simulator::jalr(Instruction inst, StateIter state_iter)
{
    auto now_state = *state_iter;
    auto new_state = now_state;

    auto op = decodeR(inst);

    new_state.reg.at(op.rd) = now_state.pc + 4;
    new_state.pc = now_state.reg.at(op.rs);

    return new_state;
}
