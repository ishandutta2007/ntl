#include <NTL/ctools.h>

#include <cstdlib>
#include <iostream>



#if (defined(__GNUC__) && defined(__x86_64__) && defined(__AVX__))
// NOTE: gcc and clang define __PCLMUL__, but icc does not

#include <wmmintrin.h>

void
pclmul_mul1 (unsigned long *c, unsigned long a, unsigned long b)
{
   __m128i aa = _mm_setr_epi64( _mm_cvtsi64_m64(a), _mm_cvtsi64_m64(0));
   __m128i bb = _mm_setr_epi64( _mm_cvtsi64_m64(b), _mm_cvtsi64_m64(0));
   _mm_storeu_si128((__m128i*)c, _mm_clmulepi64_si128(aa, bb, 0));
}

#elif (defined(NTL_SIMDE_LIB))

#define SIMDE_ENABLE_NATIVE_ALIASES
#include <NTL/simde/x86/sse2.h>
#include <NTL/simde/x86/clmul.h>


#if (defined(SIMDE_ARM_NEON_A64V8_NATIVE))

void
pclmul_mul1 (unsigned long *c, unsigned long a, unsigned long b)
{
   _mm_storeu_si128((__m128i*)c, 
                    (__m128i) vmull_p64((poly64_t) a, (poly64_t) b));
}

#else
#error "PCLMUL not supported"
#endif


#else
#error "PCLMUL not supported"

#endif



#if (NTL_BITS_PER_LONG != 64)
#error "PCLMUL not supported"
#endif

using namespace std;

// sanity check: we require little endianness for general
// compatability of memory layout (only needed when using SIMDE).
// this means little endian by lane and by byte within a lane.
int little_endian()
{
   __m128i v = _mm_set_epi32(_ntl_nofold(0), _ntl_nofold(0), _ntl_nofold(0), _ntl_nofold(1));
   return ((char*)&v)[0] == 1;
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

   unsigned long a = ((unsigned long) _ntl_nofold(15)) << (NTL_BITS_PER_LONG-4);
   unsigned long b = _ntl_nofold(6);
   unsigned long c[2];

   pclmul_mul1(c, a, b);

   unsigned long c0 = ((unsigned long) _ntl_nofold(1)) << (NTL_BITS_PER_LONG-3);
   unsigned long c1 = _ntl_nofold(2);


   if (!little_endian()) return -1;


   if (c[0] == c0 && c[1] == c1) 
      return 0;
   else
      return -1;
}
