#include "simulator.hpp"

Simulator::PreState Simulator::j(Instruction inst)
{
    auto op = decodeJ(inst);

    auto pre_state = makePrePCState(m_pc);

    m_pc = (m_pc & 0xf0000003) | (op.addr << 2);

    return pre_state;
}
