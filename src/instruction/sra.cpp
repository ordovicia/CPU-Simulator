#include "util.hpp"
#include "simulator.hpp"

Simulator::PreState Simulator::sra(Instruction inst)
{
    auto op = decodeR(inst);

    auto pre_state = makePreGPRegState(op.rd);

    m_reg.at(op.rd) = signExt(m_reg.at(op.rs) >> op.shamt, 32 - op.shamt);
    m_pc += 4;

    return pre_state;
}
