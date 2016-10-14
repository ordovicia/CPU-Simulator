#pragma once

#include <cstdint>
#include <functional>

enum class OpCode : uint32_t {
    NOP = 0,
    ADD = 1,
    ADDI = 2,
    SUB = 3,
    HALT = 60,
};

struct OpCodeHash {
    size_t operator()(OpCode op) const noexcept
    {
        return std::hash<uint32_t>{}(static_cast<uint32_t>(op));
    }
};
