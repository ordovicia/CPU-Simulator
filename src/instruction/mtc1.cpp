#include "simulator.hpp"
#include "util.hpp"

Simulator::PreState Simulator::mtc1(Instruction inst)
{
    auto op = decodeI(inst);

    auto pre_state = makePreFRegState(op.rt);

    m_freg.at(op.rt) = btof(m_reg.at(op.rs));
    m_pc += 4;

    return pre_state;
}
