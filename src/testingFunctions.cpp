#include "testingFunctions.hpp"
#define _USE_MATH_DEFINES
#include <cmath>

double quadraticFunction(double x) {
    return std::pow(x, 2) - 5 * x + 6;
}

double rastriginFunction(double x) {
    return 10 + std::pow(x, 2) - 10 * std::cos(2 * M_PI * x);
}
