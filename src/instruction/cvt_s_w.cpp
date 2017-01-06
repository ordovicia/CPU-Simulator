#include "simulator.hpp"
#include "util.hpp"

Simulator::State Simulator::cvt_s_w(Instruction inst)
{
    auto new_state = *m_state_iter;
    new_state.memory_patch = MemoryPatch{};

    auto op = decodeI(inst);

    new_state.pc += 4;
    new_state.freg.at(op.rt)
        = static_cast<float>(ftob(m_state_iter->freg.at(op.rs)));

    return new_state;
}
