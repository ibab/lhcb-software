#include <xmmintrin.h>
#include <emmintrin.h>
#ifndef _WIN32
#include <smmintrin.h>
#include <tmmintrin.h>
#endif
#include "Bits.h"

#include <iostream>
#include <iomanip>
#include <climits>
#include <cmath>
using namespace std;
#define MASK_EXTRACT(typ,x,n)   (((typ*)&x)[i])

namespace Test {
  template<class T> static void m128_dump_type(const char* prefix, __m128i s, const char* postfix) {
    size_t i;
    cout << prefix;
    for(i=0; i<sizeof(s)/sizeof(T); ++i)  {
      T c = MASK_EXTRACT(T,s,i);
      cout << setw(sizeof(T)*2) << hex << setfill('0') << (unsigned int)c << " " << postfix;
    }
    cout << endl;
    for(i=0; i<sizeof(s)/sizeof(T); ++i)  {
      T c = MASK_EXTRACT(T,s,i);
      for(size_t j=0, n=sizeof(c)*CHAR_BIT; j<n; ++j)  {
	cout << (char)((c&(1<<j)) ? '1' : '.');
      }
    }
    cout << endl;
  }
  void m128_dump(const __m128i& s)  {
    m128_dump_type<unsigned char>         ("m128i_u8:   ", s, "");
    m128_dump_type<unsigned short>        ("m128i_u16:  ", s, " ");
    m128_dump_type<unsigned int>          ("m128i_u32:  ", s, "  ");
    m128_dump_type<unsigned long long int>("m128i_u64:  ", s, "  ");
  }


  void dump(const char* txt,const __m128i& v) {
    unsigned int* q = (unsigned int*)&v;
    cout << setw(32) << left << txt << ": " << hex
	 << setw(8) << q[0] << " " 
	 << setw(8) << q[1] << " " 
	 << setw(8) << q[2] << " " 
	 << setw(8) << q[3] << endl;
  }
}
