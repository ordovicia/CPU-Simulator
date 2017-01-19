#include <exception>
#include "util.hpp"
#include "simulator.hpp"

Simulator::Simulator(
    const std::string& binfile, const std::string& infile,
    bool interactive, bool output_memory, bool prev_enable)
    : m_binfile_name(binfile),
      m_interactive(interactive),
      m_output_memory(output_memory),
      m_prev_enable(prev_enable)
{
    initDisassembler();

    m_binfile.open(binfile, std::ios::binary);
    if (m_binfile.fail())
        FAIL("File " << binfile << " couldn't be opened");

    if (not infile.empty()) {
        m_infile.open(infile);
        if (m_infile.fail())
            FAIL("File " << infile << " couldn't be opened");
    }

    m_outfile.open("out.log");
    if (m_outfile.fail())
        FAIL("File out.log couldn't be opened for writing");

    constexpr size_t CODE_RESERVE_SIZE = 1 << 12;
    m_codes.reserve(CODE_RESERVE_SIZE);
    Instruction r;
    while (m_binfile.read(reinterpret_cast<char*>(&r), sizeof r)) {
        m_codes.emplace_back(r);
    }
    m_pc_called_cnt.resize(m_codes.size());

    m_state_hist.push(PreState{});
    m_state_hist_iter = m_state_hist.deque.begin();
}

