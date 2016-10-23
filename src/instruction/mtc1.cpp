#include "simulator.hpp"
#include "util.hpp"

Simulator::State Simulator::mtc1(Instruction inst)
{
    auto new_state = *m_state_iter;

    auto op = decodeR(inst);

    new_state.pc += 4;
    new_state.freg.at(op.rd) = btof(m_state_iter->reg.at(op.rs));

    return new_state;
}
