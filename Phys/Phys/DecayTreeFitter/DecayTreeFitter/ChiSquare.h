#ifndef DECAYTREEFITTER_ChiSquare_H
#define DECAYTREEFITTER_ChiSquare_H

namespace DecayTreeFitter 
{

  class ChiSquare
  {
  public:

    /// Constructor
    ChiSquare(const double chi2, int ndof) : m_chi2(chi2),m_nDoF(ndof) {}
  
    /// Default Constructor
    ChiSquare() : m_chi2(0.0),
                  m_nDoF(0) {}
  
    /// Default Destructor
     ~ChiSquare() {}
  
    /// return chi2/ndof if ndof>0. returns zero otherwise.
    double chi2PerDoF() const { return m_nDoF>0 ? m_chi2/m_nDoF : 0 ; }
  
    /// addition operator
    ChiSquare& operator+=(const ChiSquare& rhs);
  
    /// subtraction operator
    ChiSquare& operator-=(const ChiSquare& rhs);
  
    /// addition operator
    ChiSquare operator+(const ChiSquare& rhs);
  
    /// subtraction operator
    ChiSquare operator-(const ChiSquare& rhs);
  
    /// Retrieve const  chi square
    double chi2() const { return m_chi2 ; }
  
    /// Retrieve const  number of degrees of freedom
    int nDoF() const { return m_nDoF ; }
  
  protected:

  private:

    double m_chi2; ///< chi square
    int    m_nDoF; ///< number of degrees of freedom
  
  }; // class ChiSquare


  inline ChiSquare& ChiSquare::operator+=(const ChiSquare& rhs) 
  {
    m_chi2 += rhs.m_chi2 ;
    m_nDoF += rhs.m_nDoF ;
    return *this ;
  }

  inline ChiSquare& ChiSquare::operator-=(const ChiSquare& rhs) 
  {
    m_chi2 -= rhs.m_chi2 ;
    m_nDoF -= rhs.m_nDoF ;
    return *this ;
  }

  inline ChiSquare ChiSquare::operator+(const ChiSquare& rhs) 
  {
    ChiSquare rc = *this ;
    rc += rhs ;
    return rc ;
  }

  inline ChiSquare ChiSquare::operator-(const ChiSquare& rhs) 
  {
    ChiSquare rc = *this ;
    rc -= rhs ;
    return rc ;
  }
}
  
#endif
