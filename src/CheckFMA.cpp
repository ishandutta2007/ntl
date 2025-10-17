
#include <NTL/ctools.h>

#include <cstdlib>
#include <iostream>


#if (defined(NTL_SIMDE_LIB))

#define SIMDE_ENABLE_NATIVE_ALIASES
#include "/Users/shoup/repos/simde/x86/fma.h"

#if (!defined(SIMDE_ARM_NEON_A64V8_NATIVE))
#error "AVX2 with FMA not supported well by simde"
#endif

#if 1
// hack to override SIMDE limitations
#ifdef _mm256_fmadd_pd
#undef _mm256_fmadd_pd
#endif

inline static __m256d
arm_fmadd_pd(__m256d a, __m256d b, __m256d c)
{
   simde__m256d_private
   r_,
   a_ = simde__m256d_to_private(a),
   b_ = simde__m256d_to_private(b),
   c_ = simde__m256d_to_private(c);
   r_.m128d[0] = _mm_fmadd_pd(a_.m128d[0], b_.m128d[0], c_.m128d[0]);
   r_.m128d[1] = _mm_fmadd_pd(a_.m128d[1], b_.m128d[1], c_.m128d[1]);
   return simde__m256d_from_private(r_);
}


#define _mm256_fmadd_pd(a, b, c) arm_fmadd_pd(a, b, c)  
#endif

#else

#include <immintrin.h>

#if (!defined(__GNUC__) || !defined(__x86_64__) || !defined(__AVX2__))
#error "FMA not supported"
#endif

#endif






#if (NTL_BITS_PER_LONG != 64 || NTL_BITS_PER_INT != 32 || NTL_DOUBLE_PRECISION != 53)
#error "FMA not supported"
// sanity check -- code that uses this feature also relies on this
#endif

#ifndef NTL_HAVE_ALIGNED_ARRAY
#error "FMA not supported"
#endif

using namespace std;


void fun(double * x, const double *a, const double *b)
{
   __m256d xvec, avec, bvec, cvec;

   avec = _mm256_load_pd(a);
   bvec = _mm256_load_pd(b);
   xvec = _mm256_load_pd(x);

   xvec = _mm256_fmadd_pd(avec, bvec, xvec);

   _mm256_store_pd(x, xvec);
}
int main()
{
   NTL_AVX_LOCAL_ARRAY(vp, double, 12);

   double *a = vp + 0*4;
   double *b = vp + 1*4;
   double *x = vp + 2*4;

   a[0] = atoi("1");
   a[1] = atoi("2");
   a[2] = atoi("3");
   a[3] = atoi("4");

   b[0] = atoi("2");
   b[1] = atoi("3");
   b[2] = atoi("4");
   b[3] = atoi("5");

   x[0] = atoi("3");
   x[1] = atoi("4");
   x[2] = atoi("5");
   x[3] = atoi("6");

   fun(x, a, b);

   if (x[0] == 5 && x[1] == 10 && x[2] == 17 && x[3] == 26)
      return 0;
   else
      return -1;
}



