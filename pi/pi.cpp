#include "../utils.h"

#include <iostream>
#include <iomanip>
#include <math.h>
#include <stdlib.h>

int main() {
    int inside_circle = 0;
    int runs = 0;
    std::cout << std::fixed << std::setprecision(12);
    const clock_t begin_time = clock();
    while (true) {
        runs++;
        auto x = rng(-1,1);
        auto y = rng(-1,1);
        if (x*x + y*y < 1)
            inside_circle++;

        if (runs % 100000 == 0)
            std::cout << "Estimate of Pi = "
                      << 4*double(inside_circle) / runs
                      << " - " << float( clock () - begin_time ) /  CLOCKS_PER_SEC
                      << '\n';
    }
}