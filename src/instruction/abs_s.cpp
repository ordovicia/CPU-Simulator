#include "simulator.hpp"
#include <cmath>

Simulator::PreState Simulator::abs_s(Instruction inst)
{
    auto op = decodeI(inst);

    auto pre_state = makePreFRegState(op.rt);

    m_freg.at(op.rt) = std::abs(m_freg.at(op.rs));
    m_pc += 4;

    return pre_state;
}
