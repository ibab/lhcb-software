#include <xmmintrin.h>
#include <emmintrin.h>
#ifndef _WIN32
#include <smmintrin.h>
#include <tmmintrin.h>
#endif

#include "RTL/rtl.h"
#include "Test/Bits.h"
using Test::dump;

int sse_bit_test0() {
  //int _mm_testz_si128 (__m128i s1, __m128i s2);
  // 32bit -> m128 __m128i s1 = _mm_cvtsi32_si128(0+16);
  long hits = VXD_HITS|ECAL_HITS|MUON_HITS|TRACKER_HITS;
  //__m128i s1 = _mm_cvtsi64_si128(hits);
  __m128i s1 = _mm_set_epi32(hits,hits,hits,hits);

  __m128i s2 = _mm_cvtsi32_si128(hits);
  s2 = _mm_set_epi32(hits,0,0,0);

  dump("s1",s1);
  dump("s2",s2);

  // SSE4, but not working
  // s2 = _mm_insert_epi32(s2,hits,3);
  // dump("_mm_insert_epi32: s2",s2);

  __m128i r = _mm_and_si128(s1,s2);
  dump("_mm_and_si128: r:",r);
  __m128i r2 = _mm_cmpeq_epi32(r,s2);
  dump("_mm_cmpeq_epi32:r:",r2);
  __m128i r3 = _mm_sad_epu8(s1,s2);
  dump("_mm_sad_epu8:r:",r3);

  // requires SSSE3 aka -mssse3
#ifndef _WIN32
  __m128i r4 = _mm_hadd_epi32(s1,s2);
  dump("_mm_hadd_epi32:r:",r4);
  _mm_stream_si128(&s1,_mm_set_epi32(-1,-1,-1,-1));
  _mm_stream_si128(&s2,_mm_set_epi32(-1,-1,-1,-1));
  /// SSSE3 Only!
  __m128i r5 = _mm_hadd_epi32(s1,s2);
  dump("_mm_hadd_epi32:r:",r5);
#endif
  return 1;
}

