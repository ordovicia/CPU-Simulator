#include "simulator.hpp"
#include "util.hpp"

Simulator::PreState Simulator::in(Instruction inst)
{
    auto op = decodeR(inst);

    auto pre_state = makePrePCState(m_pc);

    if (!m_infile.is_open())
        FAIL("# Error: Input file not opened\n");
    auto in_ = m_infile.get();
    if (m_infile.eof())
        FAIL("# Error: Input file reached EOF\n");

    m_reg.at(op.rd) = static_cast<int32_t>(in_);
    m_pc += 4;

    return pre_state;
}
