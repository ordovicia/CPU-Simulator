#include "simulator.hpp"

Simulator::PreState Simulator::nop(Instruction /* inst */)
{
    auto pre_state = makePrePCState(m_pc);
    m_pc += 4;

    return pre_state;
}
