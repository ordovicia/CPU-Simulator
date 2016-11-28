#include "simulator.hpp"

Simulator::State Simulator::out(Instruction inst)
{
    auto new_state = *m_state_iter;
    new_state.memory_patch = MemoryPatch{};

    auto op = decodeR(inst);
    m_outfile << static_cast<char>(m_state_iter->reg.at(op.rs));

    new_state.pc += 4;
    return new_state;
}
