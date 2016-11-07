#include "util.hpp"
#include "simulator.hpp"

Simulator::State Simulator::mult(Instruction inst)
{
    auto new_state = *m_state_iter;
    new_state.memory_patch = MemoryPatch{};

    auto op = decodeR(inst);

    auto rs_s = static_cast<int32_t>(m_state_iter->reg.at(op.rs));
    auto rt_s = static_cast<int32_t>(m_state_iter->reg.at(op.rt));

    new_state.pc += 4;
    new_state.reg.at(op.rd) = rs_s * rt_s;

    return new_state;
}
