
#include "RichReadout/RichShape.h" 
//#include <iostream.h>
#include <iostream>
#include <math.h>


RichShape::RichShape (double tpeak, double alpha)
      :  alpha_(alpha), tpeak_(tpeak) 
 {}


double RichShape::operator() (double time) const
{
  double tpeak = getTpeak();
  if (time > 0.)
    return pow(time/tpeak,alpha_)*exp(-(time-tpeak)/tpeak*alpha_);
  else
    return 0.;
}













