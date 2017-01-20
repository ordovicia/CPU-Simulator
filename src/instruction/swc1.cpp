#include "simulator.hpp"
#include "util.hpp"

Simulator::PreState Simulator::swc1(Instruction inst)
{
    auto op = decodeI(inst);
    auto addr = (m_reg.at(op.rt)
                    + static_cast<int32_t>(signExt(op.immediate, 16))) / 4;
    checkMemoryIndex(addr);

    auto pre_state = makeMemPreState(addr);

    m_memory[addr] = ftob(m_freg.at(op.rs));
    m_pc += 4;

    return pre_state;
}
