#include "simulator.hpp"

Simulator::State Simulator::srl(Instruction inst)
{
    auto new_state = *m_state_iter;

    auto op = decodeR(inst);

    new_state.pc += 4;
    new_state.reg.at(op.rd) = m_state_iter->reg.at(op.rs) >> op.shamt;

    return new_state;
}
