#include <cstring>
#include "util.hpp"

uint32_t bitset(uint32_t inst, int begin, int end)
{
    int len = end - begin;
    inst <<= begin;
    inst >>= (32 - len);
    return inst;
}

uint32_t signExt(uint32_t x, int bits)
{
    if (x & (1 << (bits - 1)))
        return (~0u << bits) | x;
    else
        return x;
}

bool streq(const char* s1, const char* s2) { return strcmp(s1, s2) == 0; }
bool streqn(const char* s1, const char* s2, int n)
{
    for (int i = 0; i < n; i++) {
        auto c1 = s1[i];
        auto c2 = s2[i];
        if (c1 != c2 || c1 == '\0' || c2 == '\0')
            return false;
    }

    return true;
}

uint32_t ftob(float f)
{
    FloatBit fb{f};
    return fb.b;
}

float btof(uint32_t b)
{
    FloatBit fb;
    fb.b = b;
    return fb.f;
}
