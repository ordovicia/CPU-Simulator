#include "simulator.hpp"

Simulator::State Simulator::halt(Instruction /* inst */)
{
    m_halt = true;

    if (m_run) {
        std::ofstream ofs{"register.log"};
        ofs << std::hex;
        for (auto r : m_state_iter->reg)
            ofs << r << ' ';
        std::cout << std::endl;
        for (auto r : m_state_iter->freg)
            ofs << r << ' ';
        std::cout << std::endl;
        ofs.close();
    }

    return *m_state_iter;
}
