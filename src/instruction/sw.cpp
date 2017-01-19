#include "simulator.hpp"
#include "util.hpp"

Simulator::PreState Simulator::sw(Instruction inst)
{
    auto op = decodeI(inst);

    auto addr = (m_reg.at(op.rt)
                    + static_cast<int32_t>(signExt(op.immediate, 16))) / 4;
    auto pre_state = makeMemPreState(addr);

    if (addr < 0 || addr >= static_cast<int32_t>(MEMORY_NUM))
        FAIL("# Error: Memory index out of range");

    m_memory.at(addr) = m_reg.at(op.rs);
    m_pc += 4;

    return pre_state;
}
