#include "simulator.hpp"

Simulator::State Simulator::beq(Instruction inst, StateIter state_iter)
{
    auto now_state = *state_iter;
    auto new_state = now_state;

    auto op = decodeI(inst);

    if (now_state.reg.at(op.rs) == now_state.reg.at(op.rt))
        new_state.pc += static_cast<int32_t>(op.immediate << 2);

    return new_state;
}
