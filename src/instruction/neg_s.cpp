#include "simulator.hpp"

Simulator::State Simulator::neg_s(Instruction inst)
{
    auto new_state = *m_state_iter;
    new_state.memory_patch = MemoryPatch{};

    auto op = decodeI(inst);

    new_state.pc += 4;
    new_state.freg.at(op.rt) = -m_state_iter->freg.at(op.rs);

    return new_state;
}
