#ifndef LHCBMATH_SYMMATRIXINVERTER_H
#define LHCBMATH_SYMMATRIXINVERTER_H

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
     *  routines for symmetric matrices
     */
    namespace SymMatrixInverter
    {
      /// inverts a symmetric matrix (dimension n, general, implementation)
      template<class T> struct inverterForAllN
      {
	typedef typename T::value_type F;
	inline bool operator()(T& m, int N) const
	{
	  // perform variant cholesky decomposition of matrix: M = L O L^T
	  // L is lower triangular matrix with positive eigenvalues, thus same
	  // use as in proper cholesky decomposition
	  // O is diagonal, elements are either +1 or -1, to account for potential
	  // negative eigenvalues of M (name is o because it contains "ones")
	  // only thing that can go wrong: trying to divide by zero on diagonale
	  // (matrix is singular in this case)

	  // we only need N * (N + 1) / 2 elements to store L
	  // element L(i,j) is at array position (i * (i + 1)) / 2 + j
	  F l[(N * (N + 1)) / 2];
	  // sign on diagonale
	  F o[N];

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
		tmp -= base1[k] * base2[k] * o[k];
	      base1[j] = tmp *= base2[j] * o[j];
	      // keep track of contribution to element on diagonale
	      tmpdiag -= tmp * tmp * o[j];
	    }
	    // keep truncation error small
	    tmpdiag = m(i, i) + tmpdiag;
	    // save sign to have L positive definite
	    if (tmpdiag < F(0)) {
	      tmpdiag = -tmpdiag;
	      o[i] = F(-1);
	    } else {
	      // tmpdiag has right sign
	      o[i] = F(1);
	    }
	    if (tmpdiag == F(0)) return false;
	    else base1[i] = std::sqrt(F(1) / tmpdiag);
	  }

	  // ok, next step: invert off-diagonal part of matrix L
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

	  // Li = L^(-1) formed, now calculate M^(-1) = Li^T O Li
	  for (int i = N; i--; ) {
	    for (int j = i + 1; j--; ) {
	      F tmp = F(0);
	      base1 = &l[(N * (N - 1)) / 2];
	      for (int k = N; k-- > i; base1 -= k)
		tmp += base1[i] * base1[j] * o[k];
	      m(i,j) = tmp;
	    }
	  }

	  return true;
	}
      };

      /// inverts a symmetric matrix (dimension n, general case)
      template<class T, class F, int N> struct inverter
      {
	inline bool operator()(T& m) const
	{ return inverterForAllN<T>()(m, T::kRows); }
      };

      /// inverts a symmetric matrix (dimension 6)
      template<class T, class F> struct inverter<T,F,6>
      {
	inline bool operator()(T& m) const
	{
	  F li11 = m(0,0), o1 = F(1);
	  if (li11 < F(0)) li11 = -li11, o1 = -o1;
	  if (li11 == F(0)) return false;
	  else li11 = std::sqrt(F(1) / li11);

	  const F l21 = m(1,0) * li11 * o1;
	  F li22 = m(1,1) - l21 * l21 * o1, o2 = F(1);
	  if (li22 < F(0)) li22 = -li22, o2 = -o2;
	  if (li22 == F(0)) return false;
	  else li22 = std::sqrt(F(1) / li22);

	  const F l31 = m(2,0) * li11 * o1;
	  const F l32 = (m(2,1) - l21 * l31 * o1) * li22 * o2;

	  F li33 = m(2,2) - (l31 * l31 * o1 + l32 * l32 * o2), o3 = F(1);
	  if (li33 < F(0)) li33 = -li33, o3 = -o3;
	  if (li33 == F(0)) return false;
	  else li33 = std::sqrt(F(1) / li33);

	  const F l41 = m(3,0) * li11 * o1;
	  const F l42 = (m(3,1) - l21 * l41 * o1) * li22 * o2;
	  const F l43 = (m(3,2) - l31 * l41 * o1 - l32 * l42 * o2) * li33 * o3;

	  F li44 = m(3,3) - (l41 * l41 * o1 + l42 * l42 * o2 + l43 * l43 * o3),
	    o4 = F(1);
	  if (li44 < F(0)) li44 = -li44, o4 = -o4;
	  if (li44 == F(0)) return false;
	  else li44 = std::sqrt(F(1) / li44);

	  const F l51 = m(4,0) * li11 * o1;
	  const F l52 = (m(4,1) - l21 * l51 * o1) * li22 * o2;
	  const F l53 = (m(4,2) - l31 * l51 * o1 - l32 * l52 * o2) * li33 * o3;
	  const F l54 = (m(4,3) - l41*l51*o1 - l42*l52*o2 - l43*l53*o3) * li44*o4;

	  F li55 = m(4,4) - (l51*l51*o1+l52*l52*o2+l53*l53*o3+l54*l54*o4), o5=F(1);
	  if (li55 < F(0)) li55 = -li55, o5 = -o5;
	  if (li55 == F(0)) return false;
	  else li55 = std::sqrt(F(1) / li55);

	  const F l61 = m(5,0) * li11 * o1;
	  const F l62 = (m(5,1) - l21 * l61 * o1) * li22 * o2;
	  const F l63 = (m(5,2) - l31 * l61 * o1 - l32 * l62 * o2) * li33 * o3;
	  const F l64 = (m(5,3) - l41*l61*o1-l42*l62*o2-l43*l63*o3)*li44*o4;
	  const F l65=(m(5,4)-l51*l61*o1-l52*l62*o2-l53*l63*o3-l54*l64*o4)*li55*o5;

	  F li66 = m(5,5)-(l61*l61*o1+l62*l62*o2+l63*l63*o3+l64*l64*o4+l65*l65*o5),
	    o6 = F(1);
	  if (li66 < F(0)) li66 = -li66, o6 = -o6;
	  if (li66 == F(0)) return false;
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
	      l64*l43*l32*li44*li33 - l65*l53*l32*li55*li33 - l65*l54*l42*li55*li44 +
	      l63*l32*li33 + l64*l42*li44 + l65*l52*li55 - l62) * li22 * li66;
	  const F li61 = (-l65*l54*l43*l32*l21*li22*li33*li44*li55 +
	      l64*l43*l32*l21*li22*li33*li44 + l65*l53*l32*l21*li22*li33*li55 +
	      l65*l54*l42*l21*li22*li44*li55 + l65*l54*l43*l31*li33*li44*li55 -
	      l63*l32*l21*li22*li33 - l64*l42*l21*li22*li44 - l65*l52*l21*li22*li55 -
	      l64*l43*l31*li33*li44 - l65*l53*l31*li33*li55 - l65*l54*l41*li44*li55 +
	      l62*l21*li22 + l63*l31*li33 + l64*l41*li44 + l65*l51*li55 - l61) *
	    li11 * li66;

	  m(0,0)=li61*li61*o6+li51*li51*o5+li41*li41*o4+li31*li31*o3+li21*li21*o2+
	    li11*li11*o1;
	  m(1,0)=li61*li62*o6+li51*li52*o5+li41*li42*o4+li31*li32*o3+li21*li22*o2;
	  m(1,1)=li62*li62*o6+li52*li52*o5+li42*li42*o4+li32*li32*o3+li22*li22*o2;
	  m(2,0)=li61*li63*o6+li51*li53*o5+li41*li43*o4+li31*li33*o3;
	  m(2,1)=li62*li63*o6+li52*li53*o5+li42*li43*o4+li32*li33*o3;
	  m(2,2)=li63*li63*o6+li53*li53*o5+li43*li43*o4+li33*li33*o3;
	  m(3,0)=li61*li64*o6+li51*li54*o5+li41*li44*o4;
	  m(3,1)=li62*li64*o6+li52*li54*o5+li42*li44*o4;
	  m(3,2)=li63*li64*o6+li53*li54*o5+li43*li44*o4;
	  m(3,3)=li64*li64*o6+li54*li54*o5+li44*li44*o4;
	  m(4,0)=li61*li65*o6+li51*li55*o5;
	  m(4,1)=li62*li65*o6+li52*li55*o5;
	  m(4,2)=li63*li65*o6+li53*li55*o5;
	  m(4,3)=li64*li65*o6+li54*li55*o5;
	  m(4,4)=li65*li65*o6+li55*li55*o5;
	  m(5,0)=li61*li66*o6;
	  m(5,1)=li62*li66*o6;
	  m(5,2)=li63*li66*o6;
	  m(5,3)=li64*li66*o6;
	  m(5,4)=li65*li66*o6;
	  m(5,5)=li66*li66*o6;

	  return true;
	}
      };

      /// inverts a symmetric matrix (dimension 5)
      template<class T, class F> struct inverter<T,F,5>
      {
	inline bool operator()(T& m) const
	{
	  F li11 = m(0,0), o1 = F(1);
	  if (li11 < F(0)) li11 = -li11, o1 = -o1;
	  if (li11 == F(0)) return false;
	  else li11 = std::sqrt(F(1) / li11);

	  const F l21 = m(1,0) * li11 * o1;
	  F li22 = m(1,1) - l21 * l21 * o1, o2 = F(1);
	  if (li22 < F(0)) li22 = -li22, o2 = -o2;
	  if (li22 == F(0)) return false;
	  else li22 = std::sqrt(F(1) / li22);

	  const F l31 = m(2,0) * li11 * o1;
	  const F l32 = (m(2,1) - l21 * l31 * o1) * li22 * o2;

	  F li33 = m(2,2) - (l31 * l31 * o1 + l32 * l32 * o2), o3 = F(1);
	  if (li33 < F(0)) li33 = -li33, o3 = -o3;
	  if (li33 == F(0)) return false;
	  else li33 = std::sqrt(F(1) / li33);

	  const F l41 = m(3,0) * li11 * o1;
	  const F l42 = (m(3,1) - l21 * l41 * o1) * li22 * o2;
	  const F l43 = (m(3,2) - l31 * l41 * o1 - l32 * l42 * o2) * li33 * o3;

	  F li44 = m(3,3) - (l41 * l41 * o1 + l42 * l42 * o2 + l43 * l43 * o3),
	    o4 = F(1);
	  if (li44 < F(0)) li44 = -li44, o4 = -o4;
	  if (li44 == F(0)) return false;
	  else li44 = std::sqrt(F(1) / li44);

	  const F l51 = m(4,0) * li11 * o1;
	  const F l52 = (m(4,1) - l21 * l51 * o1) * li22 * o2;
	  const F l53 = (m(4,2) - l31 * l51 * o1 - l32 * l52 * o2) * li33 * o3;
	  const F l54 = (m(4,3) - l41*l51*o1 - l42*l52*o2 - l43*l53*o3) * li44*o4;

	  F li55 = m(4,4) - (l51*l51*o1+l52*l52*o2+l53*l53*o3+l54*l54*o4), o5=F(1);
	  if (li55 < F(0)) li55 = -li55, o5 = -o5;
	  if (li55 == F(0)) return false;
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

	  m(0,0)=li51*li51*o5+li41*li41*o4+li31*li31*o3+li21*li21*o2+li11*li11*o1;
	  m(1,0)=li51*li52*o5+li41*li42*o4+li31*li32*o3+li21*li22*o2;
	  m(1,1)=li52*li52*o5+li42*li42*o4+li32*li32*o3+li22*li22*o2;
	  m(2,0)=li51*li53*o5+li41*li43*o4+li31*li33*o3;
	  m(2,1)=li52*li53*o5+li42*li43*o4+li32*li33*o3;
	  m(2,2)=li53*li53*o5+li43*li43*o4+li33*li33*o3;
	  m(3,0)=li51*li54*o5+li41*li44*o4;
	  m(3,1)=li52*li54*o5+li42*li44*o4;
	  m(3,2)=li53*li54*o5+li43*li44*o4;
	  m(3,3)=li54*li54*o5+li44*li44*o4;
	  m(4,0)=li51*li55*o5;
	  m(4,1)=li52*li55*o5;
	  m(4,2)=li53*li55*o5;
	  m(4,3)=li54*li55*o5;
	  m(4,4)=li55*li55*o5;

	  return true;
	}
      };

      /// inverts a symmetric matrix (dimension 4)
      template<class T, class F> struct inverter<T,F,4>
      {
	inline bool operator()(T& m) const
	{
	  F li11 = m(0,0), o1 = F(1);
	  if (li11 < F(0)) li11 = -li11, o1 = -o1;
	  if (li11 == F(0)) return false;
	  else li11 = std::sqrt(F(1) / li11);

	  const F l21 = m(1,0) * li11 * o1;
	  F li22 = m(1,1) - l21 * l21 * o1, o2 = F(1);
	  if (li22 < F(0)) li22 = -li22, o2 = -o2;
	  if (li22 == F(0)) return false;
	  else li22 = std::sqrt(F(1) / li22);

	  const F l31 = m(2,0) * li11 * o1;
	  const F l32 = (m(2,1) - l21 * l31 * o1) * li22 * o2;

	  F li33 = m(2,2) - (l31 * l31 * o1 + l32 * l32 * o2), o3 = F(1);
	  if (li33 < F(0)) li33 = -li33, o3 = -o3;
	  if (li33 == F(0)) return false;
	  else li33 = std::sqrt(F(1) / li33);

	  const F l41 = m(3,0) * li11 * o1;
	  const F l42 = (m(3,1) - l21 * l41 * o1) * li22 * o2;
	  const F l43 = (m(3,2) - l31 * l41 * o1 - l32 * l42 * o2) * li33 * o3;

	  F li44 = m(3,3) - (l41 * l41 * o1 + l42 * l42 * o2 + l43 * l43 * o3),
	    o4 = F(1);
	  if (li44 < F(0)) li44 = -li44, o4 = -o4;
	  if (li44 == F(0)) return false;
	  else li44 = std::sqrt(F(1) / li44);

	  const F li21 = -l21 * li11 * li22;
	  const F li32 = -l32 * li22 * li33;
	  const F li31 = (l21 * l32 * li22 - l31) * li11 * li33;
	  const F li43 = -l43 * li44 * li33;
	  const F li42 = (l32 * l43 * li33 - l42) * li22 * li44;
	  const F li41 = (-l21 * l32 * l43 * li22 * li33 +
	      l21 * l42 * li22 + l31 * l43 * li33 - l41) * li11 * li44;

	  m(0,0)=li41*li41*o4+li31*li31*o3+li21*li21*o2+li11*li11*o1;
	  m(1,0)=li41*li42*o4+li31*li32*o3+li21*li22*o2;
	  m(1,1)=li42*li42*o4+li32*li32*o3+li22*li22*o2;
	  m(2,0)=li41*li43*o4+li31*li33*o3;
	  m(2,1)=li42*li43*o4+li32*li33*o3;
	  m(2,2)=li43*li43*o4+li33*li33*o3;
	  m(3,0)=li41*li44*o4;
	  m(3,1)=li42*li44*o4;
	  m(3,2)=li43*li44*o4;
	  m(3,3)=li44*li44*o4;

	  return true;
	}
      };

      /// inverts a symmetric matrix (dimension 3)
      template<class T, class F> struct inverter<T,F,3>
      {
	inline bool operator()(T& m) const
	{
	  F li11 = m(0,0), o1 = F(1);
	  if (li11 < F(0)) li11 = -li11, o1 = -o1;
	  if (li11 == F(0)) return false;
	  else li11 = std::sqrt(F(1) / li11);

	  const F l21 = m(1,0) * li11 * o1;
	  F li22 = m(1,1) - l21 * l21 * o1, o2 = F(1);
	  if (li22 < F(0)) li22 = -li22, o2 = -o2;
	  if (li22 == F(0)) return false;
	  else li22 = std::sqrt(F(1) / li22);

	  const F l31 = m(2,0) * li11 * o1;
	  const F l32 = (m(2,1) - l21 * l31 * o1) * li22 * o2;

	  F li33 = m(2,2) - (l31 * l31 * o1 + l32 * l32 * o2), o3 = F(1);
	  if (li33 < F(0)) li33 = -li33, o3 = -o3;
	  if (li33 == F(0)) return false;
	  else li33 = std::sqrt(F(1) / li33);

	  const F li21 = -l21 * li11 * li22;
	  const F li32 = -l32 * li22 * li33;
	  const F li31 = (l21 * l32 * li22 - l31) * li11 * li33;

	  m(0,0)=li31*li31*o3+li21*li21*o2+li11*li11*o1;
	  m(1,0)=li31*li32*o3+li21*li22*o2;
	  m(1,1)=li32*li32*o3+li22*li22*o2;
	  m(2,0)=li31*li33*o3;
	  m(2,1)=li32*li33*o3;
	  m(2,2)=li33*li33*o3;

	  return true;
	}
      };

      /// inverts a symmetric matrix (dimension 2)
      template<class T, class F> struct inverter<T,F,2>
      {
	inline bool operator()(T& m) const
	{
	  F li11 = m(0,0), o1 = F(1);
	  if (li11 < F(0)) li11 = -li11, o1 = -o1;
	  if (li11 == F(0)) return false;
	  else li11 = std::sqrt(F(1) / li11);

	  const F l21 = m(1,0) * li11 * o1;
	  F li22 = m(1,1) - l21 * l21 * o1, o2 = F(1);
	  if (li22 < F(0)) li22 = -li22, o2 = -o2;
	  if (li22 == F(0)) return false;
	  else li22 = std::sqrt(F(1) / li22);

	  const F li21 = -l21 * li11 * li22;

	  m(0,0)=li21*li21*o2+li11*li11*o1;
	  m(1,0)=li21*li22*o2;
	  m(1,1)=li22*li22*o2;

	  return true;
	}
      };

      /// inverts a symmetric matrix (dimension 1)
      template<class T, class F> struct inverter<T,F,1>
      {
	inline bool operator()(T& m) const
	{
	  if (std::abs(m(0,0)) == F(0)) return false;
	  m(0,0) = F(1) / m(0,0);
	  return true;
	}
      };

      /// refuses to invert a symmetric matrix of dimension 0
      template<class T, class F> struct inverter<T,F,0>
      {
	private:
	  // make operator private and abstract to force compile time error
	  virtual bool operator()(T& m) const = 0;
      };
    }
  }
}

#endif // LHCBMATH_SYMMATRIXINVERTER_H

// vim:sw=2:tw=78:ft=cpp
