#include <xmmintrin.h>
#include <emmintrin.h>
#ifndef _WIN32
#include <smmintrin.h>
#include <tmmintrin.h>
#endif

#include <climits>
#include <iostream>
#include <iomanip>

#define _USE_SSE2 1
#include "Test/Bits.h"
using Test::m128_dump;
using namespace std;


namespace {
  static __m128i bit128[128];

  void init_bits()  {
    int w_32 = CHAR_BIT*sizeof(int);
    for(size_t i=0; i<128; ++i)   {
      unsigned int* ui = (unsigned int*)&(bit128[i]);
#ifdef _USE_SSE2
      _mm_stream_si128(&bit128[i],_mm_setzero_si128());
#else
      ui[0] = ui[1] = ui[2] = ui[3] = 0;
#endif
      ui[i/w_32] = 1<<(i%w_32);
    }
  }

  int match_bit_mask(const __m128i& a, const __m128i& mask)  {
#ifdef _USE_SSE2
    return _mm_movemask_epi8(_mm_cmpeq_epi16(_mm_and_si128(a,mask),mask));
#else
    unsigned int* ua = (unsigned int*)&a;
    unsigned int* um = (unsigned int*)&mask;
    return (ua[0]&um[0])==um[0] && (ua[1]&um[1])==um[1] && (ua[2]&um[2])==um[2] && (ua[3]&um[3])==um[3];
#endif
  }

  int test_bit(const __m128i& s, int b)  {
#ifdef _USE_SSE2
    return _mm_movemask_epi8(_mm_cmpeq_epi16(_mm_and_si128(s,bit128[b]),bit128[b]));
#else
    unsigned int* ui = (unsigned int*)&s;
    return 0 != (ui[b/32]&(1<<(b%32)));
#endif
  }

  __m128i set_bit(const __m128i& s, int b)  {
#ifdef _USE_SSE2
    return _mm_or_si128(s,bit128[b]);
#else
    unsigned int* ui = (unsigned int*)&s;
    ui[b/32] |= (1<<(b%32));
    return s;
#endif
  }

  int test()  {
    init_bits();
#if 0
    typedef union __declspec(intrin_type) _CRT_ALIGN(16) __m128i {
      __int8              m128i_i8[16];
      __int16             m128i_i16[8];
      __int32             m128i_i32[4];    
      __int64             m128i_i64[2];
      unsigned __int8     m128i_u8[16];
      unsigned __int16    m128i_u16[8];
      unsigned __int32    m128i_u32[4];
      unsigned __int64    m128i_u64[2];
    } __m128i;
#endif
    __m128i s, bit128[128];
    size_t i;
    int w_8  = CHAR_BIT;
    int w_16 = CHAR_BIT*sizeof(short);
    int w_32 = CHAR_BIT*sizeof(int);
    for(i=0; i<128; ++i)   {
      _mm_stream_si128(&bit128[i],_mm_setzero_si128());
      ((int*)&bit128[i])[i/w_32] = 1<<(i%w_32);
    }
    m128_dump(s);
    int bit = 48;
    cout << "================= BIT:" << bit << endl;
    cout << dec << bit/w_8  << " "  << int(bit%w_8) << " ";
    cout << dec << bit/w_16 << " "  << int(bit%w_16) << " ";
    cout << dec << bit/w_32 << " "  << int(bit%w_32) << " ";
    cout << endl;

    // Set a few bits on the mask
    s = set_bit(set_bit(set_bit(set_bit(set_bit(set_bit(_mm_setzero_si128(),0),5),16),48),100),127);
    m128_dump(s);
    m128_dump(bit128[0]);
    m128_dump(bit128[5]);
    m128_dump(bit128[16]);
    m128_dump(bit128[48]);
    m128_dump(bit128[100]);
    m128_dump(bit128[127]);
    cout << endl;
    for(int b=0; b<128;++b)  {
      int bb  = test_bit(s,b);
      int bbb = test_bit(bit128[b],b);
      int m1  = match_bit_mask(s,bit128[b]);
      cout << "1111:" << "Bit:" << dec << setfill(' ') << right << setw(3) << b 
	   << " Result:" << hex << setw(4) << bb
	   << " test:  " << hex << setw(4) << bbb
	   << " match: " << hex << setw(4) << m1;
      if ( bb == 0xffff )  {
	cout << " Bit: " << dec << b << " !!!!! ";
      }
      cout << endl;
    }
    return 1;
  }
}

int sse_bit_test3()  {  return test(); }

