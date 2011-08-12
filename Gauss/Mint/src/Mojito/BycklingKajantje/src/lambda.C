// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:58 GMT
#include "lambda.h"

double lambda(double x, double y, double z){  
  double a = (x-y-z);
  return a*a - 4*y*z;
}

//

