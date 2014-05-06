#ifndef GAUSS_FCT_HH
#define GAUSS_FCT_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:57 GMT

#include "Mint/IReturnReal.h"

namespace MINT{
class GaussFct : virtual public IReturnReal{
 protected:
  double _x;
  double _height;
  double _mean, _sigma;
  double _sqrt2pi;

 public:
  double x() const;
  double height() const;
  double mean() const;
  double sigma() const;
  
  void set_x(double x_in);
  void set_height(double h);
  void set_mean(double m);
  void set_sigma(double s);
  
  double getVal() const;
  double getIntegral() const;
  double RealVal(){ return getVal(); }

  GaussFct(double x_in, double h, double m, double s);

  GaussFct();

};
} // namespace MINT

#endif
//
