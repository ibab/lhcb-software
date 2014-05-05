#ifndef MINT_COMPLEX_PRODUCT_HH
#define MINT_COMPLEX_PRODUCT_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:55 GMT

#include "Mint/IReturnComplex.h"
#include "Mint/BasicComplex.h"
#include <vector>
#include <complex>
#include "Mint/counted_ptr.h"

namespace MINT{
  class ComplexProduct : public IReturnComplex{
  protected:
    std::vector< counted_ptr<IReturnComplex> > _facVec; 
    std::vector< double > _fixedDoubleVec; 
    std::vector< std::complex<double> > _fixedComplexVec; 
    
    inline double fixedRealProduct(){
      double prod(1); 
      for(unsigned i = 0; i < _fixedDoubleVec.size(); i++){
	prod *= _fixedDoubleVec[i];
      }      
      return prod;
    }
    inline std::complex<double> fixedComplexProduct(){
      std::complex<double> prod(1, 0); 
      for(unsigned i = 0; i < _fixedComplexVec.size(); i++){
	prod *= _fixedComplexVec[i];
      }      
      return prod;
    }
    
    inline std::complex<double> complexProduct(){
      std::complex<double> prod(1,0); 
      for(unsigned i = 0; i < _facVec.size(); i++){
	prod *= (_facVec[i])->ComplexVal();
      }      
      return prod;
    }
     
  public:
    ComplexProduct();
    ComplexProduct(double initVal);
    ComplexProduct(const std::complex<double>& z);
    ComplexProduct(const ComplexProduct& other);

    void multiply(const ComplexProduct& other);

    void addTerm(double val); // by value
    void addTerm(const std::complex<double>& z); // by value
    void addTerm(const counted_ptr<IReturnComplex>& irc); // by reference

    ComplexProduct& operator*=(double val);
    ComplexProduct& operator*=(const std::complex<double>& z);
    ComplexProduct& operator*=(const counted_ptr<IReturnComplex>& irc);
    ComplexProduct& operator*=(const ComplexProduct& cp);

   inline std::complex<double> ComplexVal(){
     return  complexProduct() *  fixedComplexProduct() * fixedRealProduct();
   }
    
  };
}
#endif
//
