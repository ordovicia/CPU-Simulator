#include "simulator.hpp"

Simulator::State Simulator::sll(Instruction inst, StateIter state_iter)
{
    auto now_state = *state_iter;
    auto new_state = now_state;

    auto op = decodeR(inst);

    new_state.pc += 4;
    new_state.reg.at(op.rd) = now_state.reg.at(op.rs) << op.shamt;

    return new_state;
}
