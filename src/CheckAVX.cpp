#include <NTL/ctools.h>

#include <cstdlib>
#include <iostream>


#if (defined(__GNUC__) && defined(__x86_64__) && defined(__AVX__))

#include <immintrin.h>

#elif (defined(NTL_SIMDE_LIB))

#define SIMDE_ENABLE_NATIVE_ALIASES
#include <simde/x86/avx.h>

#if (!defined(SIMDE_ARM_NEON_A64V8_NATIVE))
#error "AVX not supported"
#endif

#else

#error "AVX not supported"

#endif




#if (NTL_BITS_PER_LONG != 64 || NTL_BITS_PER_INT != 32 || NTL_DOUBLE_PRECISION != 53)
#error "AVX not supported"
// sanity check -- code that uses this feature also relies on this
#endif

#ifndef NTL_HAVE_ALIGNED_ARRAY
#error "AVX not supported"
#endif

using namespace std;

void fun(double * x, const double *a, const double *b)
{
   __m256d xvec, avec, bvec, cvec;

   avec = _mm256_load_pd(a);
   bvec = _mm256_load_pd(b);
   xvec = _mm256_load_pd(x);

   xvec = _mm256_add_pd(_mm256_mul_pd(avec, bvec), xvec);

   _mm256_store_pd(x, xvec);
}

int main()
{
   NTL_AVX_LOCAL_ARRAY(vp, double, 12);

   double *a = vp + 0*4;
   double *b = vp + 1*4;
   double *x = vp + 2*4;

   a[0] = _ntl_nofold(1);
   a[1] = _ntl_nofold(2);
   a[2] = _ntl_nofold(3);
   a[3] = _ntl_nofold(4);

   b[0] = _ntl_nofold(2);
   b[1] = _ntl_nofold(3);
   b[2] = _ntl_nofold(4);
   b[3] = _ntl_nofold(5);

   x[0] = _ntl_nofold(3);
   x[1] = _ntl_nofold(4);
   x[2] = _ntl_nofold(5);
   x[3] = _ntl_nofold(6);

   fun(x, a, b);

// sanity check: we require little endianness for general
// compatability of memory layout (only needed when using SIMDE)

   int test_var = _ntl_nofold(1);
   char *test_little_endian = (char*)&test_var;
   if (!test_little_endian[0]) return -1;


   if (x[0] == 5 && x[1] == 10 && x[2] == 17 && x[3] == 26)
      return 0;
   else
      return -1;
}



