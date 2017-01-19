#include "simulator.hpp"
#include "util.hpp"

Simulator::PreState Simulator::bgez(Instruction inst)
{
    auto op = decodeI(inst);

    auto pre_state = makePrePCState(m_pc);

    if (m_reg.at(op.rs) >= 0)
        m_pc += static_cast<int64_t>(signExt(op.immediate, 16)) << 2;
    else
        m_pc += 4;

    return pre_state;
}
