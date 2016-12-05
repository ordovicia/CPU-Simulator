#include <iostream>
#include "util.hpp"

using namespace std;

#define myassert(b)                                        \
    if (not(b)) {                                          \
        cerr << "Assertion failed @ " << __LINE__ << endl; \
        return 1;                                          \
    }

int main()
{
    myassert(bitset(0x00ff00ff, 0, 8) == 0x00000000);
    myassert(bitset(0x00ff00ff, 0, 16) == 0x000000ff);
    myassert(bitset(0x00ff00ff, 0, 24) == 0x0000ff00);
    myassert(bitset(0x00ff00ff, 0, 32) == 0x00ff00ff);
    myassert(bitset(0x00ff00ff, 8, 16) == 0x000000ff);
    myassert(bitset(0x00ff00ff, 8, 24) == 0x0000ff00);
    myassert(bitset(0x00ff00ff, 8, 32) == 0x00ff00ff);
    myassert(bitset(0x00ff00ff, 16, 24) == 0x00000000);
    myassert(bitset(0x00ff00ff, 16, 32) == 0x000000ff);
    myassert(bitset(0x00ff00ff, 24, 32) == 0x000000ff);

    myassert(signExt(0x0f, 5) == 0x0f);
    myassert(signExt(0x1f, 5) == 0xffffffff);
    myassert(signExt(0x7f, 8) == 0x7f);
    myassert(signExt(0xff, 8) == 0xffffffff);
    myassert(signExt(0xfff8, 16) == 0xfffffff8);

    cerr << "All test passed" << endl;

    return 0;
}
