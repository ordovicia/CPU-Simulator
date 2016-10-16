#include "simulator.hpp"

Simulator::State Simulator::divu(Instruction inst, StateIter state_iter)
{
    auto now_state = *state_iter;
    auto new_state = now_state;

    auto op = decodeR(inst);

    uint32_t rs_unsigned = now_state.reg.at(op.rs);
    uint32_t rt_unsigned = now_state.reg.at(op.rt);

    new_state.pc += 4;
    new_state.reg.at(op.rd) = rs_unsigned / rt_unsigned;
    new_state.hi = rs_unsigned % rt_unsigned;

    return new_state;
}
