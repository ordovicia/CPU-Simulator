#include "simulator.hpp"

Simulator::State Simulator::mthi(Instruction inst, StateIter state_iter)
{
    auto now_state = *state_iter;
    auto new_state = now_state;

    auto op = decodeR(inst);

    new_state.pc += 4;
    new_state.hi = now_state.reg.at(op.rs);

    return new_state;
}
