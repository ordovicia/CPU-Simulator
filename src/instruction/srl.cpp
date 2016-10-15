#include "simulator.hpp"

Simulator::State Simulator::srl(Instruction inst, StateIter state_iter)
{
    auto now_state = *state_iter;
    auto new_state = now_state;

    auto op = decodeR(inst);

    new_state.pc++;
    new_state.reg.at(op.rd) = now_state.reg.at(op.rs) >> op.shamt;

    return new_state;
}
