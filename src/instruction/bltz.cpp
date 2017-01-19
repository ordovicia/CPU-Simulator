#include "simulator.hpp"
#include "util.hpp"

Simulator::PreState Simulator::bltz(Instruction inst)
{
    auto op = decodeI(inst);

    auto pre_state = makePrePCState(m_pc);

    if (m_reg.at(op.rs) < 0)
        m_pc += static_cast<int32_t>(signExt(op.immediate, 16)) << 2;
    else
        m_pc += 4;

    return pre_state;
}
