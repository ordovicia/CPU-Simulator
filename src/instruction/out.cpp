#include "simulator.hpp"

Simulator::PreState Simulator::out(Instruction inst)
{
    auto op = decodeR(inst);

    auto pre_state = makePrePCState(m_pc);

    m_outfile << static_cast<char>(m_reg.at(op.rs));
    // m_outfile << std::flush; // HALTでflush

    m_pc += 4;

    return pre_state;
}
