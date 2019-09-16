#include <iostream>
#include <fstream>
#include <cmath>
#include "system.h"

System::DemandConst::DemandConst(uint32_t val) {
    this->val = val;
}

uint32_t System::DemandConst::operator()() {
    return this->val;
}

System::DemandKDE::DemandKDE(std::string fn, int32_t seed) {
    std::ifstream ifs(fn);
    double var = 0, h, mean;
    ifs >> h;
    ifs >> mean;
    ifs >> var;
    this->h = h;
    this->mean = mean;
    this->c = 1 / sqrt(1 + h * h / var);
    for (int32_t i = 0, *j = this->data; i < 100; i++, ifs >> *(j++)); 
    this->generator.change_seed(seed);
}

static inline double norm_cdf(double x) {
    if (x > 0.0) {
        if (x > 45.0) {
            return 1.0;
        } else {
            double temp = (((((x * 5.383e-6 + 4.88906e-5) * x + 3.80036e-5) * x +
                  .0032776263) * x + .0211410061) * x + .049867347) * x + 1.;
            temp *= temp;
            temp *= temp;
            temp *= temp;
            return (1. - .5 / (temp * temp));
        }
    } else {
        if (x < -45.0) {
            return 0.0;
        } else {
            x = -x;
            return (0.50 / pow((1.0 + (0.0498673470 +
                    (0.0211410061 + (0.0032776263 + (0.0000380036 +
                    (0.0000488906 + 0.0000053830 * x) * x) * x) * x) * x) *
                    x), 16));
        }
    }
}

static inline double norm_inv(double p) {
    double delta;
    if (p >= 0.5) {
        delta = p;
    } else {
        delta = 1.0 - p;
    }
    double w = sqrt(-log((1.0 - delta) * (1.0 - delta)));
    double low = w - (2.515517 + (0.802853 + 0.010328 * w) * w) /
                 (1.0 + (1.432788 + (0.189269 + 0.001308 * w) * w) * w);
    double high = low + 4.5E-4;
    low -= 4.5E-4;
    double mid;
    do {
        mid = (low + high) / 2.0;
        if (norm_cdf(mid) > delta) {
            low = mid;
        } else {
            high = mid;
        }
    } while (fabs(mid - (low + high) / 2.0) > 1.0E-15);
    if (p >= 0.5) {
        return mid;
    } else {
        return -mid;
    }
}

uint32_t System::DemandKDE::operator()() {
    uint32_t i = this->generator() * 100;
    double w = norm_inv(this->generator());
    return fabs(this->mean + (this->data[i] - this->mean + this->h * w) * this->c) + 0.5;
}
