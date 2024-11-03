#ifndef _MATH_H
#define _MATH_H 1

#include <sys/defs.h>

#define HUGE_VAl (__builtin_huge_val())
#define HUGE_VALF (__builtin_huge_valf())
#define HUGE_VALL (__builtin_huge_vall())

#define HUGE_VAL_F16 (__builtin_huge_valf16())
#define HUGE_VAL_F32 (__builtin_huge_valf32())
#define HUGE_VAL_F64 (__builtin_huge_valf64())
#define HUGE_VAL_F128 (__builtin_huge_valf128())
#define HUGE_VAL_F32X (__builtin_huge_valf32x())
#define HUGE_VAL_F64X (__builtin_huge_valf64x())
#define HUGE_VAL_F128X (__builtin_huge_valf128x())

#  define INFINITY (__builtin_inff ())
#  define NAN (__builtin_nanf (""))
#  define SNANF (__builtin_nansf (""))
#  define SNAN (__builtin_nans (""))
#  define SNANL (__builtin_nansl (""))
# define SNANF16 (__builtin_nansf16 (""))
# define SNANF32 (__builtin_nansf32 (""))
# define SNANF64 (__builtin_nansf64 (""))
# define SNANF128 (__builtin_nansf128 (""))
# define SNANF32X (__builtin_nansf32x (""))
# define SNANF64X (__builtin_nansf64x (""))
# define SNANF128X (__builtin_nansf128x (""))

#include <sys/flt-eval-method.h>

# if __GLIBC_FLT_EVAL_METHOD == 0 || __GLIBC_FLT_EVAL_METHOD == 16
typedef float float_t;
typedef double double_t;
# elif __GLIBC_FLT_EVAL_METHOD == 1
typedef double float_t;
typedef double double_t;
# elif __GLIBC_FLT_EVAL_METHOD == 2
typedef long double float_t;
typedef long double double_t;
# elif __GLIBC_FLT_EVAL_METHOD == 32
typedef _Float32 float_t;
typedef double double_t;
# elif __GLIBC_FLT_EVAL_METHOD == 33
typedef _Float32x float_t;
typedef _Float32x double_t;
# elif __GLIBC_FLT_EVAL_METHOD == 64
typedef _Float64 float_t;
typedef _Float64 double_t;
# elif __GLIBC_FLT_EVAL_METHOD == 65
typedef _Float64x float_t;
typedef _Float64x double_t;
# elif __GLIBC_FLT_EVAL_METHOD == 128
typedef _Float128 float_t;
typedef _Float128 double_t;
# elif __GLIBC_FLT_EVAL_METHOD == 129
typedef _Float128x float_t;
typedef _Float128x double_t;
# else
#  error "Unknown __GLIBC_FLT_EVAL_METHOD"
# endif

float floorf(float arg) __CONST;
double floor(double arg) __CONST;
long double floorl(long double arg) __CONST;

#endif // MATH_H