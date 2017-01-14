#include "simulator.hpp"
#include "util.hpp"

Simulator::State Simulator::bltzal(Instruction inst)
{
    auto new_state = *m_state_iter;

    auto op = decodeI(inst);
    auto rs = static_cast<int32_t>(m_state_iter->reg.at(op.rs));

    if (rs < 0) {
        new_state.reg.at(31) = static_cast<int32_t>(m_state_iter->pc + 4);
        new_state.pc += static_cast<int64_t>(signExt(op.immediate, 16)) << 2;
    } else
        new_state.pc += 4;

    return new_state;
}
