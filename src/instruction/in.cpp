#include "simulator.hpp"
#include "util.hpp"

Simulator::PreState Simulator::in(Instruction inst)
{
    auto op = decodeR(inst);

    auto pre_state = makePrePCState(m_pc);

    // #ifndef FELIS_SIM_NO_ASSERT
    if (!m_infile.is_open())
        FAIL("# Error: Input file not opened\n");
    if (m_infile.eof())
        FAIL("# Error: Input file reached EOF\n");
    // #endif

    char in_;
    m_infile.get(in_);
    // m_reg.at(op.rd) = static_cast<int32_t>(in_);
    m_reg.at(op.rd) = (m_reg.at(op.rd) & (~0u << 8)) | in_;

    m_pc += 4;

    return pre_state;
}
