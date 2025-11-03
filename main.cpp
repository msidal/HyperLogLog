#include <iostream>
#include <string>
#include "hyperloglog.hpp"

int main(int argc, char** argv) {
    uint8_t p = 14;
    if (argc >= 2) {
        int pi = std::stoi(argv[1]);
        if (pi < 4) pi = 4;
        if (pi > 20) pi = 20;
        p = static_cast<uint8_t>(pi);
    }

    HyperLogLog hll(p);
    int N = (argc >= 3)   ? std::stoi(argv[2]) : 100000;

    for (int i = 0; i < N; ++i) {
        hll.add(std::string("user_") + std::to_string(i));
    }

    double est = hll.estimate();

    std::cout << "actual:   " << N << "\n";
    std::cout << "estimate: " << (long long)std::llround(est) << "\n";

    return 0;
}
