#ifndef MINT_COMPLEX_PRODUCT_FOREVENT_HH
#define MINT_COMPLEX_PRODUCT_FOREVENT_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:55 GMT

#include "Mint/ComplexProduct.h"
#include "Mint/IReturnComplexForEvent.h"
#include "Mint/BasicComplex.h"
#include <vector>
#include <complex>
#include "Mint/counted_ptr.h"

namespace MINT{
  template<typename EVENT_TYPE>
    class ComplexProductForEvent : virtual public IReturnComplexForEvent<EVENT_TYPE>{
    ComplexProduct _eventIndependentProduct;
    
    std::vector< counted_ptr<IReturnComplexForEvent<EVENT_TYPE> > > _evt_dep_factors; 
    
    inline std::complex<double> event_dependent_complexProduct(EVENT_TYPE& evt){
      std::complex<double> prod(1,0); 
      for(unsigned i = 0; i < _evt_dep_factors.size(); i++){
	prod *= (_evt_dep_factors[i])->ComplexVal(evt);
      }      
      return prod;
    }
    
  public:
  ComplexProductForEvent() : _eventIndependentProduct(){}
  ComplexProductForEvent(double initVal) : _eventIndependentProduct(initVal){}
  ComplexProductForEvent(const std::complex<double>& z) : _eventIndependentProduct(z){}
  ComplexProductForEvent(const ComplexProduct& other) : _eventIndependentProduct(other){}
  ComplexProductForEvent(const ComplexProductForEvent<EVENT_TYPE>& other)
    : _eventIndependentProduct(other._eventIndependentProduct)
      , _evt_dep_factors(other._evt_dep_factors)
      {}
    
  void addTerm(double val){_eventIndependentProduct.addTerm(val);}   // by value
  void addTerm(const std::complex<double>& z){_eventIndependentProduct.addTerm(z);} // by value
  void addTerm(const counted_ptr<IReturnComplex>& irc){_eventIndependentProduct.addTerm(irc);} // by reference
  void addTerm(counted_ptr<IReturnComplexForEvent<EVENT_TYPE> >& irce){_evt_dep_factors.push_back(irce);} // by reference

  void multiply(const ComplexProductForEvent& other){
    _eventIndependentProduct.multiply(other);
    for(unsigned int i=0; i< other._evt_dep_factors.size(); i++){
      addTerm(other._evt_dep_factors[i]);
    }
  }

  ComplexProductForEvent& operator*=(double val){addTerm(val);}
  ComplexProductForEvent& operator*=(const std::complex<double>& z){addTerm(z);}
  ComplexProductForEvent& operator*=(const counted_ptr<IReturnComplex>& irc){addTerm(irc);}
  ComplexProductForEvent& operator*=(counted_ptr<IReturnComplexForEvent<EVENT_TYPE> >& irce){addTerm(irce);}
  ComplexProductForEvent& operator*=(const ComplexProduct& cp){_eventIndependentProduct.multiply(cp);}
  ComplexProductForEvent& operator*=(const ComplexProductForEvent<EVENT_TYPE>& cpe){multiply(cpe);}

  inline std::complex<double> ComplexVal(EVENT_TYPE& evt){
    return  event_dependent_complexProduct(evt) 
      *  _eventIndependentProduct.ComplexVal();
  }
    
  };
}
#endif
//
