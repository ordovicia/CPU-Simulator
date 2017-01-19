#include "simulator.hpp"
#include "util.hpp"

Simulator::PreState Simulator::cvt_s_w(Instruction inst)
{
    auto op = decodeI(inst);

    auto pre_state = makePreFRegState(op.rt);

    m_freg.at(op.rt) = static_cast<float>(ftob(m_freg.at(op.rs)));
    m_pc += 4;

    return pre_state;
}
