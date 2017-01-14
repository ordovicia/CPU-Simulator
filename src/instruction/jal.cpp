#include "simulator.hpp"

Simulator::State Simulator::jal(Instruction inst)
{
    auto new_state = *m_state_iter;
    new_state.memory_patch = MemoryPatch{};

    auto op = decodeJ(inst);

    new_state.reg.at(31) = static_cast<int32_t>(m_state_iter->pc + 4);
    new_state.pc = op.addr << 2;

    return new_state;
}
