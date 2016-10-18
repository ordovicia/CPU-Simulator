#include "simulator.hpp"

Simulator::State Simulator::halt(Instruction /* inst */)
{
    m_halt = true;
    return *m_state_iter;
}
