#include "simulator.hpp"

Simulator::State Simulator::lw(Instruction inst)
{
    auto new_state = *m_state_iter;
    new_state.memory_patch = MemoryPatch{};

    auto op = decodeI(inst);
    auto addr = static_cast<uint32_t>(m_state_iter->reg.at(op.rs)
                                      + static_cast<int32_t>(op.immediate)) / 4;

    new_state.pc += 4;
    new_state.reg.at(op.rt) = m_memory.at(addr);

    return new_state;
}
