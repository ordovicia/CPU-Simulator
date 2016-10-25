#include "simulator.hpp"

Simulator::State Simulator::ori(Instruction inst)
{
    auto new_state = *m_state_iter;
    new_state.memory_patch = MemoryPatch{};

    auto op = decodeI(inst);

    new_state.pc += 4;
    new_state.reg.at(op.rt) = m_state_iter->reg.at(op.rs) | op.immediate;

    return new_state;
}
