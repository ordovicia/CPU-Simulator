#include "simulator.hpp"
#include "util.hpp"

Simulator::PreState Simulator::mfc1(Instruction inst)
{
    auto op = decodeI(inst);

    auto pre_state = makePreGPRegState(op.rt);

    m_reg.at(op.rt) = ftob(m_freg.at(op.rs));
    m_pc += 4;

    return pre_state;
}
