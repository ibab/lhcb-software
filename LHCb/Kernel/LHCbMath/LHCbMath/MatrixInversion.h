#ifndef LHCBMATH_MATRIXINVERSION_H
#define LHCBMATH_MATRIXINVERSION_H

// ============================================================================
// ROOT
// ============================================================================
#include "GaudiKernel/SymmetricMatrixTypes.h"

// ============================================================================
/** @file
 * Implements matrix inversion for a pos-def symmetric
 * matrix. Currently only implemented for a 5x5 matrix. 
 * 
 * Author: Manuel Schiller
 **/

namespace Gaudi
{
  namespace Math 
  {
    // ========================================================================
    /** Inverts a pos-def symmetrix matrix using cholesky
     * decomposition of matrix: M = L L^T. Returns false if the matrix
     * has zero or negative eigenvalues. If the matrix is very
     * ill-conditioned, one may want to do pivoting.
     *
     * For now only implemented for a 5x5 matrix, but rest can follow
     * if needed.
     *
     *  @param m      symmetric matrix, to be modified
     *  @return true if success, false otherwise
     *  @author Manuel Schiller
     *  @date 2008-07-14
     **/
    inline bool invertPosDefSymMatrix( Gaudi::SymMatrix5x5& m )
    {
      // perform cholesky decomposition of matrix: M = L L^T
      // only thing that can go wrong: trying to take square root of negative
      // number or zero (matrix is ill-conditioned or singular in these cases)
      //
      // if matrix is very very very ill-conditioned, one may want to do pivoting
      //
      // quirk: we need to invert L later anyway, so we can invert elements on
      // diagonale straight away (we only ever need their reciprocals!)
      if (m(0,0) <= 0.0) return false;
      double li11 = std::sqrt(1. / m(0,0));
      double l21 = m(1,0) * li11;
      double li22 = m(1,1) - l21 * l21;
      if (li22 <= 0.0) return false;
      else li22 = std::sqrt(1. / li22);
      double l31 = m(2,0) * li11;
      double l32 = (m(2,1) - l21 * l31) * li22;
      double li33 = m(2,2) - l31 * l31 - l32 * l32;
      if (li33 <= 0.0) return false;
      else li33 = std::sqrt(1. / li33);
      double l41 = m(3,0) * li11;
      double l42 = (m(3,1) - l21 * l41) * li22;
      double l43 = (m(3,2) - l31 * l41 - l32 * l42) * li33;
      double li44 = m(3,3) - l41 * l41 - l42 * l42 - l43 * l43;
      if (li44 <= 0.0) return false;
      else li44 = std::sqrt(1. / li44);
      double l51 = m(4,0) * li11;
      double l52 = (m(4,1) - l21 * l51) * li22;
      double l53 = (m(4,2) - l31 * l51 - l32 * l52) * li33;
      double l54 = (m(4,3) - l41 * l51 - l42 * l52 - l43 * l53) * li44;
      double li55 = m(4,4) - l51*l51-l52*l52-l53*l53-l54*l54;
      if (li55 <= 0.0) return false;
      else li55 = std::sqrt(1. / li55);
      
      // invert lower triangular matrix L: Li = L^(-1)
      // already inverted elements on diagonale above
      double li21 = -l21 * li11 * li22;
      double li32 = -l32 * li22 * li33;
      double li31 = (l21 * l32 * li22 - l31) * li11 * li33;
      double li43 = -l43 * li44 * li33;
      double li42 = (l32 * l43 * li33 - l42) * li22 * li44;
      double li41 = (-l21 * l32 * l43 * li22 * li33 +
		     l21 * l42 * li22 + l31 * l43 * li33 - l41) * li11 * li44;
      double li54 = -l54 * li55 * li44;
      double li53 = (l54 * l43 * li44 - l53) * li33 * li55;
      double li52 = (-l32 * l43 * l54 * li33 * li44 +
		     l32 * l53 * li33 + l42 * l54 * li44 - l52) * li22 * li55;
      double li51 = (l21*l32*l43*l54*li22*li33*li44 -
		     l54*l43*l31*li44*li33 - l53*l32*l21*li22*li33 - l54*l42*l21*li44*li22 +
		     l52*l21*li22 + l53*l31*li33 + l54*l41*li44 -l51) * li11 * li55;
      
      // inverse of M: M^(-1) = Li^T Li, put result into m
      m(0,0) = li51*li51 + li41*li41 + li31*li31 + li21*li21 + li11*li11;
      m(1,0) = li51*li52 + li41*li42 + li31*li32 + li21*li22;
      m(1,1) = li52*li52 + li42*li42 + li32*li32 + li22*li22;
      m(2,0) = li51*li53 + li41*li43 + li31*li33;
      m(2,1) = li52*li53 + li42*li43 + li32*li33;
      m(2,2) = li53*li53 + li43*li43 + li33*li33;
      m(3,0) = li51*li54 + li41*li44;
      m(3,1) = li52*li54 + li42*li44;
      m(3,2) = li53*li54 + li43*li44;
      m(3,3) = li54*li54 + li44*li44;
      m(4,0) = li51*li55;
      m(4,1) = li52*li55;
      m(4,2) = li53*li55;
      m(4,3) = li54*li55;
      m(4,4) = li55*li55;
      
      return true;
    }
  }
}

#endif
