#include "simulator.hpp"

Simulator::State Simulator::jalr(Instruction inst)
{
    auto new_state = *m_state_iter;
    new_state.memory_patch = MemoryPatch{};

    auto op = decodeI(inst);

    new_state.reg.at(op.rt) = static_cast<int32_t>(m_state_iter->pc + 4);
    new_state.pc = m_state_iter->reg.at(op.rs);

    return new_state;
}
