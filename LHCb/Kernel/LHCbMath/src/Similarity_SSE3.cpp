
#pragma GCC optimize "O3"
#include "LHCbMath/Similarity.h"
#pragma GCC target "sse3"

#include <x86intrin.h>

namespace LHCb {
namespace Math {


// origin: 5x5 input symmetric matrix, in row-major version,i.e.
//  1
//  2  3
//  4  5  6
//  7  8  9 10
// 11 12 13 14 15
// F: transformation, row-major
// ti: output 5x5 matrix: F * origin * Transpose(F)

inline double dot5_sse3(const double* f, __m128d  r0, __m128d  r1, double r2) {
    auto r = r0*f[0]+r1*f[2];
    return _mm_hadd_pd(r,r)[0]+r2*f[4];
}

// reshuffle the origin matrix for SIMD use...
struct alignas(16) sse_t {
    __m128d r0,r10,r2,r12,r4,r14,r6,r16,r8,r18,r24;
    sse_t(const double* d) :
      r0  { d[ 0], d[ 1] }, r10 { d[ 3], d[ 6] },
      r2  { d[ 1], d[ 2] }, r12 { d[ 4], d[ 7] },
      r4  { d[ 3], d[ 4] }, r14 { d[ 5], d[ 8] },
      r6  { d[ 6], d[ 7] }, r16 { d[ 8], d[ 9] },
      r8  { d[10], d[11] }, r18 { d[12], d[13] }, r24 { d[14], 0. }
    { };
 inline __m128d  g0(const double* f ) const { 
      return r0*f[0]  + r2*f[1] + r4*f[2] + r6*f[3] + r8*f[4] ;
 }
 inline __m128d  g2(const double* f ) const { 
      return r10*f[0] + r12*f[1] + r14*f[2] + r16*f[3] + r18*f[4] ;
 }
 inline double g4(const double *f) const {
     return dot5_sse3( f, r8, r18, r24[0] );
 }
};

void similarity_5_5_sse3(const double* Ci, const double* Fi, double* ti)  {
      // std::cout << "using similarity_5_5_sse3" << std::endl;
      // reshuffle the origin matrix for SIMD use...
      sse_t m { Ci };
     
      auto _0 = m.g0(Fi);
      auto _2 = m.g2(Fi);
      auto _4 = m.g4(Fi);
      ti[ 0] = dot5_sse3(Fi   ,_0,_2,_4);
      ti[ 1] = dot5_sse3(Fi+ 5,_0,_2,_4);
      ti[ 3] = dot5_sse3(Fi+10,_0,_2,_4);
      ti[ 6] = dot5_sse3(Fi+15,_0,_2,_4);
      ti[10] = dot5_sse3(Fi+20,_0,_2,_4);
      _0 = m.g0(Fi+5);
      _2 = m.g2(Fi+5);
      _4 = m.g4(Fi+5);
      ti[ 2] = dot5_sse3(Fi+ 5,_0,_2,_4);
      ti[ 4] = dot5_sse3(Fi+10,_0,_2,_4);
      ti[ 7] = dot5_sse3(Fi+15,_0,_2,_4);
      ti[11] = dot5_sse3(Fi+20,_0,_2,_4);
      _0 = m.g0(Fi+10);
      _2 = m.g2(Fi+10);
      _4 = m.g4(Fi+10);
      ti[ 5] = dot5_sse3(Fi+10,_0,_2,_4);
      ti[ 8] = dot5_sse3(Fi+15,_0,_2,_4);
      ti[12] = dot5_sse3(Fi+20,_0,_2,_4);
      _0 = m.g0(Fi+15);
      _2 = m.g2(Fi+15);
      _4 = m.g4(Fi+15);
      ti[ 9] = dot5_sse3(Fi+15,_0,_2,_4);
      ti[13] = dot5_sse3(Fi+20,_0,_2,_4);
      _0 = m.g0(Fi+20);
      _2 = m.g2(Fi+20);
      _4 = m.g4(Fi+20);
      ti[14] = dot5_sse3(Fi+20,_0,_2,_4);
}
void similarity_5_7_sse3(const double* Ci, const double* Fi, double* Ti)  {

      // reshuffle the 5x5 symmetric Ci matrix for SIMD use...
      sse_t m { Ci };
     
      auto _0 = m.g0(Fi);
      auto _2 = m.g2(Fi);
      auto _4 = m.g4(Fi);
      Ti[ 0] = dot5_sse3(Fi   ,_0,_2,_4);
      Ti[ 1] = dot5_sse3(Fi+ 5,_0,_2,_4);
      Ti[ 3] = dot5_sse3(Fi+10,_0,_2,_4);
      Ti[ 6] = dot5_sse3(Fi+15,_0,_2,_4);
      Ti[10] = dot5_sse3(Fi+20,_0,_2,_4);
      Ti[15] = dot5_sse3(Fi+25,_0,_2,_4);
      Ti[21] = dot5_sse3(Fi+30,_0,_2,_4);
      _0 = m.g0(Fi+5);
      _2 = m.g2(Fi+5);
      _4 = m.g4(Fi+5);
      Ti[ 2] = dot5_sse3(Fi+ 5,_0,_2,_4);
      Ti[ 4] = dot5_sse3(Fi+10,_0,_2,_4);
      Ti[ 7] = dot5_sse3(Fi+15,_0,_2,_4);
      Ti[11] = dot5_sse3(Fi+20,_0,_2,_4);
      Ti[16] = dot5_sse3(Fi+25,_0,_2,_4);
      Ti[22] = dot5_sse3(Fi+30,_0,_2,_4);
      _0 = m.g0(Fi+10);
      _2 = m.g2(Fi+10);
      _4 = m.g4(Fi+10);
      Ti[ 5] = dot5_sse3(Fi+10,_0,_2,_4);
      Ti[ 8] = dot5_sse3(Fi+15,_0,_2,_4);
      Ti[12] = dot5_sse3(Fi+20,_0,_2,_4);
      Ti[17] = dot5_sse3(Fi+25,_0,_2,_4);
      Ti[23] = dot5_sse3(Fi+30,_0,_2,_4);
      _0 = m.g0(Fi+15);
      _2 = m.g2(Fi+15);
      _4 = m.g4(Fi+15);
      Ti[ 9] = dot5_sse3(Fi+15,_0,_2,_4);
      Ti[13] = dot5_sse3(Fi+20,_0,_2,_4);
      Ti[18] = dot5_sse3(Fi+25,_0,_2,_4);
      Ti[24] = dot5_sse3(Fi+30,_0,_2,_4);
      _0 = m.g0(Fi+20);
      _2 = m.g2(Fi+20);
      _4 = m.g4(Fi+20);
      Ti[14] = dot5_sse3(Fi+20,_0,_2,_4);
      Ti[19] = dot5_sse3(Fi+25,_0,_2,_4);
      Ti[25] = dot5_sse3(Fi+30,_0,_2,_4);
      _0 = m.g0(Fi+25);
      _2 = m.g2(Fi+25);
      _4 = m.g4(Fi+25);
      Ti[20] = dot5_sse3(Fi+25,_0,_2,_4);
      Ti[26] = dot5_sse3(Fi+30,_0,_2,_4);
      _0 = m.g0(Fi+30);
      _2 = m.g2(Fi+30);
      _4 = m.g4(Fi+30);
      Ti[27] = dot5_sse3(Fi+30,_0,_2,_4);
}
}
}
