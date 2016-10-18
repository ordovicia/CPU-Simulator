#include "simulator.hpp"

Simulator::State Simulator::bltzal(Instruction inst)
{
    auto new_state = *m_state_iter;

    auto op = decodeI(inst);
    int32_t rs = m_state_iter->reg.at(op.rs);

    if (rs < 0) {
        new_state.reg.at(31) = m_state_iter->pc + 4;
        new_state.pc += static_cast<int32_t>(op.immediate << 2);
    }

    return new_state;
}
