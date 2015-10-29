#include "GaudiKernel/GenericVectorTypes.h"
#include "GaudiKernel/GenericMatrixTypes.h"
#include "GaudiKernel/SymmetricMatrixTypes.h"
#include "VectorClass/vectorclass.h"
#include <x86intrin.h>

namespace {

  struct avx_guard {
    // see Agner Fog's optimization guide, 12.1 about mixing AVX and non-AVX code,
    // (http://www.agner.org/optimize/optimizing_cpp.pdf)
    // and preserving the YMM register state.
    // Invoking __mm256_zeroupper seems to reduce the overhead when switching.
    ~avx_guard() { _mm256_zeroupper(); }
  };

  inline double dot5_avx(Vec4d f0, double f1, Vec4d  r0,  double r1) {
    return horizontal_add(r0*f0)+r1*f1;
  }

  inline double dot5_avx(const double* f, Vec4d  r0,  double r2) {
    return dot5_avx( Vec4d{}.load(f), f[4], r0, r2 );
  }

  inline Vec2d dots2_5_avx(const double* f, Vec4d r0, double r2) {
    auto x0 = r0*Vec4d{}.load(f);
    auto x1 = r0*Vec4d{}.load(f+5);
    auto temp = Vec4d{_mm256_hadd_pd( x0, x1 )};
    return temp.get_low() + temp.get_high() + gather2d<4,9>(f)*r2 ;
  }

  inline Vec4d madd4_5_avx(Vec4d f0, Vec4d f1, Vec4d f2, Vec4d f3, Vec4d f4, Vec4d r0, double r4  ) {
    return f0*r0[0] + f1*r0[1] + f2*r0[2] + f3*r0[3] + f4*r4;
  }

  inline Vec4d dots4_5_avx(const double* f, Vec4d r0, double r4) {
    auto x0 = r0*Vec4d{}.load(f);
    auto x1 = r0*Vec4d{}.load(f+5);
    auto x2 = r0*Vec4d{}.load(f+10);
    auto x3 = r0*Vec4d{}.load(f+15);
    auto temp01 = Vec4d{_mm256_hadd_pd( x0, x1 )};
    auto temp23 = Vec4d{_mm256_hadd_pd( x2, x3 )};
    return blend4d<2,3,4,5>(temp01,temp23)
         + blend4d<0,1,6,7>(temp01,temp23)
         + gather4d<4,9,14,19>(f)*r4; 
  }

  // reshuffle a symmetric, lower triangle, row major matrix for SIMD use...
  struct alignas(32) avx_5_t {
    Vec4d c0,c1,c2,c3,c4; // r4 == c4...
    double c44 ;
    avx_5_t(const double* d)  :
         c0 { gather4d<0,1,3,6>(d) },
         c1 { gather4d<1,2,4,7>(d) },
         c2 { gather4d<3,4,5,8>(d) },
         c3 { gather4d<6,7,8,9>(d) },
         c4 { gather4d<10,11,12,13>(d) }, c44{ d[14] }
      { };
      // return a column of a rhs product with column-major f (aka. tranpose of row major f)
      template<int i=0, int j=i+1, int k=j+1, int l=k+1, int m=l+1>
      inline Vec4d c0i(const double* f ) const  {
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
        avx_guard guard{};

        const avx_5_t m { Ci };
        *Ti = dot5_avx(Fi,m.c0i(Fi), m.c4i(Fi));
      }

