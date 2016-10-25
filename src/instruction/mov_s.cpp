#include "simulator.hpp"

Simulator::State Simulator::mov_s(Instruction inst)
{
    auto new_state = *m_state_iter;
    new_state.memory_patch = MemoryPatch{};

    auto op = decodeR(inst);

    new_state.pc += 4;
    new_state.freg.at(op.rd) = m_state_iter->freg.at(op.rs);

    return new_state;
}
