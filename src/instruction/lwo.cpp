#include "simulator.hpp"
#include "util.hpp"

Simulator::State Simulator::lwo(Instruction inst)
{
    auto new_state = *m_state_iter;
    new_state.memory_patch = MemoryPatch{};

    auto op = decodeR(inst);
    size_t addr = (op.rs + op.rt) / 4;

    new_state.pc += 4;
    new_state.reg.at(op.rd) = m_memory.at(addr);

    return new_state;
}
