#include "../utils.h"

#include <iostream>
#include <iomanip>
#include <math.h>
#include <stdlib.h>

int main() {
    int N = 10000000;
    auto sum = 0.0;
    for (int i = 0; i < N; i++) {
        auto x = rng(0,2);
        sum += x*x;
    }
    std::cout << std::fixed << std::setprecision(12);
    std::cout << "I = " << 2 * sum/N << '\n';
}