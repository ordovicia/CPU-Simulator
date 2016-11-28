#include "simulator.hpp"
#include "util.hpp"

Simulator::State Simulator::swo(Instruction inst)
{
    auto new_state = *m_state_iter;
    new_state.memory_patch = MemoryPatch{};

    auto op = decodeR(inst);
    size_t addr = (op.rt + op.rd) / 4;

    new_state.pc += 4;
    auto pre_mem = m_memory.at(addr);
    m_memory.at(addr) = m_state_iter->reg.at(op.rs);
    new_state.memory_patch = {true, addr, pre_mem};

    return new_state;
}
