#ifndef LHCB_MATH_SIMILARITY_HH
#define LHCB_MATH_SIMILARITY_HH
#include "GaudiKernel/GenericVectorTypes.h"
#include "GaudiKernel/GenericMatrixTypes.h"
#include "GaudiKernel/SymmetricMatrixTypes.h"

namespace LHCb { namespace Math {

// Forward declare pointer-to-worker-function type
using similarity_t = void (*)(const double* Ci, const double* Fi, double* ti);

// the actual function to call -- will be adjusted on first invocation to 
// a version which matches the supported instruction set we're running on...

extern  similarity_t similarity_5_1;
extern  similarity_t similarity_5_5;
extern  similarity_t similarity_5_7;


// trampoline functions --- these do a 'vtbl'-like function dispatch
// to the correct implementation, and adapt the arguments so that only
// 'POD types' are used...

// perform similarity transform -- 
inline double Similarity( const Gaudi::Vector5& F,
                          const Gaudi::SymMatrix5x5& origin )
{
    double target;
    (*similarity_5_1)( origin.Array(), F.Array(), &target );
    return target;
}

inline double Similarity( const Gaudi::Matrix1x5& F,
                          const Gaudi::SymMatrix5x5& origin )
{
    double target;
    (*similarity_5_1)( origin.Array(), F.Array(), &target );
    return target;
}

// 'target' and 'origin' are NOT allowed to be the same object.
// (actually, in the SSE3 and AVX versions it is possible, but not in
// the generic version -- hence this could be a very confusing bug)
inline void Similarity( const Gaudi::Matrix5x5& F,
                        const Gaudi::SymMatrix5x5& origin,
                        Gaudi::SymMatrix5x5& target ) 
{
    (*similarity_5_5)( origin.Array(), F.Array(), target.Array() );
}

inline Gaudi::SymMatrix5x5 Similarity( const Gaudi::Matrix5x5& F,
                                       const Gaudi::SymMatrix5x5& origin )
{
    Gaudi::SymMatrix5x5 target;
    Similarity( F, origin, target );
    return target; // rely on RVO to make this efficient...
}

// 'target' and 'origin' are NOT allowed to be the same object.
// (actually, in the SSE3 and AVX versions it is possible, but not in
// the generic version -- hence this could be a very confusing bug)
inline void Similarity( const ROOT::Math::SMatrix<double,7,5>& F,
                        const Gaudi::SymMatrix5x5& origin,
                        Gaudi::SymMatrix7x7& target ) 
{
    (*similarity_5_7)( origin.Array(), F.Array(), target.Array() );
}

inline Gaudi::SymMatrix7x7 Similarity( const ROOT::Math::SMatrix<double,7,5>& F,
                                       const Gaudi::SymMatrix5x5& origin )
{
    Gaudi::SymMatrix7x7 target;
    Similarity( F, origin, target );
    return target; // rely on RVO to make this efficient...
}




// Forward declare pointer-to-worker-function type
using average_t = bool (*)(const double* X1, const double* C1,
                           const double* X2, const double* C2,
                           double *X, double *C );

// the actual function to call -- will be adjusted on first invocation to 
// a version which matches the supported instruction set we're running on...

extern  average_t average;

inline bool Average( const Gaudi::Vector5& X1, const Gaudi::SymMatrix5x5& C1,
                     const Gaudi::Vector5& X2, const Gaudi::SymMatrix5x5& C2,
                     Gaudi::Vector5& X, Gaudi::SymMatrix5x5& C ) 
{
// 
//  template<class Vector, class SymMatrix>
//  bool weightedAverage( const Vector& X1, const SymMatrix& C1,
//                        const Vector& X2, const SymMatrix& C2,
//                        Vector& X, SymMatrix& C )
//  {
//    static SymMatrix invR;
//    static ROOT::Math::SMatrix<double, Vector::kSize, Vector::kSize> K ;
//    invR = C1 + C2 ;
//    bool success = invR.InvertChol() ;
//    K = C1 * invR ;
//    X = X1 + K*(X2 - X1) ;
//    ROOT::Math::AssignSym::Evaluate(C, K*C2 ) ; 
//    return success ;
//  }
//    // the following used to be more stable, but isn't any longer, it seems:
//    //ROOT::Math::AssignSym::Evaluate(C, -2 * K * C1) ;
//    //C += C1 + ROOT::Math::Similarity(K,R) ;
//
    return (*average)( X1.Array(), C1.Array(), 
                       X2.Array(), C2.Array(), 
                       X.Array(),  C.Array() );
}

// Forward declare pointer-to-worker-function type
using filter_t = double (*)( double* X, double* C,
                             const double* Xref, const double* H,
                             double refResidual, double errorMeas2 );


extern filter_t filter;

// compute a filter step in a Kalman filter
// updates X and C in situ, and returns the chisquared
inline double Filter( Gaudi::Vector5& X, Gaudi::SymMatrix5x5& C,
                      const Gaudi::Vector5& Xref, 
                      const Gaudi::Matrix1x5& H,
                      double refResidual, double errorMeas2 )
{
//      // calculate the linearized residual of the prediction and its error
//      double res = refResidual + ( H * (Xref - X) ) (0) ;
//
//      static SMatrix<double,5,1> CHT, K ;
//      CHT = C * Transpose(H) ;
//      double errorRes2  = errorMeas2 + (H*CHT)(0,0) ;  
//      // calculate gain matrix K
//      K = CHT / errorRes2;
//      
//      // update the state vector
//      X += K.Col(0) * res ;
//      
//      // update the covariance matrix
//      static TrackSymMatrix tmp ;
//      ROOT::Math::AssignSym::Evaluate(tmp, K * Transpose(CHT) ) ;
//      C -= tmp ;
//      return res*res / errorRes2;

    return (*filter)(X.Array(),C.Array(),
                     Xref.Array(),H.Array(),
                     refResidual,errorMeas2);
}


} }
#endif
