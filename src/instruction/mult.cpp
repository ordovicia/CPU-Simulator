#include "util.hpp"
#include "simulator.hpp"

Simulator::State Simulator::mult(Instruction inst)
{
    auto new_state = *m_state_iter;
    new_state.memory_patch = MemoryPatch{};

    auto op = decodeR(inst);

    int64_t rs_s = static_cast<int32_t>(m_state_iter->reg.at(op.rs));
    int64_t rt_s = static_cast<int32_t>(m_state_iter->reg.at(op.rt));
    int64_t acc = rs_s * rt_s;

    new_state.pc += 4;
    new_state.hi = bitset64(acc, 0, 32);
    new_state.lo = bitset64(acc, 32, 64);

    return new_state;
}
