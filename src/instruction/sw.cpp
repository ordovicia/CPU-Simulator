#include "simulator.hpp"
#include "util.hpp"

Simulator::State Simulator::sw(Instruction inst)
{
    auto new_state = *m_state_iter;
    new_state.memory_patch = MemoryPatch{};

    auto op = decodeI(inst);
    auto addr = static_cast<uint32_t>(
                    m_state_iter->reg.at(op.rt)
                    + static_cast<int32_t>(signExt(op.immediate, 16))) / 4;

    new_state.pc += 4;
    auto pre_mem = m_memory.at(addr);
    m_memory.at(addr) = m_state_iter->reg.at(op.rs);
    new_state.memory_patch = {true, addr, pre_mem};

    return new_state;
}