      void similarity_5_5(const double* Ci, const double* Fi, double* Ti)  {
        avx_guard guard{};

        // reshuffle the symmetric, lower diagonal, row-major Ci matrix for SIMD use...
        const avx_5_t m { Ci };

        const auto _00 = m.c0i(Fi   ); const auto _04 = m.c4i(Fi);
        const auto _10 = m.c0i(Fi+5 ); const auto _14 = m.c4i(Fi+5);
        const auto _20 = m.c0i(Fi+10); const auto _24 = m.c4i(Fi+10);
        const auto _30 = m.c0i(Fi+15); const auto _34 = m.c4i(Fi+15);
        const auto _40 = m.c0i(Fi+20); const auto _44 = m.c4i(Fi+20);

        const auto r0 = dots4_5_avx(Fi,_00,_04);
        const auto r1 = dots4_5_avx(Fi+5,_10,_14);
        // is the layout of r2 & r3 optimal? 
        const auto r2 = Vec4d{ dots2_5_avx(Fi+10,_20,_24), dots2_5_avx(Fi+15,_30,_34)};
        const auto r3 = Vec4d{ dot5_avx(Fi+20,_20,_24), 0., dot5_avx(Fi+20,_40,_44), dot5_avx(Fi+20,_00,_04) };

        // shuffle the result so that we can store the output 'in order'
        blend4d<0,1,4,2>(r0,r1).store(Ti+0);
        blend4d<0,4,2,3>(blend4d<1,-256,7,2>(r1,r0),r2).store(Ti+4);
        blend4d<0,1,2,7>(blend4d<1,2,7,-256>(r2,r3),r1).store(Ti+8);
        blend4d<0,7,2,-256>(r3,r2).store_partial(3,Ti+12);
      }

      void similarity_5_7(const double* Ci, const double* Fi, double* Ti)  {
        avx_guard guard{};

        // reshuffle the 5x5 symmetric Ci matrix for SIMD use...
        const avx_5_t  m { Ci };

        const auto _00 = m.c0i(Fi);    const auto _04 = m.c4i(Fi);
        const auto _10 = m.c0i(Fi+5);  const auto _14 = m.c4i(Fi+5);
        const auto _20 = m.c0i(Fi+10); const auto _24 = m.c4i(Fi+10);
        const auto _30 = m.c0i(Fi+15); const auto _34 = m.c4i(Fi+15);
        const auto _40 = m.c0i(Fi+20); const auto _44 = m.c4i(Fi+20);
        const auto _50 = m.c0i(Fi+25); const auto _54 = m.c4i(Fi+25);
        const auto _60 = m.c0i(Fi+30); const auto _64 = m.c4i(Fi+30);

        const auto r0 = dots4_5_avx(Fi,_00,_04);
        const auto r1 = dots4_5_avx(Fi+5,_10,_14);
        const auto r2 = dots4_5_avx(Fi+10,_20,_24);
        const auto r3 = dots4_5_avx(Fi+15,_30,_34);
        const auto r4 = Vec4d{ dots2_5_avx(Fi+20,_40,_44), dots2_5_avx(Fi+20,_00,_04) };
        const auto r5 = Vec4d{ dots2_5_avx(Fi+25,_50,_54), dots2_5_avx(Fi+25,_10,_14) };
        const auto r6 = Vec4d{ dot5_avx(Fi+30,_00,_04),dot5_avx(Fi+30,_20,_24),dot5_avx(Fi+30,_40,_44),dot5_avx(Fi+30,_60,_64) };

        // shuffle the result so that we can store the output 'in order'
        blend4d<0,1,4,2>(r0,r1).store(Ti+0);
        blend4d<0,4,2,3>(blend4d<5,-256,3,6>(r0,r1),r2).store(Ti+4);
        blend4d<0,1,6,7>(blend4d<1,4,-256,-256>(r2,r3), blend4d<-256,-256,6,3>(r1,r4) ).store(Ti+8);
        blend4d<0,1,4,7>(blend4d<2,5,-256,-256>(r2,r3), r4).store(Ti+12);
        blend4d<0,1,6,7>(blend4d<6,3,-256,-256>(r2,r5), blend4d<-256,-256,2,5>(r3,r4)).store(Ti+16);
        blend4d<0,4,3,5>(r5,r6).store(Ti+20);
        blend4d<7,1,2,3>(blend4d<-256,6,1,7>(r5,r6),r3).store(Ti+24);
      }


