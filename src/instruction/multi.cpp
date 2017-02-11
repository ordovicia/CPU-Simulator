#include "simulator.hpp"
#include "util.hpp"

Simulator::PreState Simulator::multi(Instruction inst)
{
    auto op = decodeI(inst);

    auto pre_state = makePreGPRegState(op.rt);

    m_muldivop_file << m_reg.at(op.rs) << ' '
                    << static_cast<int32_t>(signExt(op.immediate, 16))
                    << " #multi\n";

    m_reg.at(op.rt) = m_reg.at(op.rs)
                      * static_cast<int32_t>(signExt(op.immediate, 16));
    m_pc += 4;

    return pre_state;
}
