#include "simulator.hpp"

Simulator::State Simulator::nop(Instruction /* inst */)
{
    auto new_state = *m_state_iter;
    new_state.memory_patch = MemoryPatch{};

    new_state.pc += 4;

    return new_state;
}
