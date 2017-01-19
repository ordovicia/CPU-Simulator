#include "simulator.hpp"

Simulator::PreState Simulator::add_s(Instruction inst)
{
    auto op = decodeR(inst);

    auto pre_state = makePreFRegState(op.rd);

    m_freg.at(op.rd) = m_freg.at(op.rs) + m_freg.at(op.rt);
    m_pc += 4;

    return pre_state;
}
