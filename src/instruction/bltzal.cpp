#include "simulator.hpp"
#include "util.hpp"

Simulator::PreState Simulator::bltzal(Instruction inst)
{
    auto op = decodeI(inst);

    auto pre_state = makePrePCState(m_pc);

    if (m_reg.at(op.rs) < 0) {
        pre_state.gpreg.changed = true;
        pre_state.gpreg.idx = 31;
        pre_state.gpreg.preval = m_reg.at(31);

        m_reg.at(31) = static_cast<int32_t>(m_pc + 4);
        m_pc += static_cast<int64_t>(signExt(op.immediate, 16)) << 2;
    } else
        m_pc += 4;

    return pre_state;
}
