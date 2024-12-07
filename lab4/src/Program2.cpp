#include <iostream>
#include <dlfcn.h>

typedef int (*gcf_f)(int a, int b);
typedef float (*square_f)(float a, float b);

gcf_f gcf = nullptr;
square_f square = nullptr;
void* lib = nullptr;

void load_functions(std::string lib_path){
    if (lib != nullptr)
        dlclose(lib);

    lib = dlopen(lib_path.c_str(), RTLD_LAZY);
    if (lib == nullptr) {
        std::cerr << "Error loading library: " << dlerror() << std::endl;
        exit(1);
    }

    gcf = (gcf_f)dlsym(lib, "gcf");
    square = (square_f)dlsym(lib, "square");

    if (!gcf || !square) {
        std::cerr << "Failed to load symbols: " << dlerror() << std::endl;
        dlclose(lib);
        exit(1);
    }
}


int main() {
    load_functions("./libLibImpl1.so");

    while (true) {
        std::cout << "Menu:\n"
                     "0 - Change library source\n"
                     "1 - Invoke gcf(int, int)\n"
                     "2 - Invoke square(float, float)\n"
                     "Enter:" << std::endl;

        int function_no;
        std::cin >> function_no;

        switch (function_no) {
            case 0: {
                std::cout << "Enter library path (eg. ./libLibImpl1.so):" << std::endl;

                std::string lib_path;
                std::cin.ignore(255, '\n');
                std::getline(std::cin, lib_path);

                load_functions(lib_path);
                break;
            }
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