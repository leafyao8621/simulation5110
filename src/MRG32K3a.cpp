#include "MRG32K3a.h"
#define norm 2.328306549295728e-10
#define m1   4294967087.0
#define m2   4294944443.0
#define a12     1403580.0
#define a13n     810728.0
#define a21      527612.0
#define a23n    1370589.0

MRG32K3a::MRG32K3a(int32_t seed) {
    s10 = seed;
    s11 = seed;
    s12 = seed;
    s20 = seed;
    s21 = seed;
    s22 = seed;
}

MRG32K3a::MRG32K3a() {
    s10 = 1;
    s11 = 1;
    s12 = 1;
    s20 = 1;
    s21 = 1;
    s22 = 1;
}

void MRG32K3a::change_seed(int32_t seed) {
    s10 = seed;
    s11 = seed;
    s12 = seed;
    s20 = seed;
    s21 = seed;
    s22 = seed;
}

double MRG32K3a::operator() (void) {
      long k;
      double p1, p2;
      /* Component 1 */
      p1 = a12 * s11 - a13n * s10;
      k = p1 / m1;
      p1 -= k * m1;
      if (p1 < 0.0)
      p1 += m1;
      s10 = s11;
      s11 = s12;
      s12 = p1;

      /* Component 2 */
      p2 = a21 * s22 - a23n * s20;
      k = p2 / m2;
      p2 -= k * m2;
      if (p2 < 0.0)
      p2 += m2;
      s20 = s21;
      s21 = s22;
      s22 = p2;

      /* Combination */
      if (p1 <= p2)
      return ((p1 - p2 + m1) * norm);
      else
      return ((p1 - p2) * norm);
}
