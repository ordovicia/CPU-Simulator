#include <iostream>
#include "util.hpp"

using namespace std;

void myassert_impl(bool b, int l);

#define myassert(b) \
    myassert_impl((b), __LINE__)

int main()
{
    cout << hex;

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

    myassert(signExt5(0x0f) == 0x0f);
    myassert(signExt5(0x1f) == 0xffffffff);
    myassert(signExt8(0x7f) == 0x7f);
    myassert(signExt8(0xff) == 0xffffffff);

    return 0;
}

void myassert_impl(bool b, int l)
{
    if (b)
        return;
    cerr << "Assertion failed @ " << l << endl;
}
