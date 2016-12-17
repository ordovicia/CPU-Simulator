#include "simulator.hpp"
#include "util.hpp"

Simulator::State Simulator::mtc1(Instruction inst)
{
    auto new_state = *m_state_iter;
    new_state.memory_patch = MemoryPatch{};

    auto op = decodeI(inst);

    new_state.pc += 4;
    new_state.freg.at(op.rt) = btof(m_state_iter->reg.at(op.rs));

    return new_state;
}
