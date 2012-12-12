/* faster versions of 2**x in single, double precision.
 *
 * Based on the Cephes math library 2.8
 */

#include "fastermath.h"
#include "fm_internal.h"

/* optimizer friendly implementation of exp2(x).
 *
 * strategy:
 *
 * split argument into an integer part and a fraction:
 * ipart = floor(x+0.5);
 * fpart = x - ipart;
 *
 * compute exp2(ipart) from setting the ieee754 exponent
 * compute exp2(fpart) using a pade' approximation for x in [-0.5;0.5[
 *
 * the result becomes: exp2(x) = exp2(ipart) * exp2(fpart)
 */

static const double fm_exp2_q[] __attribute__ ((aligned(16))) = {
/*  1.00000000000000000000e0, */
    2.33184211722314911771e2,
    4.36821166879210612817e3
};
static const double fm_exp2_p[] __attribute__ ((aligned(16))) = {
    2.30933477057345225087e-2,
    2.02020656693165307700e1,
    1.51390680115615096133e3
};

double fm_exp2(double x)
{
    double   ipart, fpart, px, qx;
    udi_t    epart;

    ipart = __builtin_floor(x+0.5);
    fpart = x - ipart;
    epart.f = 0.0;
    epart.b.e = ((unsigned int) ipart) + FM_DOUBLE_BIAS;

    x = fpart*fpart;

    px =        fm_exp2_p[0];
    px = px*x + fm_exp2_p[1];
    qx =    x + fm_exp2_q[0];
    px = px*x + fm_exp2_p[2];
    qx = qx*x + fm_exp2_q[1];

    px = px * fpart;

    x = 1.0 + 2.0*(px/(qx-px));
    return epart.f*x;
}

/* optimizer friendly implementation of exp2f(x).
 *
 * strategy:
 *
 * split argument into an integer part and a fraction:
 * ipart = floorf(x+0.5);
 * fpart = x - ipart;
 *
 * compute exp2f(ipart) from setting the ieee754 exponent
 * compute exp2f(fpart) using a polynomial approximation for x in [-0.5;0.5[
 *
 * the result becomes: exp2f(x) = exp2f(ipart) * exp2f(fpart)
 */

static const float fm_exp2f_p[] __attribute__ ((aligned(16))) = {
    1.535336188319500e-4f,
    1.339887440266574e-3f,
    9.618437357674640e-3f,
    5.550332471162809e-2f,
    2.402264791363012e-1f,
    6.931472028550421e-1f,
    1.000000000000000f
};

float fm_exp2f(float x)
{
    float ipart, fpart;
    ufi_t epart;

    ipart = __builtin_floorf(x + 0.5f);
    fpart = x - ipart;
    epart.u = 0U;
    epart.b.e = ((unsigned int) ipart) + FM_FLOAT_BIAS;

    x =           fm_exp2f_p[0];
    x = x*fpart + fm_exp2f_p[1];
    x = x*fpart + fm_exp2f_p[2];
    x = x*fpart + fm_exp2f_p[3];
    x = x*fpart + fm_exp2f_p[4];
    x = x*fpart + fm_exp2f_p[5];
    x = x*fpart + fm_exp2f_p[6];

    return epart.f*x;
}

#if defined(LIBM_ALIAS)
/* include aliases to the equivalent libm functions for use with LD_PRELOAD. */
double exp2(double x) __attribute__ ((alias("fm_exp2")));
float exp2f(float x) __attribute__ ((alias("fm_exp2f")));
#endif

/* 
 * Local Variables:
 * mode: c
 * compile-command: "make"
 * c-basic-offset: 4
 * fill-column: 76 
 * indent-tabs-mode: nil 
 * End: 
 */
