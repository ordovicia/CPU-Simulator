#include "simulator.hpp"

Simulator::PreState Simulator::srl(Instruction inst)
{
    auto op = decodeR(inst);

    auto pre_state = makePreGPRegState(op.rd);

    m_reg.at(op.rd) = static_cast<int32_t>(
        static_cast<uint32_t>(m_reg.at(op.rs)) >> op.shamt);
    m_pc += 4;

    return pre_state;
}
