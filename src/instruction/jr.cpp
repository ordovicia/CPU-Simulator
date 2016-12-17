#include "simulator.hpp"

Simulator::State Simulator::jr(Instruction inst)
{
    auto new_state = *m_state_iter;
    new_state.memory_patch = MemoryPatch{};

    auto op = decodeI(inst);

    new_state.pc = m_state_iter->reg.at(op.rs);

    return new_state;
}
