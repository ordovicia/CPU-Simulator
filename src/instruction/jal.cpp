#include "simulator.hpp"

Simulator::PreState Simulator::jal(Instruction inst)
{
    auto op = decodeJ(inst);

    auto pre_state = makePreGPRegState(31);

    m_reg.at(31) = static_cast<int32_t>(m_pc + 4);
    m_pc = op.addr << 2;

    return pre_state;
}
