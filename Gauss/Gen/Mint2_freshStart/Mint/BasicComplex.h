#ifndef MINT_BASIC_COMPLEX_CLASS_HH
#define MINT_BASIC_COMPLEX_CLASS_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:55 GMT
#include "Mint/IReturnComplex.h"
#include <complex>
namespace MINT{
  class BasicComplex : virtual public IReturnComplex{
    std::complex<double> _z;
  public:
    BasicComplex(double initVal=0);
    BasicComplex(const std::complex<double>& initVal);
    BasicComplex(const BasicComplex& other);
    BasicComplex(IReturnComplex* other);
    
    virtual std::complex<double> ComplexVal();
    virtual void setVal(std::complex<double>& val);
    virtual std::complex<double> getVal() const;

    virtual ~BasicComplex(){};
  };
}
#endif
//

