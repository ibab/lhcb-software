
#ifndef RICHREADOUT_RICHSHAPE_H
#define RICHREADOUT_RICHSHAPE_H 1

#include <math.h>

//AH 290903

//! The signal shape function after the amplifier/shaper
class RichShape {

public:

  RichShape::RichShape (double tpeak, double alpha)
    :  m_alpha(alpha), 
       m_tpeak(tpeak)  { }
  
  double operator() (double time) const;

  double getTpeak () const 
  {
    return m_tpeak;
  }
  
  void setTpeak (double value) 
  {
    m_tpeak = value;
  }

private:

  double m_alpha;
  double m_tpeak;

};

inline double RichShape::operator() (double time) const
{
  if ( time <= 0 ) return 0;
  return ( pow(time/getTpeak(),m_alpha) * 
           exp(-(time-getTpeak())/getTpeak()*m_alpha) );
}

#endif // RICHREADOUT_RICHSHAPE_H 
