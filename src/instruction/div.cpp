#include "simulator.hpp"

Simulator::PreState Simulator::div(Instruction inst)
{
    auto op = decodeR(inst);

    auto pre_state = makePreGPRegState(op.rd);

    m_muldivop_file << m_reg.at(op.rs) << ' '
                    << m_reg.at(op.rt)
                    << " #div\n";

    m_reg.at(op.rd) = m_reg.at(op.rs) / m_reg.at(op.rt);
    m_pc += 4;

    return pre_state;
}
