#include "simulator.hpp"

Simulator::State Simulator::xori(Instruction inst, StateIter state_iter)
{
    auto now_state = *state_iter;
    auto new_state = now_state;

    auto op = decodeI(inst);

    new_state.pc++;
    new_state.reg.at(op.rt) = now_state.reg.at(op.rs) ^ op.immediate;

    return new_state;
}
