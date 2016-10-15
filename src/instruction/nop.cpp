#include "simulator.hpp"

Simulator::State Simulator::nop(Instruction /* inst */, StateIter state_iter)
{
    auto now_state = *state_iter;
    auto new_state = now_state;

    new_state.pc++;

    return new_state;
}
