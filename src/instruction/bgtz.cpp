#include "simulator.hpp"

Simulator::State Simulator::bgtz(Instruction inst)
{
    auto new_state = *m_state_iter;

    auto op = decodeI(inst);
    int32_t rs = m_state_iter->reg.at(op.rs);

    if (rs > 0)
        new_state.pc += static_cast<int32_t>(op.immediate << 2);

    return new_state;
}
