#include "simulator.hpp"
#include "util.hpp"

Simulator::State Simulator::lwoc1(Instruction inst)
{
    auto new_state = *m_state_iter;
    new_state.memory_patch = MemoryPatch{};

    auto op = decodeR(inst);
    auto addr = (m_state_iter->reg.at(op.rs) + m_state_iter->reg.at(op.rt)) / 4;

    new_state.pc += 4;
    try {
        new_state.freg.at(op.rd) = btof(m_memory.at(addr));
    } catch (std::out_of_range e) {
        FAIL("# Memory index out of range\n" << e.what());
    }

    return new_state;
}
