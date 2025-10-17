#include <NTL/ctools.h>

#include <cstdlib>
#include <iostream>


#if (defined(NTL_SIMDE_LIB))

#define SIMDE_ENABLE_NATIVE_ALIASES
#include "/Users/shoup/repos/simde/x86/clmul.h"

#if (!defined(SIMDE_ARM_NEON_A64V8_NATIVE))
#error "PCLMUL not supported"
#endif

#if 1
// hack to override SIMDE limitations

#if (!defined(__GNUC__))
#error "PCLMUL not supported"
#endif


#ifdef _mm_clmulepi64_si128 
#undef _mm_clmulepi64_si128
#endif


inline static __m128i 
arm_pclmul(__m128i a, __m128i b)
{
   __m128i res;
   __asm__ ("pmull    %0.1q, %1.1d, %2.1d                \n\t"  
           : "=w" (res) : "w" (a), "w" (b) );      
   return res;
}

#define _mm_clmulepi64_si128(a, b, _ignore) arm_pclmul(a, b)
#endif


#else

#include <wmmintrin.h>

#if (!defined(__GNUC__) || !defined(__x86_64__) || !defined(__AVX__))
#error "PCLMUL not supported"
#endif

#if (NTL_BITS_PER_LONG != 64)
#error "PCLMUL not supported"
#endif

// NOTE: gcc and clang define __PCLMUL__, but icc does not

#endif

using namespace std;



void
pclmul_mul1 (unsigned long *c, unsigned long a, unsigned long b)
{
   __m128i aa = _mm_setr_epi64( _mm_cvtsi64_m64(a), _mm_cvtsi64_m64(0));
   __m128i bb = _mm_setr_epi64( _mm_cvtsi64_m64(b), _mm_cvtsi64_m64(0));
   _mm_storeu_si128((__m128i*)c, _mm_clmulepi64_si128(aa, bb, 0));
}

int main()
{

// Example with 8 bits per word
//        11110000
//           x 110
//----------------
//       111100000
//      1111000000
//----------------
//      1000100000

   unsigned long a = ((unsigned long) atoi("15")) << (NTL_BITS_PER_LONG-4);
   unsigned long b = atoi("6");
   unsigned long c[2];

   pclmul_mul1(c, a, b);

   unsigned long c0 = ((unsigned long) atoi("1")) << (NTL_BITS_PER_LONG-3);
   unsigned long c1 = atoi("2");

   if (c[0] == c0 && c[1] == c1) 
      return 0;
   else
      return -1;
}