      bool average( const double* X1, const double* C1,
                    const double* X2, const double* C2,
                    double* X, double* C )
      {
        avx_guard guard{};
        // compute the inverse of the covariance (i.e. weight) of the difference: R=(C1+C2)
        Gaudi::SymMatrix5x5 invRM;
        const auto y0 = Vec4d{}.load(C1)+Vec4d{}.load(C2);
        const auto y1 = Vec4d{}.load(C1+4)+Vec4d{}.load(C2+4);
        const auto y2 = Vec4d{}.load(C1+8)+Vec4d{}.load(C2+8);
        const auto y3 = Vec4d{}.load(C1+12)+Vec4d{}.load(C2+12);
        auto invR = invRM.Array();
        y0.store(invR);
        y1.store(invR+4);
        y2.store(invR+8);
        y3.store(invR+12);
        invR[14] = C1[14]+C2[14];

        bool success = invRM.InvertChol() ;
        // compute the gain matrix

        // K <- C1*inverse(C1+C2) = C1*invR
        const avx_5_t _invR(invR);

        const auto kr0 = _invR.c0i<0,1,3,6,10>(C1);
        const auto kr1 = _invR.c0i<1,2,4,7,11>(C1);
        const auto kr2 = _invR.c0i<3,4,5,8,12>(C1);
        const auto kr3 = _invR.c0i<6,7,8,9,13>(C1);
        const auto kr4 = _invR.c0i<10,11,12,13,14>(C1);

        // transpose (kr0,kr1,kr2,kr3) -> (kc0,kc1,kc2,kc3)
        const auto  _0 = blend4d<0,1,4,5>(kr0,kr2);
        const auto  _1 = blend4d<0,1,4,5>(kr1,kr3);
        const auto  _2 = blend4d<2,3,6,7>(kr0,kr2);
        const auto  _3 = blend4d<2,3,6,7>(kr1,kr3);

        const auto kc0 = blend4d<0,4,2,6>(_0,_1);
        const auto kc1 = blend4d<1,5,3,7>(_0,_1);
        const auto kc2 = blend4d<0,4,2,6>(_2,_3);
        const auto kc3 = blend4d<1,5,3,7>(_2,_3);

        const avx_5_t _C1(C1);
        const auto kc4 = _C1.c0i<10,11,12,13,14>(invR);
        const auto k44 = _C1.c4i(invR+10);

        //       kc0[0] kc1[0] kc2[0] kc3[0]  kc4[0]     kr0[0] kr0[1] kr0[2] kr0[3]  .         0  1  2  3  4
        //       kc0[1] kc1[1] kc2[1] kc3[1]  kc4[1]     kr1[0] kr1[1] kr1[2] kr1[3]  .         5  6  7  8  9
        //  K =  kc0[2] kc1[2] kc2[2] kc3[2]  kc4[2]  =  kr2[0] kr2[1] kr2[2] kr2[3]  .    =   10 11 12 13 14
        //       kc0[3] kc1[3] kc2[3] kc3[3]  kc4[3]     kr3[0] kr3[1] kr3[2] kr3[3]  .        15 16 17 18 19
        //        .      .      .      .      k44        kr4[0] kr4[1] kr4[2] kr4[3]  k44      20 21 22 23 24

        // X <- X1 + C1*inverse(C1+C2)*(X2-X1) =  X1 + K*(X2-X1) = X1 + K*d
        const auto _x20 = Vec4d{}.load(X2);
        const auto _x10 = Vec4d{}.load(X1);
        const auto d0 = _x20-_x10; double d4 = X2[4]-X1[4];

        const auto _00 = _x10 + kc0*d0[0]+kc1*d0[1]+kc2*d0[2]+kc3*d0[3]+kc4*d4 ;

        // C <-  C1 * inverse(C1+C2)  * C2 =  K * C2
        const avx_5_t _C2(C2);
        const auto r1 = kc0*_C2.c0 + kc1*_C2.c1 + kc2*_C2.c2 + kc3*_C2.c3 + kc4*_C2.c4;
        const auto r2 = madd4_5_avx(kc0,kc1,kc2,kc3,kc4, _C2.c0,_C2.c4[0]);
        const auto r4 = madd4_5_avx(kc0,kc1,kc2,kc3,kc4, _C2.c1,_C2.c4[1]);
        const auto r3 = madd4_5_avx(_C2.c0,_C2.c1,_C2.c2,_C2.c3,_C2.c4, kr4, k44);
        //TODO: can we avoid the horizontal_add by transposing the following?? or use a variant of dots2 for the last two???
        const auto r5 = Vec4d{  dot5_avx( kr3,kc4[3], _C2.c2, _C2.c4[2]),
                                dot5_avx( kr4,k44, d0, d4),
                                dot5_avx( kr4,k44, _C2.c0,_C2.c4[0] ) ,
                                dot5_avx( kr4,k44, _C2.c4,_C2.c44   ) } ; 
        _00.store(X);
        X[4] = X1[4] + r5[1]; 

        blend4d<0,5,1,6>(r1,r2).store(C+0);
        blend4d<2,5,6,3>(r4,blend4d<-256,2,7,-256>(r1,r2)).store(C+4);
        blend4d<0,5,2,7>(r5,blend4d<-256,3,-256,5>(r1,r3)).store(C+8);
        blend4d<2,3,7,-256>(r3,r5).store_partial(3,C+12);

        return success;
      }

