#pragma once

#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <functional>
#include "sized_deque.hpp"
#include "opcode.hpp"

class Simulator
{
public:
    explicit Simulator(const std::string& binfile, int print_step);
    void run();

private:
    std::ifstream m_binfile;
    const int m_print_step;

    bool m_halt = false;
    std::unordered_set<uint32_t> m_breakpoints;

    // Instruction
    using Instruction = uint32_t;  // 32bit Instruction code
    std::vector<Instruction> m_codes;

    int64_t m_dynamic_inst_cnt = 0;

    // State
    static constexpr int REG_SIZE = 32;
    static constexpr int FREG_SIZE = 32;

    struct State {
        uint32_t pc = 0;

        /*
         * General purpose registers
         * R0 is zero register
         */
        std::array<uint32_t, REG_SIZE> reg = {{0}};

        // Floating point registers
        std::array<float, FREG_SIZE> freg = {{0}};

        uint32_t hi = 0;
        uint32_t lo = 0;

        // TODO Memory
    };

    // State history
    static constexpr size_t STATE_HIST_SIZE = 256;
    SizedDeque<State, STATE_HIST_SIZE> m_state_hist;
    using StateIter = typename decltype(m_state_hist)::Iterator;
    StateIter m_state_iter;

    void reset();

    // Operand
    /*
     * Type R:
     * -----------------------------------------
     * | opcode | rs | rt | rd | shamt |(fanct)|
     * -----------------------------------------
     * | 6      | 5  | 5  | 5  | 5     |(6    )|
     * -----------------------------------------
     */
    struct OperandR {
        uint32_t rs;
        uint32_t rt;
        uint32_t rd;
        uint32_t shamt;
        uint32_t fanct;
    };

    /*
     * Type I:
     * -----------------------------------------
     * | opcode | rs | rt | immediate          |
     * -----------------------------------------
     * | 6      | 5  | 5  | 16                 |
     * -----------------------------------------
     */
    struct OperandI {
        uint32_t rs;
        uint32_t rt;
        uint32_t immediate;
    };

    /*
     * Type J:
     * -----------------------------------------
     * | opcode | addr                         |
     * -----------------------------------------
     * | 6      | 26                           |
     * -----------------------------------------
     */
    struct OperandJ {
        uint32_t rs;
        uint32_t addr;
    };

    // Function for each instruction
    std::unordered_map<OpCode,
        std::function<State(Instruction)>, OpCodeHash> m_inst_funcs;

    // Instruction called counter
    std::unordered_map<OpCode, int64_t, OpCodeHash> m_inst_cnt;

    void initInstruction();

    static OpCode decodeOpCode(Instruction);
    State exec(OpCode, Instruction);

    static OperandR decodeR(Instruction);
    static OperandI decodeI(Instruction);
    static OperandJ decodeJ(Instruction);

    static void printBitset(
        uint32_t bits, int begin, int end, bool endl = false);

    struct Screen {
        int width = 0, height = 0;
        int col_num = 0;
        int code_window_len = 0;

        void update();
        void printBoarder(char c, bool p = true) const;
    } m_screen;

    void printState() const;
    void printCode() const;
    void printBreakPoints() const;

    static void printHelp();

#include "instruction.hpp"
};
