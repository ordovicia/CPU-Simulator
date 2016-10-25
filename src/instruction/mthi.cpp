#include "simulator.hpp"

Simulator::State Simulator::mthi(Instruction inst)
{
    auto new_state = *m_state_iter;
    new_state.memory_patch = MemoryPatch{};

    auto op = decodeR(inst);

    new_state.pc += 4;
    new_state.hi = m_state_iter->reg.at(op.rs);

    return new_state;
}
