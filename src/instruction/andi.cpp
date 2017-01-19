#include "simulator.hpp"

Simulator::PreState Simulator::andi(Instruction inst)
{
    auto op = decodeI(inst);

    auto pre_state = makePreGPRegState(op.rt);

    m_reg.at(op.rt) = m_reg.at(op.rs) & op.immediate;
    m_pc += 4;

    return pre_state;
}
