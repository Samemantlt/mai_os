#include <algorithm>


extern "C" int gcf(int a, int b) {
    for (int i = std::min(a, b); i > 1; i--) {
        if ((a % i == 0) && (b % i == 0)) {
            return i;
        }
    }
    return 1;
}