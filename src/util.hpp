#pragma once

#include <cstdint>

#define FAIL(msg)                  \
    endwin();                      \
    std::cerr << msg << std::endl; \
    std::exit(1);

/*
 * Extract bit string.
 * bitset(10110111000..., 0, 8) = 0..010110111
 */
uint32_t bitset(uint32_t inst, int begin, int end);
uint32_t bitset64(uint64_t inst, int begin, int end);

/*
 * Sign extention for 5bit
 * signExt5(01010) -> 0..00001010
 * signExt5(11010) -> 1..11111010
 */
uint32_t signExt5(uint32_t x);

/*
 * streq("hoge", "hoge") == true
 */
bool streq(const char* s1, const char* s2);

/*
 * streqn("hoge1", "hoge2", 4) == true
 */
bool streqn(const char* s1, const char* s2, int n);
