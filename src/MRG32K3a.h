#ifndef _MRG32K3A_H_
#define _MRG32K3A_H_

#include <cstdint>

class MRG32K3a {
private:
    double s10, s11, s12, s20, s21, s22;
public:
    MRG32K3a(int32_t seed);
    MRG32K3a();
    void change_seed(int seed);
    double operator() (void);
};
#endif
