#include "simulator.hpp"
#include "util.hpp"

Simulator::PreState Simulator::lwoc1(Instruction inst)
{
    auto op = decodeR(inst);

    auto pre_state = makePreFRegState(op.rd);

    auto addr = (m_reg.at(op.rs) + m_reg.at(op.rt)) / 4;
    if (addr < 0 || addr >= static_cast<int32_t>(MEMORY_NUM))
        FAIL("# Error: Memory index out of range");

    m_freg.at(op.rd) = btof(m_memory.at(addr));
    m_pc += 4;

    return pre_state;
}
