#include <exception>
#include "util.hpp"
#include "simulator.hpp"

std::unordered_map<OpCode,
    std::pair<std::string, Simulator::OperandType>> Simulator::m_mnemonic_table;

Simulator::Simulator(
    const std::string& binfile, const std::string& infile,
    bool run, bool output_memory)
    : m_binfile_name(binfile), m_run(run), m_output_memory(output_memory)
{
    initInstruction();
    initDisassembler();

    m_binfile.open(binfile, std::ios::binary);
    if (m_binfile.fail())
        throw std::runtime_error{"File " + binfile + " couldn't be opened"};

    if (not infile.empty()) {
        m_infile.open(infile);
        if (m_infile.fail())
            throw std::runtime_error{"File " + infile + " couldn't be opened"};
    }

    m_outfile.open("out.log", std::ios::out);
    if (m_outfile.fail())
        throw std::runtime_error{"File out.log couldn't be opened for writing"};

    constexpr size_t CODE_RESERVE_SIZE = 65536;
    m_codes.reserve(CODE_RESERVE_SIZE);
    int32_t inst_cnt = 0;
    Instruction r;
    while (m_binfile.read(reinterpret_cast<char*>(&r), sizeof r)) {
        m_codes.emplace_back(r);
        inst_cnt++;
    }
    m_pc_called_cnt.resize(inst_cnt);

    m_state_hist.push(State{});
    m_state_iter = m_state_hist.deque.begin();
}

void Simulator::run()
{
    bool running = false;

    while (true) {
        if (m_run) {
            if (m_dynamic_inst_cnt % STATE_HIST_SIZE == 0) {
                erase();
                printw("running %s\n"
                       "PC = %d, Static/Dynamic inst cnt = %zu/%d\n",
                    m_binfile_name.c_str(), m_state_iter->pc,
                    m_codes.size(), m_dynamic_inst_cnt);
                refresh();
            }
        } else if (not running or m_halt) {
            // Console output
            erase();
            m_screen.update();
            printState();
            printCode();

            // Input command
            addstr(">> ");
            refresh();

            char input[16];
            getnstr(input, 16);

            if (streq(input, "run") or streq(input, "r")) {
                running = true;
            } else if (streq(input, "reset")) {
                reset();
                running = false;
                continue;
            } else if (streqn(input, "break", 5)) {
                int b;
                sscanf(input + 5, "%d", &b);
                m_breakpoints.insert(b);

                continue;
            } else if (streqn(input, "b", 1)) {
                int b;
                sscanf(input + 1, "%d", &b);
                m_breakpoints.insert(b);

                continue;
            } else if (streq(input, "pb")) {
                printBreakPoints();
                getch();
                continue;
            } else if (streqn(input, "db", 2)) {
                int b;
                sscanf(input + 2, "%d", &b);
                m_breakpoints.erase(b);
                continue;
            } else if (streq(input, "step") or streq(input, "s")) {
                // break;
            } else if (streq(input, "prev") or streq(input, "p")) {
                if (m_state_iter == m_state_hist.deque.begin()) {
                    addstr("# Error. Out of saved history");
                    refresh();
                    getch();
                    continue;
                } else {
                    const auto& mp = m_state_iter->memory_patch;
                    if (mp.valid)
                        m_memory.at(mp.idx) = mp.pre_val;

                    m_state_iter--;
                }

                running = false;
                m_halt = false;
                continue;
            } else if (streqn(input, "pm", 2)) {
                size_t idx;
                sscanf(input + 2, "%lu", &idx);
                printw("memory[%lu] = %d\n", idx, m_memory.at(idx));
                refresh();
                getch();
                continue;
            } else if (streq(input, "quit") or streq(input, "q")) {
                return;
            } else if (streq(input, "help") or streq(input, "h")) {
                printHelp();
                getch();
                continue;
            } else {
                continue;
            }
        }

        if (not m_halt) {  // next instruction
            try {
                auto pc = m_state_iter->pc / 4;
                Instruction inst = m_codes.at(pc);  // fetch
                auto opcode = decodeOpCode(inst);
                auto new_state = exec(opcode, inst);

                if (m_state_iter == std::prev(m_state_hist.deque.end())) {
                    m_state_iter = m_state_hist.push(new_state);
                    m_pc_called_cnt.at(pc)++;
                    m_dynamic_inst_cnt++;
                } else {
                    m_state_iter++;
                }

                if (m_breakpoints.find(m_state_iter->pc) != m_breakpoints.end())
                    running = false;
            } catch (std::out_of_range e) {
                FAIL("# Program counter out of range\n" << e.what());
            }
        } else if (m_run)
            return;
    }
}

void Simulator::reset()
{
    m_halt = false;
    m_dynamic_inst_cnt = 0;
    m_breakpoints.clear();
    m_state_hist.deque.clear();
    m_state_hist.push(State{});
    m_state_iter = m_state_hist.deque.begin();

    for (auto& p : m_inst_cnt)
        p.second = 0;
}

OpCode Simulator::decodeOpCode(Instruction inst)
{
    return static_cast<OpCode>(bitset(inst, 0, 6));
}

Simulator::State Simulator::exec(OpCode opcode, Instruction inst)
{
    try {
        m_inst_cnt.at(opcode)++;
        return (m_inst_funcs.at(opcode))(inst);
    } catch (std::out_of_range e) {
        FAIL("# Invalid instruction code\n" << e.what());
    }
}

Simulator::OperandR Simulator::decodeR(Instruction inst)
{
    return OperandR{
        bitset(inst, 6, 11),
        bitset(inst, 11, 16),
        bitset(inst, 16, 21),
        bitset(inst, 21, 26),
        bitset(inst, 26, 32)};
}

Simulator::OperandI Simulator::decodeI(Instruction inst)
{
    return OperandI{
        bitset(inst, 6, 11),
        bitset(inst, 11, 16),
        bitset(inst, 16, 32)};
}

Simulator::OperandJ Simulator::decodeJ(Instruction inst)
{
    return OperandJ{
        bitset(inst, 6, 11),
        bitset(inst, 11, 32)};
}
