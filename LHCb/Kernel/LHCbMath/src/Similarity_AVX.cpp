#pragma GCC optimize "O3"
#pragma GCC target   "avx"

#include "LHCbMath/Similarity.h"
#include <x86intrin.h>

namespace LHCb {
namespace Math {



#if 0
 dot_2_4() {
__m256d xy = _mm256_mul_pd( x, y );
__m256d zw = _mm256_mul_pd( z, w );
__m256d temp = _mm256_hadd_pd( xy, zw );
__m128d hi128 = _mm256_extractf128_pd( temp, 1 );
__m128d dotproduct = _mm_add_pd( (__m128d)temp, hi128 );
}

 dot_4_4() { // compute 4 dot products of 4-vectors

__m256d xy0 = _mm256_mul_pd( x[0], y[0] );
__m256d xy1 = _mm256_mul_pd( x[1], y[1] );
__m256d xy2 = _mm256_mul_pd( x[2], y[2] );
__m256d xy3 = _mm256_mul_pd( x[3], y[3] );

// low to high: xy00+xy01 xy10+xy11 xy02+xy03 xy12+xy13
__m256d temp01 = _mm256_hadd_pd( xy0, xy1 );   

// low to high: xy20+xy21 xy30+xy31 xy22+xy23 xy32+xy33
__m256d temp23 = _mm256_hadd_pd( xy2, xy3 );

// low to high: xy02+xy03 xy12+xy13 xy20+xy21 xy30+xy31
__m256d swapped = _mm256_permute2f128_pd( temp01, temp23, 0x21 );

// low to high: xy00+xy01 xy10+xy11 xy22+xy23 xy32+xy33
__m256d blended = _mm256_blend_pd(temp01, temp23, 0b1100);

__m256d dotproduct = _mm256_add_pd( swapped, blended );

 }

#endif

inline double dot5_avx(const double* f, __m256d  r0,  double r2) {
        auto t = r0*_mm256_loadu_pd(f);
        t = _mm256_hadd_pd( t, t ); 
        return t[0]+t[2]+r2*f[4];
        // auto s = _mm256_permute2f128_pd(t,t,0x01);
        // auto s = __mm256d{ t[2],t[3],t[0],t[1] } ; 
        // return (s+t)[0] + r2*f[4];
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
    { // TODO: can we avoid reading the upper triangle in c0..c3,
      //       fill it with zero, and then 'permute' them
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

void similarity_5_5_avx(const double* Ci, const double* Fi, double* Ti)  {

      // reshuffle the Ci matrix for SIMD use...
      avx_5_t m { Ci };
     
      auto _0 = m.c0i(Fi);
      auto _4 = m.c4i(Fi);
      Ti[ 0] = dot5_avx(Fi   ,_0,_4);
      Ti[ 1] = dot5_avx(Fi+ 5,_0,_4);
      Ti[ 3] = dot5_avx(Fi+10,_0,_4);
      Ti[ 6] = dot5_avx(Fi+15,_0,_4);
      Ti[10] = dot5_avx(Fi+20,_0,_4);
      _0 = m.c0i(Fi+5);
      _4 = m.c4i(Fi+5);
      Ti[ 2] = dot5_avx(Fi+ 5,_0,_4);
      Ti[ 4] = dot5_avx(Fi+10,_0,_4);
      Ti[ 7] = dot5_avx(Fi+15,_0,_4);
      Ti[11] = dot5_avx(Fi+20,_0,_4);
      _0 = m.c0i(Fi+10);
      _4 = m.c4i(Fi+10);
      Ti[ 5] = dot5_avx(Fi+10,_0,_4);
      Ti[ 8] = dot5_avx(Fi+15,_0,_4);
      Ti[12] = dot5_avx(Fi+20,_0,_4);
      _0 = m.c0i(Fi+15);
      _4 = m.c4i(Fi+15);
      Ti[ 9] = dot5_avx(Fi+15,_0,_4);
      Ti[13] = dot5_avx(Fi+20,_0,_4);
      _0 = m.c0i(Fi+20);
      _4 = m.c4i(Fi+20);
      Ti[14] = dot5_avx(Fi+20,_0,_4);
}

void similarity_5_7_avx(const double* Ci, const double* Fi, double* Ti)  {

      // reshuffle the 5x5 symmetric Ci matrix for SIMD use...
      avx_5_t  m { Ci };
     
      auto _0 = m.c0i(Fi);
      auto _4 = m.c4i(Fi);
      //TODO: switch to snippet above which computes 4 dot products at the same time....
      Ti[ 0] = dot5_avx(Fi   ,_0,_4);
      Ti[ 1] = dot5_avx(Fi+ 5,_0,_4);
      Ti[ 3] = dot5_avx(Fi+10,_0,_4);
      Ti[ 6] = dot5_avx(Fi+15,_0,_4);
      Ti[10] = dot5_avx(Fi+20,_0,_4);
      Ti[15] = dot5_avx(Fi+25,_0,_4);
      Ti[21] = dot5_avx(Fi+30,_0,_4);
      _0 = m.c0i(Fi+5);
      _4 = m.c4i(Fi+5);
      Ti[ 2] = dot5_avx(Fi+ 5,_0,_4);
      Ti[ 4] = dot5_avx(Fi+10,_0,_4);
      Ti[ 7] = dot5_avx(Fi+15,_0,_4);
      Ti[11] = dot5_avx(Fi+20,_0,_4);
      Ti[16] = dot5_avx(Fi+25,_0,_4);
      Ti[22] = dot5_avx(Fi+30,_0,_4);
      _0 = m.c0i(Fi+10);
      _4 = m.c4i(Fi+10);
      Ti[ 5] = dot5_avx(Fi+10,_0,_4);
      Ti[ 8] = dot5_avx(Fi+15,_0,_4);
      Ti[12] = dot5_avx(Fi+20,_0,_4);
      Ti[17] = dot5_avx(Fi+25,_0,_4);
      Ti[23] = dot5_avx(Fi+30,_0,_4);
      _0 = m.c0i(Fi+15);
      _4 = m.c4i(Fi+15);
      Ti[ 9] = dot5_avx(Fi+15,_0,_4);
      Ti[13] = dot5_avx(Fi+20,_0,_4);
      Ti[18] = dot5_avx(Fi+25,_0,_4);
      Ti[24] = dot5_avx(Fi+30,_0,_4);
      _0 = m.c0i(Fi+20);
      _4 = m.c4i(Fi+20);
      Ti[14] = dot5_avx(Fi+20,_0,_4);
      Ti[19] = dot5_avx(Fi+25,_0,_4);
      Ti[25] = dot5_avx(Fi+30,_0,_4);
      _0 = m.c0i(Fi+25);
      _4 = m.c4i(Fi+25);
      Ti[20] = dot5_avx(Fi+25,_0,_4);
      Ti[26] = dot5_avx(Fi+30,_0,_4);
      _0 = m.c0i(Fi+30);
      _4 = m.c4i(Fi+30);
      Ti[27] = dot5_avx(Fi+30,_0,_4);
}
}
}
