#include "simulator.hpp"
#include "util.hpp"

Simulator::PreState Simulator::swo(Instruction inst)
{
    auto op = decodeR(inst);
    auto addr
        = static_cast<int32_t>(m_reg.at(op.rt) + m_reg.at(op.rd)) / 4;
    checkMemoryIndex(addr);

    auto pre_state = makeMemPreState(addr);

    m_memory[addr] = m_reg.at(op.rs);
    m_pc += 4;

    return pre_state;
}
