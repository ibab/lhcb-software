#include "Event/TrackTypes.h"

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

inline void Similarity( const Gaudi::TrackMatrix& F,
                        const Gaudi::TrackSymMatrix& origin,
                        Gaudi::TrackSymMatrix& target ) 
{
      return (*similarity_5_5)( origin.Array(), F.Array(), target.Array() );
}

inline void Similarity( const ROOT::Math::SMatrix<double,7,5>& F,
                        const Gaudi::TrackSymMatrix& origin,
                        Gaudi::SymMatrix7x7& target ) 
{
      return (*similarity_5_7)( origin.Array(), F.Array(), target.Array() );
}

} }
