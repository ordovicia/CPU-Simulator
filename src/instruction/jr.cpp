#include "simulator.hpp"

Simulator::PreState Simulator::jr(Instruction inst)
{
    auto op = decodeI(inst);

    auto pre_state = makePrePCState(m_pc);

    m_pc = m_reg.at(op.rs);

    return pre_state;
}
