#include "simulator.hpp"

Simulator::State Simulator::sw(Instruction inst)
{
    auto new_state = *m_state_iter;

    auto op = decodeI(inst);
    auto addr = static_cast<uint32_t>(m_state_iter->reg.at(op.rt)
                                      + static_cast<int32_t>(op.immediate));

    new_state.pc += 4;
    m_memory.at(addr) = op.rs;

    return new_state;
}
