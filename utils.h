#ifndef UTILS_H
#define UTILS_H

#include <cmath>
#include <limits>
#include <memory>
#include <cstdlib>
#include <random>

using std::shared_ptr;
using std::make_shared;
using std::sqrt;


//constants
const double inf = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

//utils

inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}


inline double rng() {
    //zwraca liczbę rzeczywistą w zakresie [0,1)
    const double r = rand();
    return (r > 0.0 ? r : rand()) / (RAND_MAX + 1.0);
}


inline double rng_rand() {
    //zwraca liczbę rzeczywistą w zakresie [0,1)
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);
}

inline double rng(double min, double max) {
    //zwraca liczbę rzeczywistą w zakresie [min, max)
    return min + (max-min)*rng();
}

inline int random_int(int min, int max) {
    // Returns a random integer in [min,max].
    return static_cast<int>(rng(min, max+1));
}

inline double clamp(double x, double min, double max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

//wspolne deklaracje include
#include "ray.h"
#include "vec3.h"

#endif