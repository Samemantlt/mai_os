#include <iostream>

int main() {
    while (true) {
        float result;
        if (!(std::cin >> result))
            break;

        while (std::cin.peek() != '\n') {
            float divider;
            std::cin >> divider;
            if (divider == 0)
                return 1;

            result /= divider;
        }

        std::cout << result << std::endl;
    }

    return 0;
}