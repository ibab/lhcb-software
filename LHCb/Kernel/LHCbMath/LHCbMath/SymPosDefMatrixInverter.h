#ifndef LHCBMATH_SYMPOSDEFMATRIXINVERTER_H
#define LHCBMATH_SYMPOSDEFMATRIXINVERTER_H

/** @file
 * header file containing the templated implementation of matrix inversion
 * routines for use with ROOT's SMatrix classes (symmetric case)
 *
 * @author Manuel Schiller
 * @date Aug 29 2008
 */
namespace Gaudi
{
  namespace Math
  {
    /** namespace to hold actual implementation of templated matrix inversion
     *  routines for symmetric positive definite matrices
     */
    namespace SymPosDefMatrixInverter
    {
      // ======================================================================
      template <class MATRIX, class F, int N> 
      struct _inverterForAllN
      {
        bool operator() ( MATRIX& m ) const 
        {
          // perform Cholesky decomposition of matrix: M = L L^T
          // only thing that can go wrong: trying to take square root of negative
          // number or zero (matrix is ill-conditioned or singular in these cases)
          
          // we only need N * (N + 1) / 2 elements to store L
          // element L(i,j) is at array position (i * (i + 1)) / 2 + j
          F l[(N * (N + 1)) / 2];
          
          // quirk: we need to invert L later anyway, so we can invert elements
          // on diagonale straight away (we only ever need their reciprocals!)
          
          // cache starting address of rows of L for speed reasons
          F *base1 = &l[0];
          for ( int i = 0; i < N; base1 += ++i) {
            F tmpdiag = F(0);	// for element on diagonale
            // calculate off-diagonal elements
            F *base2 = &l[0];
            for ( int j = 0; j < i; base2 += ++j) {
              F tmp = m(i, j);
              for (int k = j; k--; )
                tmp -= base1[k] * base2[k];
              base1[j] = tmp *= base2[j];
              // keep track of contribution to element on diagonale
              tmpdiag += tmp * tmp;
            }
            // keep truncation error small
            tmpdiag = m(i, i) - tmpdiag;
            // check if positive definite
            if (tmpdiag <= F(0)) return false;
            else base1[i] = std::sqrt(F(1) / tmpdiag);
          }
          
          // ok, next step: invert off-diagonal part of matrix
          base1 = &l[1];
          for ( int i = 1; i < N; base1 += ++i) {
            for ( int j = 0; j < i; ++j) {
              F tmp = F(0);
              F *base2 = &l[(i * (i - 1)) / 2];
              for ( int k = i; k-- > j; base2 -= k)
                tmp -= base1[k] * base2[j];
              base1[j] = tmp * base1[i];
            }
          }
          
          // Li = L^(-1) formed, now calculate M^(-1) = Li^T Li
          for (int i = N; i--; ) {
            for (int j = i + 1; j--; ) {
              F tmp = F(0);
              base1 = &l[(N * (N - 1)) / 2];
              for (int k = N; k-- > i; base1 -= k)
                tmp += base1[i] * base1[j];
              m(i,j) = tmp;
            }
          }          
          return true;
        } 
      } ;
      /// invert symmetric positive definite matrix (dimension n, general, implementation)
      template<class T> struct inverterForAllN
      {
        typedef typename T::value_type F;
        inline bool operator()(T& m, const int N) const
        {
          // perform Cholesky decomposition of matrix: M = L L^T
          // only thing that can go wrong: trying to take square root of negative
          // number or zero (matrix is ill-conditioned or singular in these cases)
          
          // we only need N * (N + 1) / 2 elements to store L
          // element L(i,j) is at array position (i * (i + 1)) / 2 + j
          F l[(N * (N + 1)) / 2];
          
          // quirk: we need to invert L later anyway, so we can invert elements
          // on diagonale straight away (we only ever need their reciprocals!)
          
          // cache starting address of rows of L for speed reasons
          F *base1 = &l[0];
          for (int i = 0; i < N; base1 += ++i) {
            F tmpdiag = F(0);	// for element on diagonale
            // calculate off-diagonal elements
            F *base2 = &l[0];
            for (int j = 0; j < i; base2 += ++j) {
              F tmp = m(i, j);
              for (int k = j; k--; )
                tmp -= base1[k] * base2[k];
              base1[j] = tmp *= base2[j];
              // keep track of contribution to element on diagonale
              tmpdiag += tmp * tmp;
            }
            // keep truncation error small
            tmpdiag = m(i, i) - tmpdiag;
            // check if positive definite
            if (tmpdiag <= F(0)) return false;
            else base1[i] = std::sqrt(F(1) / tmpdiag);
          }
          
          // ok, next step: invert off-diagonal part of matrix
          base1 = &l[1];
          for (int i = 1; i < N; base1 += ++i) {
            for (int j = 0; j < i; ++j) {
              F tmp = F(0);
              F *base2 = &l[(i * (i - 1)) / 2];
              for (int k = i; k-- > j; base2 -= k)
                tmp -= base1[k] * base2[j];
              base1[j] = tmp * base1[i];
            }
          }
          
          // Li = L^(-1) formed, now calculate M^(-1) = Li^T Li
          for (int i = N; i--; ) {
            for (int j = i + 1; j--; ) {
              F tmp = F(0);
              base1 = &l[(N * (N - 1)) / 2];
              for (int k = N; k-- > i; base1 -= k)
                tmp += base1[i] * base1[j];
              m(i,j) = tmp;
            }
          }
          
          return true;
        }
      };
      
