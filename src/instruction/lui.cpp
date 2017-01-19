#include "simulator.hpp"

Simulator::PreState Simulator::lui(Instruction inst)
{
    auto op = decodeI(inst);

    auto pre_state = makePreGPRegState(op.rt);

    m_pc += 4;
    m_reg.at(op.rt) = op.immediate << 16;

    return pre_state;
}
