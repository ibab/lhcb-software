#pragma GCC optimize "O3"
#pragma GCC target   "avx"

#include "LHCbMath/Similarity.h"
#include <x86intrin.h>


namespace {

inline double dot5_avx(const double* f, __m256d  r0,  double r2) {
    auto t = r0*_mm256_loadu_pd(f);
    t = _mm256_hadd_pd( t, t ); 
    return t[0]+t[2]+r2*f[4];
    // auto s = _mm256_permute2f128_pd(t,t,0x01);
    // auto s = _mm256d{ t[2],t[3],t[0],t[1] } ; 
    // return (s+t)[0] + r2*f[4];
}

inline __m128d dots2_5_avx(const double* f, __m256d r0, double r2) {
    auto x0 = r0*_mm256_loadu_pd(f);
    auto x1 = r0*_mm256_loadu_pd(f+5);
    auto temp = _mm256_hadd_pd( x0, x1 );
    auto hi128 = _mm256_extractf128_pd( temp, 1 );
    return hi128 + __m128d{temp[0],temp[1]} + __m128d{ f[4],f[9] }*r2 ;
}

inline __m256d dots4_5_avx(const double* f, __m256d r0, double r2) {
    auto x0 = r0*_mm256_loadu_pd(f);
    auto x1 = r0*_mm256_loadu_pd(f+5);
    auto x2 = r0*_mm256_loadu_pd(f+10);
    auto x3 = r0*_mm256_loadu_pd(f+15);
    auto temp01 = _mm256_hadd_pd( x0, x1 );   
    auto temp23 = _mm256_hadd_pd( x2, x3 );
    auto swapped = _mm256_permute2f128_pd( temp01, temp23, 0x21 );
    auto blended = _mm256_blend_pd(temp01, temp23, 0xC ); // 0b1100
    return swapped+blended+( __m256d{ f[4],f[9],f[14],f[19]} * r2 );
}

// reshuffle a symmetric, lower triangle, row major matrix for SIMD use...
struct alignas(16) avx_5_t {
    __m256d c0,c1,c2,c3,c4; // r4 == c4...
    double c44 ;
    avx_5_t(const double* d)  :
         c0 { d[ 0],d[ 1],d[ 3],d[ 6] },
         c1 { d[ 1],d[ 2],d[ 4],d[ 7] },
         c2 { d[ 3],d[ 4],d[ 5],d[ 8] },
         c3 { d[ 6],d[ 7],d[ 8],d[ 9] },
         c4 { d[10],d[11],d[12],d[13] }, c44{ d[14] }
    { // TODO: can we avoid reading the upper triangle in c0..c2,
      //       fill it with eg. zero, and then 'permute' them
      //       by transposing the lower triangle???
    };
 // return a column of a rhs product with column-major f (aka. tranpose of row major f)
 inline __m256d  c0i(const double* f ) const  { 
      return c0*f[0]+c1*f[1]+c2*f[2]+c3*f[3]+c4*f[4];
 }
 inline double c4i(const double *f) const  {
      return dot5_avx( f, c4, c44 );
 }
};

}


namespace LHCb {
namespace Math {

void similarity_5_1_avx(const double* Ci, const double* Fi, double* Ti)  {
      avx_5_t m { Ci };
      *Ti= dot5_avx(Fi,m.c0i(Fi), m.c4i(Fi));
}

void similarity_5_5_avx(const double* Ci, const double* Fi, double* Ti)  {

      // reshuffle the symmetric, lower diagonal, row-major Ci matrix for SIMD use...
      avx_5_t m { Ci };
     
      auto _0 = m.c0i(Fi); auto _4 = m.c4i(Fi);
      auto r = dots4_5_avx(Fi,_0,_4);
      Ti[ 0] = r[0];
      Ti[ 1] = r[1];
      Ti[ 3] = r[2];
      Ti[ 6] = r[3];
      Ti[10] = dot5_avx(Fi+20,_0,_4);
      _0 = m.c0i(Fi+5); _4 = m.c4i(Fi+5);
      r = dots4_5_avx(Fi+5,_0,_4);
      Ti[ 2] = r[0];
      Ti[ 4] = r[1];
      Ti[ 7] = r[2];
      Ti[11] = r[3];
      _0 = m.c0i(Fi+10); _4 = m.c4i(Fi+10);
      auto s = dots2_5_avx(Fi+10,_0,_4);
      Ti[ 5] = s[0];
      Ti[ 8] = s[1];
      Ti[12] = dot5_avx(Fi+20,_0,_4);
      _0 = m.c0i(Fi+15); _4 = m.c4i(Fi+15);
      s = dots2_5_avx(Fi+15,_0,_4);
      Ti[ 9] = s[0];
      Ti[13] = s[1];
      _0 = m.c0i(Fi+20); _4 = m.c4i(Fi+20);
      Ti[14] = dot5_avx(Fi+20,_0,_4);
}

void similarity_5_7_avx(const double* Ci, const double* Fi, double* Ti)  {

      // reshuffle the 5x5 symmetric Ci matrix for SIMD use...
      avx_5_t  m { Ci };
     
      auto _0 = m.c0i(Fi); auto _4 = m.c4i(Fi);
      auto r = dots4_5_avx(Fi,_0,_4);
      auto s = dots2_5_avx(Fi+20,_0,_4);
      Ti[ 0] = r[0];
      Ti[ 1] = r[1];
      Ti[ 3] = r[2];
      Ti[ 6] = r[3];
      Ti[10] = s[0];
      Ti[15] = s[1];
      Ti[21] = dot5_avx(Fi+30,_0,_4);
      _0 = m.c0i(Fi+5); _4 = m.c4i(Fi+5);
      r = dots4_5_avx(Fi+5,_0,_4);
      s = dots2_5_avx(Fi+25,_0,_4);
      Ti[ 2] = r[0];
      Ti[ 4] = r[1];
      Ti[ 7] = r[2];
      Ti[11] = r[3];
      Ti[16] = s[0];
      Ti[22] = s[1];
      _0 = m.c0i(Fi+10); _4 = m.c4i(Fi+10);
      r = dots4_5_avx(Fi+10,_0,_4);
      Ti[ 5] = r[0];
      Ti[ 8] = r[1];
      Ti[12] = r[2];
      Ti[17] = r[3];
      Ti[23] = dot5_avx(Fi+30,_0,_4);
      _0 = m.c0i(Fi+15); _4 = m.c4i(Fi+15);
      r = dots4_5_avx(Fi+15,_0,_4);
      Ti[ 9] = r[0];
      Ti[13] = r[1];
      Ti[18] = r[2];
      Ti[24] = r[3];
      _0 = m.c0i(Fi+20); _4 = m.c4i(Fi+20);
      s = dots2_5_avx(Fi+20,_0,_4);
      Ti[14] = s[0];
      Ti[19] = s[1];
      Ti[25] = dot5_avx(Fi+30,_0,_4);
      _0 = m.c0i(Fi+25); _4 = m.c4i(Fi+25);
      s = dots2_5_avx(Fi+25,_0,_4);
      Ti[20] = s[0];
      Ti[26] = s[1];
      _0 = m.c0i(Fi+30); _4 = m.c4i(Fi+30);
      Ti[27] = dot5_avx(Fi+30,_0,_4);
}
}
}
