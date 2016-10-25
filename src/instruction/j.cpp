#include "simulator.hpp"

Simulator::State Simulator::j(Instruction inst)
{
    auto new_state = *m_state_iter;
    new_state.memory_patch = MemoryPatch{};

    auto op = decodeJ(inst);

    new_state.pc = op.addr << 2;

    return new_state;
}
