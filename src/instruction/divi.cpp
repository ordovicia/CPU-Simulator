#include "simulator.hpp"
#include "util.hpp"

Simulator::State Simulator::divi(Instruction inst)
{
    auto new_state = *m_state_iter;
    new_state.memory_patch = MemoryPatch{};

    auto op = decodeI(inst);

    new_state.pc += 4;
    new_state.reg.at(op.rt) = m_state_iter->reg.at(op.rs)
                              / static_cast<int32_t>(signExt(op.immediate, 16));

    return new_state;
}
