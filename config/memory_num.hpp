#pragma once

#include <cstddef>
#include "util.hpp"

/*
 * メモリ量の設定。
 *
 * int32_tの配列がこの要素数だけ確保される。
 */
constexpr size_t MEMORY_NUM = 1000000;

inline void checkMemoryIndex(int32_t idx)
{
    if (idx < 0 || idx >= static_cast<int32_t>(MEMORY_NUM))
        FAIL("# Error: Memory index out of range");
}
