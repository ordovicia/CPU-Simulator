#include "simulator.hpp"
#include "util.hpp"

Simulator::State Simulator::lbu(Instruction inst)
{
    auto new_state = *m_state_iter;

    auto op = decodeI(inst);
    auto addr = static_cast<uint32_t>(m_state_iter->reg.at(op.rs)
                                      + static_cast<int32_t>(op.immediate));

    new_state.pc += 4;
    new_state.reg.at(op.rt) = bitset(m_memory.at(addr), 24, 32);

    return new_state;
}
