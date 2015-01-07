#pragma GCC optimize "O3"
#pragma GCC target   "avx"
#include "GaudiKernel/GenericVectorTypes.h"
#include "GaudiKernel/GenericMatrixTypes.h"
#include "GaudiKernel/SymmetricMatrixTypes.h"
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
namespace similarity_5_avx {

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
}

namespace average_avx {

bool average( const double* X1, const double* C1,
              const double* X2, const double* C2,
              double* X, double* C ) 
{
      static int i=-1;
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

      double K[25];
      auto _1 = _invR.c0i<0,1,3,6,10>(C1);
      auto _2 = _invR.c0i<1,2,4,7,11>(C1);
      auto _3 = _invR.c0i<3,4,5,8,12>(C1);
      auto _4 = _invR.c0i<6,7,8,9,13>(C1);
      K[ 0] = _1[0];
      K[ 1] = _1[1];
      K[ 2] = _1[2];
      K[ 3] = _1[3];

      K[ 5] = _2[0];
      K[ 6] = _2[1];
      K[ 7] = _2[2];
      K[ 8] = _2[3];

      K[10] = _3[0];
      K[11] = _3[1];
      K[12] = _3[2];
      K[13] = _3[3];

      K[15] = _4[0];
      K[16] = _4[1];
      K[17] = _4[2];
      K[18] = _4[3];

      // TODO: blend&permute it! (4x2 instructions)
      // 0 1 2 3 | 0 4 2 6 | 0 4 8 c | 0 4 8 c |
      // 4 5 6 7 | 1 5 3 7 | 2 6 a e | 1 5 9 d |
      // 8 9 a b | 8 c a e | 1 5 b f | 2 6 a e |
      // c d e f | 9 d b f | 3 7 9 d | 3 7 b f |

      auto kc0 = __m256d{_1[0],_2[0],_3[0],_4[0]};
      auto kc1 = __m256d{_1[1],_2[1],_3[1],_4[1]};
      auto kc2 = __m256d{_1[2],_2[2],_3[2],_4[2]};
      auto kc3 = __m256d{_1[3],_2[3],_3[3],_4[3]};

      auto _ = _invR.c0i<10,11,12,13,14>(C1);
      K[20] = _[0];
      K[21] = _[1];
      K[22] = _[2];
      K[23] = _[3];

      avx_5_t _C1(C1);
      auto kc4 = _C1.c0i<10,11,12,13,14>(invR);
      K[ 4] = kc4[0];
      K[ 9] = kc4[1];
      K[14] = kc4[2];
      K[19] = kc4[3];

      K[24] = C1[10]*invR[10] + C1[11]*invR[11] + C1[12]*invR[12] + C1[13]*invR[13] + C1[14]*invR[14];

      // X <- X1 + C1*inverse(C1+C2)*(X2-X1) =  X1 + K*(X2-X1) = X1 + K*d
      auto _x20 = _mm256_loadu_pd(X2); 
      auto _x10 = _mm256_loadu_pd(X1); 
      auto d0 = _x20-_x10; double d4 = X2[4]-X1[4];

      _ = _x10 + kc0*d0[0]+kc1*d0[1]+kc2*d0[2]+kc3*d0[3]+kc4*d4 ;
      X[0] = _[0];
      X[1] = _[1];
      X[2] = _[2];
      X[3] = _[3];
      X[4] = X1[4] + K[20]*d0[0] + K[21]*d0[1] + K[22]*d0[2] + K[23]*d0[3] + K[24]*d4;

      // C <-  C1 * inverse(C1+C2)  * C2 =  K * C2
      avx_5_t _C2(C2);
      _ = kc0*_C2.c0 + kc1*_C2.c1 + kc2*_C2.c2+kc3*_C2.c3+kc4*_C2.c4;
      C[ 0] = _[0];
      C[ 2] = _[1];
      C[ 5] = _[2];
      C[ 9] = _[3];

      C[10] = K[20]*C2[ 0] + K[21]*C2[ 1] + K[22]*C2[ 3] + K[23]*C2[ 6] + K[24]*C2[10]; 
      C[ 1] = K[ 5]*C2[ 0] + K[ 6]*C2[ 1] + K[ 7]*C2[ 3] + K[ 8]*C2[ 6] + K[ 9]*C2[10];
      C[ 3] = K[10]*C2[ 0] + K[11]*C2[ 1] + K[12]*C2[ 3] + K[13]*C2[ 6] + K[14]*C2[10];
      C[ 6] = K[15]*C2[ 0] + K[16]*C2[ 1] + K[17]*C2[ 3] + K[18]*C2[ 6] + K[19]*C2[10];

      C[ 8] = K[15]*C2[ 3] + K[16]*C2[ 4] + K[17]*C2[ 5] + K[18]*C2[ 8] + K[19]*C2[12];
      C[12] = K[20]*C2[ 3] + K[21]*C2[ 4] + K[22]*C2[ 5] + K[23]*C2[ 8] + K[24]*C2[12]; 
      C[13] = K[20]*C2[ 6] + K[21]*C2[ 7] + K[22]*C2[ 8] + K[23]*C2[ 9] + K[24]*C2[13]; 
      C[14] = K[20]*C2[10] + K[21]*C2[11] + K[22]*C2[12] + K[23]*C2[13] + K[24]*C2[14]; 

      C[ 4] = K[10]*C2[ 1] + K[11]*C2[ 2] + K[12]*C2[ 4] + K[13]*C2[ 7] + K[14]*C2[11];
      C[ 7] = K[15]*C2[ 1] + K[16]*C2[ 2] + K[17]*C2[ 4] + K[18]*C2[ 7] + K[19]*C2[11];
      C[11] = K[20]*C2[ 1] + K[21]*C2[ 2] + K[22]*C2[ 4] + K[23]*C2[ 7] + K[24]*C2[11]; 

      // the following used to be more stable, but isn't any longer, it seems:
      //ROOT::Math::AssignSym::Evaluate(C, -2 * K * C1) ;
      //C += C1 + ROOT::Math::Similarity(K,R) ;
      return success;
}



}
}
}