void Simulator::run()
{
    int64_t step_cnt = 0;

    auto printConsole = [this] {
        erase();
        m_screen.update();
        printState();
        printCode();
    };

    while (true) {
        if (not m_interactive) {
            if (m_halt)
                return;
            if (m_dynamic_inst_cnt % (1 << 24) == 0) {
                printConsole();
                refresh();
            }
        } else if (step_cnt > 0) {
            step_cnt--;
        } else if (m_running) {
            if (m_dynamic_inst_cnt % (1 << 24) == 0) {
                printConsole();
                refresh();
            }
        } else {
            printConsole();
            // Input command
            addstr(">> ");
            refresh();

            char input[64];
            getnstr(input, 64);

#define PRINT_ERROR(msg) \
    do {                 \
        addstr(msg);     \
        refresh();       \
        getch();         \
    } while (0)

            if ((streq(input, "run") || streq(input, "r")) && not m_halt) {
                m_running = true;
            } else if (streq(input, "reset")) {
                reset();
                continue;
            } else if (streqn(input, "break", 5)) {
                inputBreakpoint(input + 5);
                continue;
            } else if (streqn(input, "b", 1)) {
                inputBreakpoint(input + 1);
                continue;
            } else if (streq(input, "pb")) {
                printBreakPoints();
                getch();
                continue;
            } else if (streqn(input, "db", 2)) {
                int b;
                if (sscanf(input + 2, "%d", &b) != 1) {
                    PRINT_ERROR("# Error: Invalid breakpoint format");
                } else {
                    m_breakpoints.erase(b);
                }

                continue;
            } else if (streqn(input, "step", 4) && not m_halt) {
                int64_t s = 0;
                if (sscanf(input + 4, "%lld", &s) == 1) {
                    if (s <= 0) {
                        PRINT_ERROR("# Error: Invalid step format");
                    } else {
                        step_cnt = s - 1;
                    }
                }
            } else if (streqn(input, "s", 1) && not m_halt) {
                int64_t s = 0;
                if (sscanf(input + 1, "%lld", &s) == 1) {
                    if (s <= 0) {
                        PRINT_ERROR("# Error: Invalid step format");
                    } else {
                        step_cnt = s - 1;
                    }
                }
            } else if (m_prev_enable
                       && (streq(input, "prev") || streq(input, "p"))) {
                if (m_state_hist_iter == m_state_hist.deque.begin()) {
                    PRINT_ERROR("# Error: Out of saved history");
                    continue;
                } else {
                    const auto& pc = m_state_hist_iter->pc;
                    if (pc.changed)
                        m_pc = pc.preval;

                    const auto& gpreg = m_state_hist_iter->gpreg;
                    if (gpreg.changed)
                        m_reg.at(gpreg.idx) = gpreg.preval;

                    const auto& freg = m_state_hist_iter->freg;
                    if (freg.changed)
                        m_freg.at(freg.idx) = freg.preval;

                    const auto& mem = m_state_hist_iter->mem;
                    if (mem.changed)
                        m_memory.at(mem.idx) = mem.preval;

                    m_state_hist_iter--;
                }

                m_halt = false;
                continue;
            } else if (streqn(input, "pm", 2)) {
                size_t idx;
                if (sscanf(input + 2, "%zu", &idx) == 0)
                    addstr("# Error: Invalid memory index format");
                else
                    printw("memory[%zu] = 0x%x\n", idx, m_memory.at(idx));
                refresh();
                getch();
                continue;
            } else if (streq(input, "log") or streq(input, "l")) {
                dumpLog();
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
            auto pc_idx = m_pc / 4;
            if (pc_idx < 0 || static_cast<int64_t>(m_codes.size()) <= pc_idx)
                FAIL("# Error: Program counter out of range");
            Instruction inst = m_codes[pc_idx];  // fetch
            auto opcode = decodeOpCode(inst);
            auto pre_state = exec(opcode, inst);

            if (m_prev_enable) {
                if (m_state_hist_iter == std::prev(m_state_hist.deque.end())) {
                    m_state_hist_iter = m_state_hist.push(pre_state);
                    m_pc_called_cnt.at(pc_idx)++;
                    m_dynamic_inst_cnt++;
                } else {
                    m_state_hist_iter++;
                }
            } else {
                m_pc_called_cnt.at(pc_idx)++;
                m_dynamic_inst_cnt++;
            }

            auto bp = m_breakpoints.find(m_pc);
            if (bp != m_breakpoints.end()) {
                if (bp->second == 0)  // break
                    m_running = false;
                else
                    bp->second--;
            }
        }
    }
}

void Simulator::disasm()
{
    int64_t c = 0;
    for (const auto& inst : m_codes) {
        printf("%7lld | %s\n", c, disasm(inst).c_str());
        c += 4;
    }
}

void Simulator::inputBreakpoint(char* input)
{
    int b, c;
    switch (sscanf(input, "%d %d", &b, &c)) {
    case 1:
        m_breakpoints[b] = 0;
        break;
    case 2:
        if (c <= 0) {
            PRINT_ERROR("# Error: Invalid breakpoint format");
        } else {
            m_breakpoints[b] = c;
        }
        break;
    default:
        PRINT_ERROR("# Error: Invalid breakpoint format");
    }
}

void Simulator::reset()
{
    m_halt = false;
    m_running = false;

    m_dynamic_inst_cnt = 0;
    for (auto& p : m_inst_cnt)
        p.second = 0;

    m_breakpoints.clear();

    m_state_hist.deque.clear();
    m_state_hist.push(PreState{});
    m_state_hist_iter = m_state_hist.deque.begin();
}

Simulator::PreState Simulator::exec(OpCode opcode, Instruction inst)
{
    m_inst_cnt[opcode]++;
    return execInst(opcode, inst);
}

void Simulator::dumpLog() const
{
    using namespace std;

    addstr("Outputting stat info... ");
    refresh();

    {
        ofstream ofs{"call_cnt.log"};
        ofs << "dynamic inst cnt = " << m_dynamic_inst_cnt << endl;
        ofs << "# PC : called cnt" << endl;
        for (size_t i = 0; i < m_pc_called_cnt.size(); i++)
            ofs << 4 * i << ' ' << m_pc_called_cnt.at(i) << endl;
    }

    {
        ofstream ofs{"instruction.log"};
        ofs << "# inst number : called cnt" << endl;
        for (auto inst : m_inst_cnt)
            ofs << static_cast<uint32_t>(inst.first) << ' '
                << inst.second << endl;
    }

    {
        ofstream ofs{"register.log"};
        ofs << hex;
        ofs << "# General purpose registers" << endl;
        for (auto r : m_reg)
            ofs << "0x" << static_cast<uint32_t>(r) << endl;
        ofs << "# Floating point registers" << endl;
        for (auto f : m_freg)
            ofs << "0x" << ftou(f) << endl;
    }

    if (m_output_memory) {
        ofstream ofs{"memory.log"};
        ofs << hex;
        for (auto m : m_memory)
            ofs << m << endl;
    }

    addstr("done!\n");
    refresh();
}
