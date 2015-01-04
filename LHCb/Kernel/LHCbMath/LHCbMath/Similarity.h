#include "GaudiKernel/GenericMatrixTypes.h"
#include "GaudiKernel/SymmetricMatrixTypes.h"

namespace LHCb { namespace Math {

// Forward declare pointer-to-worker-function type
using similarity_t = void (*)(const double* Ci, const double* Fi, double* ti);

// the actual function to call -- will be adjusted on first invocation to 
// a version which matches the architect we're running on...

extern  similarity_t similarity_5_5;
extern  similarity_t similarity_5_7;

// trampoline functions --- these do a 'vtbl'-like function dispatch
// to the correct implementation, and adapt the arguments so that only
// 'POD types' are used...

// perform similarity transform -- 
// 'target' and 'origin' are NOT allowed to be the same object.
// (actually, in the SSE3 and AVX versions that are, but not in
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

} }
