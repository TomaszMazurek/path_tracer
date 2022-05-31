#include <iostream>
#include <iomanip>
#include <math.h>
#include <stdlib.h>
#include "../utils.h"

int main() {
    for (int i = 0; i < 1000; i++) {
        auto r1 = rng();
        auto r2 = rng();
        auto x = cos(2*pi*r1)*2*sqrt(r2*(1-r2));
        auto y = sin(2*pi*r1)*2*sqrt(r2*(1-r2));
        auto z = 1 - 2*r2;
        std::cout << x << "," << y << "," << z << '\n';
    }
}