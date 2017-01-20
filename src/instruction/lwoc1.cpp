#include "simulator.hpp"
#include "util.hpp"

Simulator::PreState Simulator::lwoc1(Instruction inst)
{
    auto op = decodeR(inst);

    auto pre_state = makePreFRegState(op.rd);

    auto addr = (m_reg.at(op.rs) + m_reg.at(op.rt)) / 4;
    checkMemoryIndex(addr);

    m_freg.at(op.rd) = btof(m_memory[addr]);
    m_pc += 4;

    return pre_state;
}