      /// invert symmertric positive definite matrix (dimension n, general case)
      template<class T, class F, int N> struct inverter
      {
        /// dispatch to specialised implementation
        inline bool operator()(T& m) const
        // { return inverterForAllN<T>()(m, T::kRows); }
        { return _inverterForAllN<T,typename T::value_type,N>()(m); }
      };

      /// invert symmetric positive definite matrix (dimension 6)
      template<class T, class F> struct inverter<T,F,6>
      {
	inline bool operator()(T& m) const
	{
	  if (m(0,0) <= F(0)) return false;
	  const F li11 = std::sqrt(F(1) / m(0,0));
	  const F l21 = m(1,0) * li11;
	  F li22 = m(1,1) - l21 * l21;
	  if (li22 <= F(0)) return false;
	  else li22 = std::sqrt(F(1) / li22);
	  const F l31 = m(2,0) * li11;
	  const F l32 = (m(2,1) - l21 * l31) * li22;
	  F li33 = m(2,2) - (l31 * l31 + l32 * l32);
	  if (li33 <= F(0)) return false;
	  else li33 = std::sqrt(F(1) / li33);
	  const F l41 = m(3,0) * li11;
	  const F l42 = (m(3,1) - l21 * l41) * li22;
	  const F l43 = (m(3,2) - l31 * l41 - l32 * l42) * li33;
	  F li44 = m(3,3) - (l41 * l41 + l42 * l42 + l43 * l43);
	  if (li44 <= F(0)) return false;
	  else li44 = std::sqrt(F(1) / li44);
	  const F l51 = m(4,0) * li11;
	  const F l52 = (m(4,1) - l21 * l51) * li22;
	  const F l53 = (m(4,2) - l31 * l51 - l32 * l52) * li33;
	  const F l54 = (m(4,3) - l41 * l51 - l42 * l52 - l43 * l53) * li44;
	  F li55 = m(4,4) - (l51*l51+l52*l52+l53*l53+l54*l54);
	  if (li55 <= F(0)) return false;
	  else li55 = std::sqrt(F(1) / li55);
	  const F l61 = m(5,0) * li11;
	  const F l62 = (m(5,1) - l21 * l61) * li22;
	  const F l63 = (m(5,2) - l31 * l61 - l32 * l62) * li33;
	  const F l64 = (m(5,3) - l41 * l61 - l42 * l62 - l43 * l63) * li44;
	  const F l65 = (m(5,4) - l51 * l61 - l52 * l62 - l53 * l63 - l54 * l64) * li55;
	  F li66 = m(5,5) - (l61*l61+l62*l62+l63*l63+l64*l64+l65*l65);
	  if (li66 <= F(0)) return false;
	  else li66 = std::sqrt(F(1) / li66);

	  const F li21 = -l21 * li11 * li22;
	  const F li32 = -l32 * li22 * li33;
	  const F li31 = (l21 * l32 * li22 - l31) * li11 * li33;
	  const F li43 = -l43 * li44 * li33;
	  const F li42 = (l32 * l43 * li33 - l42) * li22 * li44;
	  const F li41 = (-l21 * l32 * l43 * li22 * li33 +
	      l21 * l42 * li22 + l31 * l43 * li33 - l41) * li11 * li44;
	  const F li54 = -l54 * li55 * li44;
	  const F li53 = (l54 * l43 * li44 - l53) * li33 * li55;
	  const F li52 = (-l32 * l43 * l54 * li33 * li44 +
	      l32 * l53 * li33 + l42 * l54 * li44 - l52) * li22 * li55;
	  const F li51 = (l21*l32*l43*l54*li22*li33*li44 -
	      l54*l43*l31*li44*li33 - l53*l32*l21*li22*li33 - l54*l42*l21*li44*li22 +
	      l52*l21*li22 + l53*l31*li33 + l54*l41*li44 -l51) * li11 * li55;
	  const F li65 = -l65 * li66 * li55;
	  const F li64 = (l65 * l54 * li55 - l64) * li44 * li66;
	  const F li63 = (-l43 * l54 * l65 * li44 * li55 +
	      l43 * l64 * li44 + l53 * l65 * li55 - l63) * li33 * li66;
	  const F li62 = (l32*l43*l54*l65*li33*li44*li55 -
	      l64*l43*l32*li44*li33 - l65*l53*l32*li55*li33 -l65*l54*l42*li55*li44 +
	      l63*l32*li33 + l64*l42*li44 + l65*l52*li55 - l62) * li22 * li66;
	  const F li61 = (-l65*l54*l43*l32*l21*li22*li33*li44*li55 +
	      l64*l43*l32*l21*li22*li33*li44 + l65*l53*l32*l21*li22*li33*li55 +
	      l65*l54*l42*l21*li22*li44*li55 + l65*l54*l43*l31*li33*li44*li55 -
	      l63*l32*l21*li22*li33 - l64*l42*l21*li22*li44 - l65*l52*l21*li22*li55 -
	      l64*l43*l31*li33*li44 - l65*l53*l31*li33*li55 - l65*l54*l41*li44*li55 +
	      l62*l21*li22 + l63*l31*li33 + l64*l41*li44 + l65*l51*li55 - l61) *
	    li11 * li66;

	  m(0,0) = li61*li61 + li51*li51 + li41*li41 + li31*li31 + li21*li21 + li11*li11;
	  m(1,0) = li61*li62 + li51*li52 + li41*li42 + li31*li32 + li21*li22;
	  m(1,1) = li62*li62 + li52*li52 + li42*li42 + li32*li32 + li22*li22;
	  m(2,0) = li61*li63 + li51*li53 + li41*li43 + li31*li33;
	  m(2,1) = li62*li63 + li52*li53 + li42*li43 + li32*li33;
	  m(2,2) = li63*li63 + li53*li53 + li43*li43 + li33*li33;
	  m(3,0) = li61*li64 + li51*li54 + li41*li44;
	  m(3,1) = li62*li64 + li52*li54 + li42*li44;
	  m(3,2) = li63*li64 + li53*li54 + li43*li44;
	  m(3,3) = li64*li64 + li54*li54 + li44*li44;
	  m(4,0) = li61*li65 + li51*li55;
	  m(4,1) = li62*li65 + li52*li55;
	  m(4,2) = li63*li65 + li53*li55;
	  m(4,3) = li64*li65 + li54*li55;
	  m(4,4) = li65*li65 + li55*li55;
	  m(5,0) = li61*li66;
	  m(5,1) = li62*li66;
	  m(5,2) = li63*li66;
	  m(5,3) = li64*li66;
	  m(5,4) = li65*li66;
	  m(5,5) = li66*li66;

	  return true;
	}
      };

