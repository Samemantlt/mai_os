#include <iostream>

extern "C" int gcf(int a, int b);
extern "C" float square(float a, float b);

int main() {
    while (true) {
        std::cout << "Select function (1 - gcf(int, int), 2 - square(float, float)) and write arguments: ";

        int function_no;
        std::cin >> function_no;

        switch (function_no) {
            case 1: {
                int a, b;
                std::cin >> a >> b;

                std::cout << gcf(a, b) << std::endl;
                break;
            }
            case 2: {
                float a, b;
                std::cin >> a >> b;

                std::cout << square(a, b) << std::endl;
                break;
            }
            default:
                return 0;
        }
    }
}