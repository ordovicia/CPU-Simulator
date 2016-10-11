#pragma once

#include <fstream>
#include <array>
#include <unordered_map>
#include <functional>
#include <exception>
#include "opcode.hpp"

class Simulator
{
public:
    explicit Simulator(const std::string& binfile);
    void run();

private:
    std::ifstream m_binfile;
    int m_pc = 0;
    int64_t m_dynamic_inst_cnt = 0;

    /*
     * General purpose registers
     * R0 is zero register
     */
    static constexpr int REG_SIZE = 32;
    std::array<int32_t, REG_SIZE> m_reg = {{0}};

    // Floating point registers
    static constexpr int FREG_SIZE = 32;
    std::array<float, FREG_SIZE> m_freg = {{0}};

    // TODO Memory

    // 32bit Instruction code
    using Instruction = uint32_t;

    Instruction fetch();
    OpCode decodeOpCode(Instruction);
    void exec(OpCode, Instruction);

    // Function for each instruction
    void nop(Instruction);
    void add(Instruction);
    void addi(Instruction);
    void sub(Instruction);

    std::unordered_map<OpCode,
        std::function<void(Instruction)>, OpCodeHash> m_inst_funcs;
    std::unordered_map<OpCode,
        int64_t, OpCodeHash> m_inst_cnt;

    // Operand
    /*
     * Type R:
     * -----------------------------------------
     * | opcode | rs | rt | rd | shamt |       |
     * -----------------------------------------
     * | 6      | 5  | 5  | 5  | 5     |       |
     * -----------------------------------------
     */
    struct OperandR {
        int rs;
        int rt;
        int rd;
        int shamt;
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
        int rs;
        int rt;
        int immediate;
    };

    void initInst();

    OperandR decodeR(Instruction);
    OperandI decodeI(Instruction);

    /*
     * Extract bit string
     * bitset(10110111000..., 0, 8) = 10110111
     */
    int32_t bitset(int32_t inst, int begin, int end)
    {
        int len = end - begin;
        inst <<= begin;
        inst >>= (32 - len);
        return inst;
    }
};
