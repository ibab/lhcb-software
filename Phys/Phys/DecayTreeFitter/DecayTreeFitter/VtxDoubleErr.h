// 	$Id: VtxDoubleErr.h,v 1.4 2009-05-12 12:07:17 wouter Exp $
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

namespace LHCb
{

  class VtxDoubleErr 
  {
  public:

    // Constructors: The m_covariance and the m_value are 0.0 unless specified:
    VtxDoubleErr() :                       m_value(0.0), m_covariance(0.0) {}
    VtxDoubleErr(double val) :             m_value(val), m_covariance(0.0) {}
    VtxDoubleErr(double val, double cov) : m_value(val), m_covariance(cov) {}
    
    // Destructor
    ~VtxDoubleErr() {}
    
    // Accessors (const)
    double value()      const {return m_value;}
    double covariance() const {return m_covariance;}
    
    VtxDoubleErr operator-();   // value() -> -value(), covariance() unaffected
    VtxDoubleErr operator - (const VtxDoubleErr &);
    VtxDoubleErr operator + (const VtxDoubleErr &);
    
    // scale 
    VtxDoubleErr operator * (double);

    // NOTE: (a * b).covariance() is 
    // b^2 * a.covariance() + a^2 * b.covariance()
    VtxDoubleErr operator * (const VtxDoubleErr &);
    
    // NOTE: (a / b).covariance() is 
    // a.covariance() / b^2 + b.covariance() * a^2 / b^4
    VtxDoubleErr operator / (const VtxDoubleErr &);
    
    // modifiers:
    void setValue(double val)      {m_value = val;}
    void setCovariance(double cov) {m_covariance = cov;}
    
    VtxDoubleErr & operator += (const VtxDoubleErr &);
    VtxDoubleErr & operator -= (const VtxDoubleErr &);
    VtxDoubleErr & operator *= (const VtxDoubleErr &);
    VtxDoubleErr & operator /= (const VtxDoubleErr &);

    double pull( double truth) const { return (m_value - truth)/std::sqrt(m_covariance) ; }
    
  private:
    // Data members
    double m_value;
    double m_covariance;
  };
  
  
  //--------------------------------------------------------------------
  // unary -:
  inline VtxDoubleErr VtxDoubleErr::operator-() {
    return VtxDoubleErr(-m_value, m_covariance);
  }
  
  //--------------------------------------------------------------------
  inline VtxDoubleErr VtxDoubleErr::operator - (const VtxDoubleErr & bde) {
    return VtxDoubleErr(m_value - bde.m_value, m_covariance - bde.m_covariance);
  }
  
  //--------------------------------------------------------------------
  inline VtxDoubleErr VtxDoubleErr::operator + (const VtxDoubleErr & bde) {
    return VtxDoubleErr(m_value + bde.m_value, m_covariance + bde.m_covariance);
  }
  
  //--------------------------------------------------------------------
  inline VtxDoubleErr VtxDoubleErr::operator * (const VtxDoubleErr & bde) {
    return VtxDoubleErr(m_value * bde.m_value, 
			m_covariance * bde.m_value * bde.m_value +
			bde.m_covariance * m_value * m_value);
  }

  //--------------------------------------------------------------------
  inline VtxDoubleErr VtxDoubleErr::operator * (double s) {
    return VtxDoubleErr(m_value * s, 
			m_covariance * s * s ) ;
  }
  
  //--------------------------------------------------------------------
  inline VtxDoubleErr VtxDoubleErr::operator / (const VtxDoubleErr & bde) {
    register double bde2 = bde.m_value * bde.m_value;
    
    return VtxDoubleErr(m_value / bde.m_value,
			m_covariance / bde2 +
			bde.m_covariance * m_value * m_value / (bde2 * bde2));
  }		      
  
  //-------------
  // Modifiers --
  //-------------
  
  inline VtxDoubleErr & VtxDoubleErr::operator += (const VtxDoubleErr & bde) {
    *this = *this + bde;
    return *this;
  }
  
  //-----------------------------------------------------------------------
  inline VtxDoubleErr & VtxDoubleErr::operator -= (const VtxDoubleErr & bde) {
    *this = *this - bde;
    return *this;
  }
  
  //-----------------------------------------------------------------------
  inline VtxDoubleErr & VtxDoubleErr::operator *= (const VtxDoubleErr & bde) {
    *this = *this * bde;
    return *this;
  }
  
  //-----------------------------------------------------------------------
  inline VtxDoubleErr & VtxDoubleErr::operator /= (const VtxDoubleErr & bde) {
    *this = *this / bde;
    return *this;
  }
  
  inline std::ostream & operator<<(std::ostream & stream, const VtxDoubleErr & bde) {
    stream << "value: " << bde.value() 
	   << " covariance: " << bde.covariance();
    return stream;
  }
}

#endif
