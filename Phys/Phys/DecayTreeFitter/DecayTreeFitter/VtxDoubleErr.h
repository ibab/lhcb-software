// 	$Id: VtxDoubleErr.h,v 1.1 2009-05-07 13:02:10 wouter Exp $
//
//--------------------------------------------------------------------------
// Author List:
//	Abi Soffer              (Original author)
//
//--------------------------------------------------------------------------
// Copyright Information:
//	Copyright (C) 1998	Colorado State University
//
//==========================================================================

#ifndef BBRDOUBLEERR_HH
#define BBRDOUBLEERR_HH

#include <cmath>
#include <iostream>

class BbrDoubleErr 
{
public:

  // Constructors: The m_covariance and the m_value are 0.0 unless specified:
  BbrDoubleErr() :                       m_value(0.0), m_covariance(0.0) {}
  BbrDoubleErr(double val) :             m_value(val), m_covariance(0.0) {}
  BbrDoubleErr(double val, double cov) : m_value(val), m_covariance(cov) {}

  // Destructor
  ~BbrDoubleErr() {}

  // Accessors (const)
  double value()                    const {return m_value;}
  double covariance()               const {return m_covariance;}

  BbrDoubleErr operator-();   // value() -> -value(), covariance() unaffected
  BbrDoubleErr operator - (const BbrDoubleErr &);
  BbrDoubleErr operator + (const BbrDoubleErr &);

  // NOTE: (a * b).covariance() is 
  // b^2 * a.covariance() + a^2 * b.covariance()
  BbrDoubleErr operator * (const BbrDoubleErr &);

  // NOTE: (a / b).covariance() is 
  // a.covariance() / b^2 + b.covariance() * a^2 / b^4
  BbrDoubleErr operator / (const BbrDoubleErr &);

  // modifiers:
  void setValue(double val)      {m_value = val;}
  void setCovariance(double cov) {m_covariance = cov;}

  BbrDoubleErr & operator += (const BbrDoubleErr &);
  BbrDoubleErr & operator -= (const BbrDoubleErr &);
  BbrDoubleErr & operator *= (const BbrDoubleErr &);
  BbrDoubleErr & operator /= (const BbrDoubleErr &);

private:
  // Data members
  double m_value;
  double m_covariance;
};


//--------------------------------------------------------------------
// unary -:
inline BbrDoubleErr BbrDoubleErr::operator-() {
  return BbrDoubleErr(-m_value, m_covariance);
}

//--------------------------------------------------------------------
inline BbrDoubleErr BbrDoubleErr::operator - (const BbrDoubleErr & bde) {
  return BbrDoubleErr(m_value - bde.m_value, m_covariance - bde.m_covariance);
}

//--------------------------------------------------------------------
inline BbrDoubleErr BbrDoubleErr::operator + (const BbrDoubleErr & bde) {
  return BbrDoubleErr(m_value + bde.m_value, m_covariance + bde.m_covariance);
}

//--------------------------------------------------------------------
inline BbrDoubleErr BbrDoubleErr::operator * (const BbrDoubleErr & bde) {
  return BbrDoubleErr(m_value * bde.m_value, 
		      m_covariance * bde.m_value * bde.m_value +
		      bde.m_covariance * m_value * m_value);
}

//--------------------------------------------------------------------
inline BbrDoubleErr BbrDoubleErr::operator / (const BbrDoubleErr & bde) {
  register double bde2 = bde.m_value * bde.m_value;

  return BbrDoubleErr(m_value / bde.m_value,
		      m_covariance / bde2 +
		      bde.m_covariance * m_value * m_value / (bde2 * bde2));
}		      

//-------------
// Modifiers --
//-------------
 
inline BbrDoubleErr & BbrDoubleErr::operator += (const BbrDoubleErr & bde) {
  *this = *this + bde;
  return *this;
}

//-----------------------------------------------------------------------
inline BbrDoubleErr & BbrDoubleErr::operator -= (const BbrDoubleErr & bde) {
  *this = *this - bde;
  return *this;
}

//-----------------------------------------------------------------------
inline BbrDoubleErr & BbrDoubleErr::operator *= (const BbrDoubleErr & bde) {
  *this = *this * bde;
  return *this;
}

//-----------------------------------------------------------------------
inline BbrDoubleErr & BbrDoubleErr::operator /= (const BbrDoubleErr & bde) {
  *this = *this / bde;
  return *this;
}

inline ostream & operator<<(ostream & stream, const BbrDoubleErr & bde) {
  stream << "value: " << bde.value() 
	 << " covariance: " << bde.covariance();
  return stream;
}

#endif
