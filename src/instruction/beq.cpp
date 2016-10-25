#include "simulator.hpp"

Simulator::State Simulator::beq(Instruction inst)
{
    auto new_state = *m_state_iter;
    new_state.memory_patch = MemoryPatch{};

    auto op = decodeI(inst);

    if (m_state_iter->reg.at(op.rs) == m_state_iter->reg.at(op.rt))
        new_state.pc += static_cast<int32_t>(op.immediate << 2);
    else
        new_state.pc += 4;

    return new_state;
}
