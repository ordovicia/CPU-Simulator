#include <iostream>
#include <cstdlib>
#include <getopt.h>

void printHelp()
{
    printf("Usage: dec2bin [-i integer] [-f float]\n");
}

void printBits(uint32_t bits)
{
    for (int b = 0; b < 32; b++) {
        uint32_t bit = (bits << b) >> 31;
        printf("%d", bit);
    }
    printf("\n");
}

int main(int argc, char** argv)
{
    using namespace std;

    if (argc < 2) {
        printHelp();
        return 1;
    }

    int result;
    while ((result = getopt(argc, argv, "i:f:")) != -1) {
        switch (result) {
        case 'i': {
            int32_t i = atoi(optarg);
            cout << i << endl;
            printBits(i);
            break;
        }
        case 'f': {
            float f = static_cast<float>(atof(optarg));
            cout << f << endl;
            union FloatBit {
                float f;
                uint32_t u;
            };
            FloatBit fb{f};
            printBits(fb.u);
            break;
        }
        default:
            printHelp();
            return 1;
        }
    }

    return 0;
}
