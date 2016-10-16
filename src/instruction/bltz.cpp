#include "simulator.hpp"

Simulator::State Simulator::bltz(Instruction inst, StateIter state_iter)
{
    auto now_state = *state_iter;
    auto new_state = now_state;

    auto op = decodeI(inst);
    int32_t rs = now_state.reg.at(op.rs);

    if (rs < 0)
        new_state.pc += static_cast<int32_t>(op.immediate << 2);

    return new_state;
}
