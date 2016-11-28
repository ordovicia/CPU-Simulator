#include "simulator.hpp"
#include "util.hpp"

Simulator::State Simulator::lwc1(Instruction inst)
{
    auto new_state = *m_state_iter;
    new_state.memory_patch = MemoryPatch{};

    auto op = decodeI(inst);
    size_t addr = (m_state_iter->reg.at(op.rs)
                      + static_cast<int32_t>(signExt(op.immediate, 16))) / 4;

    new_state.pc += 4;
    new_state.freg.at(op.rt) = btof(static_cast<uint32_t>(m_memory.at(addr)));

    return new_state;
}
