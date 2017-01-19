#include "simulator.hpp"

Simulator::PreState Simulator::j(Instruction inst)
{
    auto op = decodeJ(inst);

    auto pre_state = makePrePCState(m_pc);

    m_pc = op.addr << 2;

    return pre_state;
}
