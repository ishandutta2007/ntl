

#include <NTL/config.h>


#ifdef NTL_GMP_LIP
#include <gmp.h>
#endif


int main() { 
#ifdef NTL_GMP_LIP
   mpz_t x;
   mpz_init(x);
#endif

   return 0; 
}







