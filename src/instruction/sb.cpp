#include "simulator.hpp"
#include "util.hpp"

Simulator::State Simulator::sb(Instruction inst)
{
    auto new_state = *m_state_iter;

    auto op = decodeI(inst);
    auto addr = static_cast<uint32_t>(m_state_iter->reg.at(op.rt)
                                      + static_cast<int32_t>(op.immediate));

    new_state.pc += 4;
    auto mem = m_memory.at(addr);
    mem = (mem & 0xffffff00) | bitset(op.rs, 24, 32);
    m_memory.at(addr) = mem;

    return new_state;
}
