#include "simulator.hpp"

Simulator::State Simulator::div(Instruction inst, StateIter state_iter)
{
    auto now_state = *state_iter;
    auto new_state = now_state;

    auto op = decodeR(inst);

    int32_t rs_signed = now_state.reg.at(op.rs);
    int32_t rt_signed = now_state.reg.at(op.rt);

    new_state.pc += 4;
    new_state.reg.at(op.rd) = rs_signed / rt_signed;
    new_state.hi = rs_signed % rt_signed;

    return new_state;
}
