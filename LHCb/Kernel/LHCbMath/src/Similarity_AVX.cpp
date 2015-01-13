#pragma GCC optimize "O3"
#pragma GCC target   "avx"
#include "GaudiKernel/GenericVectorTypes.h"
#include "GaudiKernel/GenericMatrixTypes.h"
#include "GaudiKernel/SymmetricMatrixTypes.h"
#include <x86intrin.h>

namespace {

inline double dot5_avx(__m256d f0, double f1, __m256d  r0,  double r1) {
    auto t = r0*f0;
    t = _mm256_hadd_pd( t, t ); 
    return t[0]+t[2]+r1*f1;
    // auto s = _mm256_permute2f128_pd(t,t,0x01);
    // auto s = _mm256d{ t[2],t[3],t[0],t[1] } ; 
    // return (s+t)[0] + r1*f1;
}

inline double dot5_avx(const double* f, __m256d  r0,  double r2) {
    return dot5_avx( _mm256_loadu_pd(f), f[4], r0, r2 );
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
 template<int i=0, int j=i+1, int k=j+1, int l=k+1, int m=l+1>
 inline __m256d  c0i(const double* f ) const  { 
      return c0*f[i]+c1*f[j]+c2*f[k]+c3*f[l]+c4*f[m];
 }
 inline double c4i(const double *f) const  {
      return dot5_avx( f, c4, c44 );
 }
};

}


namespace LHCb {
namespace Math {
namespace avx {

    void similarity_5_1(const double* Ci, const double* Fi, double* Ti)  {
      avx_5_t m { Ci };
      *Ti= dot5_avx(Fi,m.c0i(Fi), m.c4i(Fi));
    }

