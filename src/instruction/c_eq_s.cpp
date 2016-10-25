#include "simulator.hpp"
#include "util.hpp"

Simulator::State Simulator::c_eq_s(Instruction inst)
{
    auto new_state = *m_state_iter;
    new_state.memory_patch = MemoryPatch{};

    auto op = decodeR(inst);

    auto b1 = ftob(m_state_iter->freg.at(op.rd));
    auto b2 = ftob(m_state_iter->freg.at(op.rs));

    new_state.pc += 4;
    new_state.fpcond = b1 == b2;

    return new_state;
}
