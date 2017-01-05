#include "simulator.hpp"
#include "util.hpp"

Simulator::State Simulator::in(Instruction inst)
{
    auto new_state = *m_state_iter;
    new_state.memory_patch = MemoryPatch{};

    auto op = decodeR(inst);

    auto in_ = m_infile.get();
    if (m_infile.eof())
        FAIL("Input error\n");

    new_state.pc += 4;
    new_state.reg.at(op.rd) = static_cast<int32_t>(in_);

    return new_state;
}
