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

    myassert(signExt(0x01, 5) == 0x01);
    myassert(signExt(0x12, 5) == 0xfffffff2);
    myassert(signExt(0x73, 8) == 0x73);
    myassert(signExt(0xf4, 8) == 0xfffffff4);
    myassert(signExt(0x7765, 16) == 0x7765);
    myassert(signExt(0xfa98, 16) == 0xfffffa98);

    cerr << "All test passed" << endl;

    return 0;
}
