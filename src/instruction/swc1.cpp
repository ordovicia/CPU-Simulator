#include "simulator.hpp"
#include "util.hpp"

Simulator::State Simulator::swc1(Instruction inst)
{
    auto new_state = *m_state_iter;
    new_state.memory_patch = MemoryPatch{};

    auto op = decodeI(inst);
    auto addr = (m_state_iter->reg.at(op.rt)
                    + static_cast<int32_t>(signExt(op.immediate, 16))) / 4;

    new_state.pc += 4;
    try {
        auto pre_mem = m_memory.at(addr);
        m_memory.at(addr) = ftob(m_state_iter->freg.at(op.rs));
        new_state.memory_patch = {true, addr, pre_mem};
    } catch (std::out_of_range e) {
        FAIL("# Memory index out of range\n" << e.what());
    }

    return new_state;
}
