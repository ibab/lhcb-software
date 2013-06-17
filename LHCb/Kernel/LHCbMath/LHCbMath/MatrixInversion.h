#ifndef LHCBMATH_MATRIXINVERSION_H
#define LHCBMATH_MATRIXINVERSION_H

// ============================================================================
// ROOT matrix types
// ============================================================================
#include "GaudiKernel/SymmetricMatrixTypes.h"

// ============================================================================
// matrix inversion implementation
// ============================================================================
#include "LHCbMath/SymMatrixInverter.h"

// ============================================================================
/** @file
 * Provides and interface to matrix inversion routines for a pos-def symmetric
 * matrices. Also provides an alternative to the standard ROOT inversion
 * routine for the general symmetric case.
 * 
 * @author Manuel Schiller
 * @date Aug 29 2008
 **/
namespace Gaudi
{
  namespace Math 
  {
    /** invert symmetric positive definite matrices
     *
     *  For matrix dimensions N <= 6, hand-optimized template specializations
     *  exist.
     *
     *  A Cholesky decomposition is used. For real M = M^T non-singular, one can
     *  write M = L L^T, where L is a lower triangular with positive elements on
     *  the diagonale.
     *
     *  Except for very small matrices (about 2x2 and below), this implementation
     *  is more accurate than the matrix inversion routine that comes with ROOT
     *  for SMatrix's symmetric positive definite case.
     *
     *  Because of the way Cholesky decomposition works, prescaling matrix
     *  entries to improve condition number is usually not neccessary (the
     *  decomposition involves steps which effectively amount to applying
     *  such a prescaling).
     *
     *  Starting with matrix sizes of about 4x4 upward, this routine is faster
     *  than SMatrix's built-in Invert() for the symmetric case. Again,
     *  benchmark before starting to believe!
     *
     *  Nowadays, this is part of ROOT, so we just call SMatrix::InvertChol...
     *
     *  @param matrix symmetric positive definite matrix
     *  @return true if inversion was sucessful, false otherwise
     */
    //    template<class T> inline bool invertPosDefSymMatrix(T& matrix)
    //    {
    //      /* make sure we break in the typedefs below if the user calls this
    //       * method with an unsuitable type T */
    //      enum { N = T::kRows };
    //      typedef typename T::value_type F;
    //      typedef typename ROOT::Math::MatRepSym<F, N> R;
    //      typedef typename ROOT::Math::SMatrix<F, N, N, R> M;
    //      return matrix.InvertChol();
    //    }
    /** invert symmetric matrices
     *
     *  For matrix dimensions N <= 6, hand-optimized template specializations
     *  exist.
     *
     *  A variant of Cholesky decomposition is used. For real M = M^T
     *  non-singular, one can write M = L O L^T, where L is a lower triangular
     *  with positive elements on the diagonale and O is a diagonal matrix with
     *  entries +1 or -1, depending on the sign of the eigenvalues of M.
     *
     *  Except for very small matrices (about 2x2 and below), this implementation
     *  can be more accurate than the matrix inversion routine that comes with
     *  ROOT for SMatrix's symmetric case, provided that the elements on the
     *  diagonale dominate in magnitude (i.e. the contribution to L(i,i) from
     *  M(i,i) should be the largest contribution in magnitude). In any case, you
     *  should check which routine works best for you.
     *
     *  Because of the way Cholesky decomposition works, prescaling matrix
     *  entries to improve condition number is usually not neccessary (the
     *  decomposition involves steps which effectively amount to applying
     *  such a prescaling).
     *
     *  Starting with matrix sizes of about 5x5 upward, this routine is faster
     *  that SMatrix's built-in Invert() for the symmetric case. Again,
     *  benchmark before starting to believe!
     *
     *  @param matrix symmetric matrix to invert
     *  @return true if inversion was sucessful, false otherwise
     */
    template<class T> inline bool invertSymMatrix(T& matrix)
    {
      enum { N = T::kRows };
      typedef typename T::value_type F;
      typedef typename ROOT::Math::MatRepSym<F, N> R;
      typedef typename ROOT::Math::SMatrix<F, N, N, R> M;
      return Gaudi::Math::SymMatrixInverter::inverter<M, F, N>()(matrix);
    }
  }
}

#endif // LHCBMATH_MATRIXINVERSION_H

// vim:sw=2:tw=78:ft=cpp
