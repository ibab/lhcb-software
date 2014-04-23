// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:55 GMT

#include "Mint/BasicComplex.h"
using namespace MINT;

BasicComplex::BasicComplex(double initVal)
  : _z(initVal, 0)
{
}
BasicComplex::BasicComplex(const std::complex<double>& initVal)
  : _z(initVal)
{
}
BasicComplex::BasicComplex(const BasicComplex& other)
  : IReturnComplex()
  , _z(other._z)
{
}
BasicComplex::BasicComplex(IReturnComplex* irc)
  : _z(irc->ComplexVal())
{
}

std::complex<double> BasicComplex::ComplexVal(){
  return getVal();
}
void BasicComplex::setVal(std::complex<double>& val){
  _z=val;
}
std::complex<double> BasicComplex::getVal() const{
  return _z;
}
