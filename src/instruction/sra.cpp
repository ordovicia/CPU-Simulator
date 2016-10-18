#include "util.hpp"
#include "simulator.hpp"

Simulator::State Simulator::sra(Instruction inst)
{
    auto new_state = *m_state_iter;

    auto op = decodeR(inst);

    new_state.pc += 4;
    new_state.reg.at(op.rd) = signExt5(m_state_iter->reg.at(op.rs)) >> op.shamt;

    return new_state;
}
