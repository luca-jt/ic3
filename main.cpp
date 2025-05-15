#include "ic3.h"
#include <iostream>

int main(const int argc, char const *argv[]) {
    std::string filepath;
    if (argc == 2) {
        filepath = argv[1];
    } else {
        std::cout << "Enter filepath: " << std::flush;
        std::getline(std::cin, filepath);
    }
    const auto ts = parse(filepath);
    std::cout << (ic3(ts) ? "\nSAFE" : "\nUNSAFE") << std::endl;
    return 0;
}