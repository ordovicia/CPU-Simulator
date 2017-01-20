#include "simulator.hpp"
#include "util.hpp"

Simulator::PreState Simulator::sw(Instruction inst)
{
    auto op = decodeI(inst);

    auto addr = (m_reg.at(op.rt)
                    + static_cast<int32_t>(signExt(op.immediate, 16))) / 4;
    checkMemoryIndex(addr);

    auto pre_state = makeMemPreState(addr);

    m_memory[addr] = m_reg.at(op.rs);
    m_pc += 4;

    return pre_state;
}
