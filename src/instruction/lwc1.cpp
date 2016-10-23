#include "simulator.hpp"
#include "util.hpp"

Simulator::State Simulator::lwc1(Instruction inst)
{
    auto new_state = *m_state_iter;

    auto op = decodeI(inst);
    auto addr = static_cast<uint32_t>(m_state_iter->reg.at(op.rs)
                                      + static_cast<int32_t>(op.immediate));

    new_state.pc += 4;
    new_state.freg.at(op.rt) = btof(m_memory.at(addr));

    return new_state;
}
