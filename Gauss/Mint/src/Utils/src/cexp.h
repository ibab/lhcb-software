// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:56 GMT
#ifndef CEXP_THE_COMPLEX_EXP
#define CEXP_THE_COMPLEX_EXP

#include <complex>
#include <cmath>

namespace MINT{
template<typename T>
std::complex<T> cexp(std::complex<T> arg){
  std::complex<T> phase(cos(arg.imag()), sin(arg.imag()));
  return exp(arg.real())*phase;
}
}//namespace MINT

#endif
//
