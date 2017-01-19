#include "simulator.hpp"
#include "util.hpp"

Simulator::PreState Simulator::cvt_w_s(Instruction inst)
{
    auto op = decodeI(inst);

    auto pre_state = makePreFRegState(op.rt);

    m_freg.at(op.rt) = btof(static_cast<int32_t>(m_freg.at(op.rs)));
    m_pc += 4;

    return pre_state;
}
