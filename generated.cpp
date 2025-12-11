#include <iostream>

int main() {
    // Variables
    float a, b;

    goto block_0;

    block_0:
        goto block_1;

    block_1:
        a = 5;
        std::cout << "Executed: a = 5 " << std::endl;
        goto block_2;

    block_2:
        b = 3;
        std::cout << "Executed: b = 3 " << std::endl;
        goto block_3;

    block_3:
        if (a + b <= 12) {
            goto block_4;
        } else {
            goto block_7;
        }

    block_4:
        if (a > b) {
            goto block_6;
        } else {
            goto block_5;
        }

    block_5:
        a = a + 1;
        std::cout << "Executed: a = a + 1 " << std::endl;
        goto block_3;

    block_6:
        b = b + 1;
        std::cout << "Executed: b = b + 1 " << std::endl;
        goto block_3;

    block_7:
        std::cout << "Program Ended" << std::endl;
        return 0;

    return 0;
}
