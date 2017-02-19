#include "simulator.hpp"
#include "util.hpp"

Simulator::PreState Simulator::beq(Instruction inst)
{
    auto op = decodeI(inst);

    auto pre_state = makePrePCState(m_pc);

    if (m_reg.at(op.rs) == m_reg.at(op.rt))
        m_pc += signExt(op.immediate, 16) * 4;
    else
        m_pc += 4;

    return pre_state;
}
