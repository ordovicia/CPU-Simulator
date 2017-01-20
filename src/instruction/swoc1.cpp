#include "simulator.hpp"
#include "memory_num.hpp"

Simulator::PreState Simulator::swoc1(Instruction inst)
{
    auto op = decodeR(inst);

    auto addr = (m_reg.at(op.rt) + m_reg.at(op.rd)) / 4;
    checkMemoryIndex(addr);

    auto pre_state = makeMemPreState(addr);

    m_memory[addr] = ftob(m_freg.at(op.rs));
    m_pc += 4;

    return pre_state;
}