      double filter( double* X, double* C,
                     const double* Xref, const double* H,
                     double refResidual, double errorMeas2 )
      {
        avx_guard guard{};

        const avx_5_t c(C);
        const auto cht0 = c.c0i(H);
        const auto cht4 = c.c4i(H);
        //TODO: combine
        const auto  res        = refResidual + dot5_avx(H,Vec4d{}.load(Xref)-Vec4d{}.load(X),Xref[4] - X[4]);
        const auto  errorRes2  = errorMeas2  + dot5_avx(H,cht0,cht4);

        const auto w = 1./errorRes2;
        const auto chtw0 = cht0*w;
        const auto chtw4 = cht4*w;
        // update the state vector and cov matrix
#if 1
        // keep this version for now, as it doesn't change the results...
        const auto wres = res/errorRes2;
        // const auto wres = w*res; // even this already changes the results...
        const auto x0 = Vec4d{}.load(X) + cht0*wres;
        const auto x4 = X[4]            + cht4*wres;
#else
        // this version has one division less, but produces slightly different answers... 
        // just by changing the equivalent of a*(b/c) into (a*b)/c...
        const auto x0 = Vec4d{}.load(X) + chtw0*res;
        const auto x4 = X[4]            + chtw4*res;
#endif

        const auto _0 = c.c0-cht0*chtw0[0];
        const auto _1 = blend4d<4,1,2,3>(c.c1, permute4d<1,0,2,2>(c.c4)) - Vec4d{cht4, cht0[1],cht0[2],cht0[3]} *chtw0[1];
        const auto _2 = blend4d<2,3,6,4>(c.c2,c.c4) - Vec4d{cht0[2],cht0[3],cht4,cht4}*permute4d<2,2,2,0>(chtw0);
        const auto _3 = Vec4d{c.c3[3],c.c4[3],c.c44,0.} - Vec4d{cht0[3],cht4,cht4,0.}*Vec4d{chtw0[3],chtw0[3],chtw4,0.};

        x0.store(X);
        X[4] = x4;

        blend4d<0,1,5,2>(_0,_1).store(C+0);
        blend4d<2,5,6,3>(_1,blend4d<-256,0,7,-256>(_2,_0)).store(C+4);
        blend4d<1,5,3,7>(_2,blend4d<-256,0,-256,4>(_3,_1 )).store(C+8);
        blend4d<6,1,2,-256>(_3,_2).store_partial(3,C+12);

        return w*res*res;
      }


} } }
