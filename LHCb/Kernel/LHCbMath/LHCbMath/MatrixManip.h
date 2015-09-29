

// $Id$
#ifndef LHCBMATH_MATRIXMANIP_H 
#define LHCBMATH_MATRIXMANIP_H 1

// Include files

#include "Math/SMatrix.h"

/** @file
 *  
 *  Transform to and from symmetric and standard Smatrices.
 *  
 *  @author Juan Palacios
 *  @date   2006-03-28
 */
namespace Gaudi 
{
  
  namespace Math {

    using namespace ROOT::Math;

    /** Create a new symmetric matrix from the upper triangle of the input
     *  matrix.
     *
     *  @code
     *
     *  @endcode
     *
     *  @author Juan PALACIOS juan.palacios@cern.ch
     *  @date   2006-03-28
     */  
    template <typename M>  
    SMatrix<typename M::value_type, M::kRows, M::kRows, 
            MatRepSym<typename M::value_type, M::kRows> > Symmetrize(const M& rhs) {

      SMatrix<typename M::value_type, M::kRows, M::kRows, 
        MatRepSym<typename M::value_type, M::kRows> > result;
    
      for ( unsigned int i = 0; i < M::kRows; ++i ) {
        for ( unsigned int j = 0; j < M::kRows; ++j ) {
          if (i<=j) result(i,j) = rhs(i,j);
        }
      }
      return result;
    }

  }
}

#endif // LHCB_MATRIXMANIP_H