    void similarity_5_5(const double* Ci, const double* Fi, double* Ti)  {

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

    void similarity_5_7(const double* Ci, const double* Fi, double* Ti)  {

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


    bool average( const double* X1, const double* C1,
                  const double* X2, const double* C2,
                  double* X, double* C ) 
    {
          // compute the inverse of the covariance (i.e. weight) of the difference: R=(C1+C2)
          static Gaudi::SymMatrix5x5 invRM;
          auto invR = invRM.Array();
          _mm256_storeu_pd(invR,    _mm256_loadu_pd(C1)   + _mm256_loadu_pd(C2));
          _mm256_storeu_pd(invR+4,  _mm256_loadu_pd(C1+4) + _mm256_loadu_pd(C2+4));
          _mm256_storeu_pd(invR+8,  _mm256_loadu_pd(C1+8) + _mm256_loadu_pd(C2+8));
          _mm_storeu_pd(   invR+12, _mm_loadu_pd(C1+12)   + _mm_loadu_pd(C2+12));
          invR[14] = C1[14]+C2[14];

          bool success = invRM.InvertChol() ;
          // compute the gain matrix

          // K <- C1*inverse(C1+C2) = C1*invR
          avx_5_t _invR(invR); 

          auto kr0 = _invR.c0i<0,1,3,6,10>(C1);
          auto kr1 = _invR.c0i<1,2,4,7,11>(C1);
          auto kr2 = _invR.c0i<3,4,5,8,12>(C1);
          auto kr3 = _invR.c0i<6,7,8,9,13>(C1);
          auto kr4 = _invR.c0i<10,11,12,13,14>(C1);

          auto _0 = _mm256_permute2f128_pd(kr0,kr2,0x20);
          auto _1 = _mm256_permute2f128_pd(kr1,kr3,0x20);
          auto kc0 = _mm256_shuffle_pd(_0,_1,0x0);   
          auto kc1 = _mm256_shuffle_pd(_0,_1,0xf);   

          auto _2 = _mm256_permute2f128_pd(kr0,kr2,0x31);
          auto _3 = _mm256_permute2f128_pd(kr1,kr3,0x31);
          auto kc2 = _mm256_shuffle_pd(_2,_3,0x0);
          auto kc3 = _mm256_shuffle_pd(_2,_3,0xf);   

          avx_5_t _C1(C1);
          auto kc4 = _C1.c0i<10,11,12,13,14>(invR);
          auto k44 = _C1.c4i(invR+10);

          //       kc0[0] kc1[0] kc2[0] kc3[0]  kc4[0]     kr0[0] kr0[1] kr0[2] kr0[3]  .         0  1  2  3  4
          //       kc0[1] kc1[1] kc2[1] kc3[1]  kc4[1]     kr1[0] kr1[1] kr1[2] kr1[3]  .         5  6  7  8  9
          //  K =  kc0[2] kc1[2] kc2[2] kc3[2]  kc4[2]  =  kr2[0] kr2[1] kr2[2] kr2[3]  .    =   10 11 12 13 14
          //       kc0[3] kc1[3] kc2[3] kc3[3]  kc4[3]     kr3[0] kr3[1] kr3[2] kr3[3]  .        15 16 17 18 19
          //        .      .      .      .      k44        kr4[0] kr4[1] kr4[2] kr4[3]  k44      20 21 22 23 24

          // X <- X1 + C1*inverse(C1+C2)*(X2-X1) =  X1 + K*(X2-X1) = X1 + K*d
          auto _x20 = _mm256_loadu_pd(X2); 
          auto _x10 = _mm256_loadu_pd(X1); 
          auto d0 = _x20-_x10; double d4 = X2[4]-X1[4];

          auto _ = _x10 + kc0*d0[0]+kc1*d0[1]+kc2*d0[2]+kc3*d0[3]+kc4*d4 ;
          X[0] = _[0];
          X[1] = _[1];
          X[2] = _[2];
          X[3] = _[3];
          X[4] = X1[4] + dot5_avx(kr4,k44,d0,d4);

          // C <-  C1 * inverse(C1+C2)  * C2 =  K * C2
          avx_5_t _C2(C2);
          _ = kc0*_C2.c0 + kc1*_C2.c1 + kc2*_C2.c2+kc3*_C2.c3+kc4*_C2.c4;
          C[ 0] = _[0];
          C[ 2] = _[1];
          C[ 5] = _[2];
          C[ 9] = _[3];

          //TODO: combine the next two...
          C[10] = kr4[0]*C2[ 0] + kr4[1]*C2[ 1] + kr4[2]*C2[ 3] + kr4[3]*C2[ 6] + k44*C2[10]; 
          _     = kc0   *C2[ 0] + kc1   *C2[ 1] + kc2   *C2[ 3] + kc3   *C2[ 6] + kc4*C2[10];
          C[ 1] = _[1];
          C[ 3] = _[2];
          C[ 6] = _[3];

          //TODO: combine the next two...
          C[14] = kr4[0]*C2[10] + kr4[1]*C2[11] + kr4[2]*C2[12] + kr4[3]*C2[13] + k44*C2[14]; 
          _     = kr4[0]*_C2.c0 + kr4[1]*_C2.c1 + kr4[2]*_C2.c2 + kr4[3]*_C2.c3 + k44*_C2.c4;
          C[11] = _[1];
          C[12] = _[2];
          C[13] = _[3];

          //TODO: combine the next two...
          C[ 8] = kr3[0]*C2[3] + kr3[1]*C2[4] + kr3[2]*C2[5] + kr3[3]*C2[8] + kc4[3]*C2[12];
          _     = kc0*   C2[1] + kc1*   C2[2] + kc2*   C2[4] + kc3*   C2[7] + kc4*   C2[11];
          C[ 4] = _[2];
          C[ 7] = _[3];


          return success;
    }

    double filter( double* X, double* C,
                   const double* Xref, const double* H,
                   double refResidual, double errorMeas2 )
    {
          auto  res = refResidual + dot5_avx(H,_mm256_loadu_pd(Xref)-_mm256_loadu_pd(X),Xref[4] - X[4]);
          avx_5_t c(C);
          auto cht0 = c.c0i(H);
          auto cht4 = c.c4i(H);
          auto  errorRes2  = errorMeas2 + dot5_avx(H,cht0,cht4);

          // update the state vector and cov matrix
          auto w = res/errorRes2;
          auto x = _mm256_loadu_pd(X) + cht0*w;
          _mm256_storeu_pd(X,x);
          X[4] += cht4 * w;

          w = 1./errorRes2;
          auto chtw0 = cht0*w;
          auto chtw4 = cht4*w;
          auto _ = c.c0-cht0*chtw0[0];
          C[ 0] = _[0];
          C[ 1] = _[1];
          C[ 3] = _[2];
          C[ 6] = _[3];

          C[10] = c.c4[0] - cht4 * chtw0[0];

          _ = _mm256_blend_pd( c.c1, _mm256_permute_pd(c.c4,1), 1 ) - __m256d{cht4, cht0[1],cht0[2],cht0[3]} *chtw0[1];
          C[11] = _[0];
          C[ 2] = _[1];
          C[ 4] = _[2];
          C[ 7] = _[3];

          _ = __m256d{c.c2[2],c.c2[3],c.c4[2],0.}-__m256d{cht0[2],cht0[3],cht4,0.}*chtw0[2];
          C[ 5] = _[0];
          C[ 8] = _[1];
          C[12] = _[2];

          _ = __m256d{c.c3[3],c.c4[3],c.c44,0. } - __m256d{cht0[3],cht4,cht4,0.}*__m256d{chtw0[3],chtw0[3],chtw4,0.};
          C[ 9] = _[0];
          C[13] = _[1];
          C[14] = _[2];

          return res*res/errorRes2;
    }


} } }
