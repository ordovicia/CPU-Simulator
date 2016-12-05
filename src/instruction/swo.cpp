#include "simulator.hpp"
#include "util.hpp"

Simulator::State Simulator::swo(Instruction inst)
{
    auto new_state = *m_state_iter;
    new_state.memory_patch = MemoryPatch{};

    auto op = decodeR(inst);
    auto addr
        = static_cast<int32_t>(
              m_state_iter->reg.at(op.rt) + m_state_iter->reg.at(op.rd)) / 4;

    new_state.pc += 4;
    try {
        auto pre_mem = m_memory.at(addr);
        m_memory.at(addr) = m_state_iter->reg.at(op.rs);
        new_state.memory_patch = {true, addr, pre_mem};
    } catch (std::out_of_range e) {
        FAIL("# Memory index out of range\n" << e.what());
    }

    return new_state;
}
