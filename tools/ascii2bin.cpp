#include <iostream>
#include <fstream>

int main()
{
    std::ofstream ofs{"test.bin"};

    uint32_t code = 0;
    int cnt = 0;
    char b;
    while (std::cin >> b) {
        std::cout << b;

        switch (b) {
        case '0':
            code <<= 1;
            break;
        case '1':
            code <<= 1;
            code |= 1;
            break;
        default:
            std::cerr << "ababa" << std::endl;
            return 1;
        }

        cnt++;
        if (cnt == 32) {
            std::cout << std::endl;

            auto d = reinterpret_cast<char*>(&code);
            ofs.write(d, sizeof(uint32_t));

            code = 0;
            cnt = 0;
        }
    }

    return 0;
}
