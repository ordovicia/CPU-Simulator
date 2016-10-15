#include "simulator.hpp"

Simulator::State Simulator::halt(Instruction /* inst */, StateIter state_iter)
{
    m_halt = true;
    return *state_iter;
}
