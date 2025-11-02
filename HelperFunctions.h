#ifndef RAYTRACER_HELPERFUNCTIONS_H
#define RAYTRACER_HELPERFUNCTIONS_H

#include <cmath>
#include <iostream>
#include <optional>
#include <random>
#include <limits>

const double pi = 3.1415926535897932385;
const double infinity = std::numeric_limits<double>::infinity();

inline double degreesToRadians(double degrees) {
    return (degrees * pi) / 180.0;
}

inline double radiansToDegrees(double radians) {
    return (180.0 * radians) / pi;
}

namespace Random {
    // Random engine initialized once
    static std::random_device rd;
    static std::mt19937 gen(rd());

    inline double randomDouble(double minimum, double maximum) {
        std::uniform_real_distribution<double> distribution(minimum, maximum);
        return distribution(gen);
    }

    inline double randomDouble0to1() {
        static std::uniform_real_distribution<double> distribution(0.0, 1.0);
        return distribution(gen);
    }

    inline int randomInt(int minimum, int maximum) {
        std::uniform_int_distribution<int> distribution(minimum, maximum);
        return distribution(gen);
    }

    inline int randomInt0to255() {
        static std::uniform_int_distribution<int> distribution(0, 255);
        return distribution(gen);
    }
}

#endif //RAYTRACER_HELPERFUNCTIONS_H
