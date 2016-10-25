#include "simulator.hpp"

Simulator::State Simulator::cvt_s_w(Instruction inst)
{
    auto new_state = *m_state_iter;
    new_state.memory_patch = MemoryPatch{};

    auto op = decodeR(inst);

    new_state.pc += 4;
    new_state.freg.at(op.rd) = static_cast<float>(m_state_iter->reg.at(op.rs));

    return new_state;
}
