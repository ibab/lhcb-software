
#ifndef RICHREADOUT_RICHSHAPE_H
#define RICHREADOUT_RICHSHAPE_H 1

#include <math.h>

//! The signal shape function after the amplifier/shaper
class RichShape {

public:

  RichShape::RichShape (const double tpeak, 
                        const double alpha)
    :  m_alpha(alpha),
       m_tpeak(tpeak)  { }

  double operator[] (const double time) const;

  double getTpeak () const;

  void setTpeak (const double value);

private:

  double m_alpha;
  double m_tpeak;

};

inline double RichShape::getTpeak () const
{
  return m_tpeak;
}

inline void RichShape::setTpeak (const double value)
{
  m_tpeak = value;
}

inline double RichShape::operator[] (const double time) const
{
  if ( time <= 0 ) return 0;
  return ( pow(time/getTpeak(),m_alpha) *
           exp(-(time-getTpeak())/getTpeak()*m_alpha) );
}

#endif // RICHREADOUT_RICHSHAPE_H
