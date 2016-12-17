#include "simulator.hpp"

Simulator::State Simulator::cvt_w_s(Instruction inst)
{
    auto new_state = *m_state_iter;
    new_state.memory_patch = MemoryPatch{};

    auto op = decodeI(inst);

    new_state.pc += 4;
    new_state.reg.at(op.rt)
        = static_cast<int32_t>(m_state_iter->freg.at(op.rs));

    return new_state;
}
