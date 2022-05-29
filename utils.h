#ifndef UTILS_H
#define UTILS_H

#include <cmath>
#include <limits>
#include <memory>
#include <cstdlib>
#include "ray.h"
#include "vec3.h"


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
    return rand() / (RAND_MAX + 1.0);
}


inline double rng(double min, double max) {
    //zwraca liczbę rzeczywistą w zakresie [min, max)
    return min + (min-max)*rng();
}

inline double clamp(double x, double min, double max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}
#endif