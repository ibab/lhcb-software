
#ifndef RICHREADOUT_RICHSHAPE_FLAT_H
#define RICHREADOUT_RICHSHAPE_FLAT_H 1

#include <math.h>

//! The signal shape function after the amplifier/shaper
class RichShape_flat {

public:

  RichShape_flat::RichShape_flat (double tpeak, double alpha)
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

inline double RichShape_flat::operator() (double time) const
{
  if ( time <= 0 ) return 0;
  double voltage = pow(time/getTpeak(),m_alpha) * 
		     exp(-(time-getTpeak())/getTpeak()*m_alpha);
  if (time > 50 && time < 100) voltage = -1.;
  if (time > 100) voltage = 0.;
  return voltage;
}

#endif // RICHREADOUT_RICHSHAPE_FLAT_H 