      /// invert symmetric positive definite matrix (dimension 5)
      template<class T, class F> struct inverter<T,F,5>
      {
	inline bool operator()(T& m) const
	{
	  if (m(0,0) <= F(0)) return false;
	  const F li11 = std::sqrt(F(1) / m(0,0));
	  const F l21 = m(1,0) * li11;
	  F li22 = m(1,1) - l21 * l21;
	  if (li22 <= F(0)) return false;
	  else li22 = std::sqrt(F(1) / li22);
	  const F l31 = m(2,0) * li11;
	  const F l32 = (m(2,1) - l21 * l31) * li22;
	  F li33 = m(2,2) - (l31 * l31 + l32 * l32);
	  if (li33 <= F(0)) return false;
	  else li33 = std::sqrt(F(1) / li33);
	  const F l41 = m(3,0) * li11;
	  const F l42 = (m(3,1) - l21 * l41) * li22;
	  const F l43 = (m(3,2) - l31 * l41 - l32 * l42) * li33;
	  F li44 = m(3,3) - (l41 * l41 + l42 * l42 + l43 * l43);
	  if (li44 <= F(0)) return false;
	  else li44 = std::sqrt(F(1) / li44);
	  const F l51 = m(4,0) * li11;
	  const F l52 = (m(4,1) - l21 * l51) * li22;
	  const F l53 = (m(4,2) - l31 * l51 - l32 * l52) * li33;
	  const F l54 = (m(4,3) - l41 * l51 - l42 * l52 - l43 * l53) * li44;
	  F li55 = m(4,4) - (l51*l51+l52*l52+l53*l53+l54*l54);
	  if (li55 <= F(0)) return false;
	  else li55 = std::sqrt(F(1) / li55);

	  const F li21 = -l21 * li11 * li22;
	  const F li32 = -l32 * li22 * li33;
	  const F li31 = (l21 * l32 * li22 - l31) * li11 * li33;
	  const F li43 = -l43 * li44 * li33;
	  const F li42 = (l32 * l43 * li33 - l42) * li22 * li44;
	  const F li41 = (-l21 * l32 * l43 * li22 * li33 +
	      l21 * l42 * li22 + l31 * l43 * li33 - l41) * li11 * li44;
	  const F li54 = -l54 * li55 * li44;
	  const F li53 = (l54 * l43 * li44 - l53) * li33 * li55;
	  const F li52 = (-l32 * l43 * l54 * li33 * li44 +
	      l32 * l53 * li33 + l42 * l54 * li44 - l52) * li22 * li55;
	  const F li51 = (l21*l32*l43*l54*li22*li33*li44 -
	      l54*l43*l31*li44*li33 - l53*l32*l21*li22*li33 - l54*l42*l21*li44*li22 +
	      l52*l21*li22 + l53*l31*li33 + l54*l41*li44 -l51) * li11 * li55;

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
      };

      /// invert symmetric positive definite matrix (dimension 4)
      template<class T, class F> struct inverter<T,F,4>
      {
	inline bool operator()(T& m) const
	{
	  if (m(0,0) <= F(0)) return false;
	  const F li11 = std::sqrt(F(1) / m(0,0));
	  const F l21 = m(1,0) * li11;
	  F li22 = m(1,1) - l21 * l21;
	  if (li22 <= F(0)) return false;
	  else li22 = std::sqrt(F(1) / li22);
	  const F l31 = m(2,0) * li11;
	  const F l32 = (m(2,1) - l21 * l31) * li22;
	  F li33 = m(2,2) - (l31 * l31 + l32 * l32);
	  if (li33 <= F(0)) return false;
	  else li33 = std::sqrt(F(1) / li33);
	  const F l41 = m(3,0) * li11;
	  const F l42 = (m(3,1) - l21 * l41) * li22;
	  const F l43 = (m(3,2) - l31 * l41 - l32 * l42) * li33;
	  F li44 = m(3,3) - (l41 * l41 + l42 * l42 + l43 * l43);
	  if (li44 <= F(0)) return false;
	  else li44 = std::sqrt(F(1) / li44);

	  const F li21 = -l21 * li11 * li22;
	  const F li32 = -l32 * li22 * li33;
	  const F li31 = (l21 * l32 * li22 - l31) * li11 * li33;
	  const F li43 = -l43 * li44 * li33;
	  const F li42 = (l32 * l43 * li33 - l42) * li22 * li44;
	  const F li41 = (-l21 * l32 * l43 * li22 * li33 +
	      l21 * l42 * li22 + l31 * l43 * li33 - l41) * li11 * li44;

	  m(0,0) = li41*li41 + li31*li31 + li21*li21 + li11*li11;
	  m(1,0) = li41*li42 + li31*li32 + li21*li22;
	  m(1,1) = li42*li42 + li32*li32 + li22*li22;
	  m(2,0) = li41*li43 + li31*li33;
	  m(2,1) = li42*li43 + li32*li33;
	  m(2,2) = li43*li43 + li33*li33;
	  m(3,0) = li41*li44;
	  m(3,1) = li42*li44;
	  m(3,2) = li43*li44;
	  m(3,3) = li44*li44;

	  return true;
	}
      };

      /// invert symmetric positive definite matrix (dimension 3)
      template<class T, class F> struct inverter<T,F,3>
      {
	inline bool operator()(T& m) const
	{
	  if (m(0,0) <= F(0)) return false;
	  const F li11 = std::sqrt(F(1) / m(0,0));
	  const F l21 = m(1,0) * li11;
	  F li22 = m(1,1) - l21 * l21;
	  if (li22 <= F(0)) return false;
	  else li22 = std::sqrt(F(1) / li22);
	  const F l31 = m(2,0) * li11;
	  const F l32 = (m(2,1) - l21 * l31) * li22;
	  F li33 = m(2,2) - (l31 * l31 + l32 * l32);
	  if (li33 <= F(0)) return false;
	  else li33 = std::sqrt(F(1) / li33);

	  const F li21 = -l21 * li11 * li22;
	  const F li32 = -l32 * li22 * li33;
	  const F li31 = (l21 * l32 * li22 - l31) * li11 * li33;

	  m(0,0) = li31*li31 + li21*li21 + li11*li11;
	  m(1,0) = li31*li32 + li21*li22;
	  m(1,1) = li32*li32 + li22*li22;
	  m(2,0) = li31*li33;
	  m(2,1) = li32*li33;
	  m(2,2) = li33*li33;

	  return true;
	}
      };

      /// invert symmetric positive definite matrix (dimension 2)
      template<class T, class F> struct inverter<T,F,2>
      {
	inline bool operator()(T& m) const
	{
	  if (m(0,0) <= F(0)) return false;
	  const F li11 = std::sqrt(F(1) / m(0,0));
	  const F l21 = m(1,0) * li11;
	  F li22 = m(1,1) - l21 * l21;
	  if (li22 <= F(0)) return false;
	  else li22 = std::sqrt(F(1) / li22);

	  const F li21 = -l21 * li11 * li22;

	  m(0,0) = li21*li21 + li11*li11;
	  m(1,0) = li21*li22;
	  m(1,1) = li22*li22;

	  return true;
	}
      };

      /// invert symmetric positive definite matrix (dimension 1)
      template<class T, class F> struct inverter<T,F,1>
      {
	inline bool operator()(T& m) const
	{
	  if (m(0,0) <= F(0)) return false;
	  m(0,0) = F(1) / m(0,0);
	  return true;
	}
      };

      /// refuse to invert symmetric positive definite matrix of dimension 0
      template<class T, class F> struct inverter<T,F,0>
      {
	private:
	  // make operator private and abstract to force compile time error
	  virtual bool operator()(T& m) const = 0;
      };
    }
  }
}

#endif // LHCBMATH_SYMPOSDEFMATRIXINVERTER_H

// vim:sw=2:tw=78:ft=cpp
