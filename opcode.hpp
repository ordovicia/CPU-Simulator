#pragma once

#include <cstdint>
#include <functional>

enum class OpCode : uint32_t {
    NOP = 0x0,
    ADD = 0x1,
    ADDI = 0x2,
    SUB = 0x3,
};

struct OpCodeHash {
    size_t operator()(OpCode op) const noexcept
    {
        return std::hash<uint32_t>{}(static_cast<uint32_t>(op));
    }
};
