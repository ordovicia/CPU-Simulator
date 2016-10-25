#include "util.hpp"
#include "simulator.hpp"

Simulator::State Simulator::srav(Instruction inst)
{
    auto new_state = *m_state_iter;
    new_state.memory_patch = MemoryPatch{};

    auto op = decodeR(inst);

    new_state.pc += 4;
    new_state.reg.at(op.rd)
        = signExt5(m_state_iter->reg.at(op.rs))
          >> bitset(m_state_iter->reg.at(op.rt), 28, 32);

    return new_state;
}
