#include "simulator.hpp"
#include "util.hpp"

Simulator::PreState Simulator::swoc1(Instruction inst)
{
    auto op = decodeR(inst);
    auto addr = (m_reg.at(op.rt) + m_reg.at(op.rd)) / 4;
    auto pre_state = makeMemPreState(addr);

    if (addr < 0 || addr >= static_cast<int32_t>(MEMORY_NUM))
        FAIL("# Error: Memory index out of range");

    m_memory.at(addr) = ftob(m_freg.at(op.rs));
    m_pc += 4;

    return pre_state;
}
