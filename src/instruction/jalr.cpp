#include "simulator.hpp"

Simulator::PreState Simulator::jalr(Instruction inst)
{
    auto op = decodeI(inst);

    auto pre_state = makePreGPRegState(op.rt);

    m_reg.at(op.rt) = static_cast<int32_t>(m_pc + 4);
    m_pc = m_reg.at(op.rs);

    return pre_state;
}
