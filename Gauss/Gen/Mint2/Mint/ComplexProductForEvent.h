#ifndef MINT_COMPLEX_PRODUCT_FOREVENT_HH
#define MINT_COMPLEX_PRODUCT_FOREVENT_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:55 GMT

#include "Mint/ComplexProduct.h"
#include "Mint/IComplexFitParDependent.h"
#include "Mint/IComplexForEventFitParDependent.h"
#include "Mint/IReturnComplexForEvent.h"
#include "Mint/IFitParRegister.h"
#include "Mint/FitParDependent.h"

#include "Mint/BasicComplex.h"
#include <vector>
#include <complex>
#include "Mint/counted_ptr.h"

namespace MINT{
  template<typename EVENT_TYPE>
    class ComplexProductForEvent 
    : virtual public IComplexForEventFitParDependent<EVENT_TYPE>
    , public FitParDependent
  {
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
  ComplexProductForEvent(IFitParRegister* daddy=0)
    : FitParDependent(daddy)
      , _eventIndependentProduct(this){}
  ComplexProductForEvent(double initVal, IFitParRegister* daddy=0)
    : FitParDependent(daddy)
     , _eventIndependentProduct(initVal, this){}
  ComplexProductForEvent(const std::complex<double>& z, IFitParRegister* daddy=0) 
    :  FitParDependent(daddy)
    , _eventIndependentProduct(z, this){}
  ComplexProductForEvent(const ComplexProduct& other, IFitParRegister* newDaddy=0) 
    : FitParDependent(other, newDaddy)
      , _eventIndependentProduct(other, this){}
  ComplexProductForEvent(const ComplexProductForEvent<EVENT_TYPE>& other
			 , IFitParRegister* newDaddy=0)
    : FitParDependent(other, newDaddy)
      , _eventIndependentProduct(other._eventIndependentProduct, this)
      , _evt_dep_factors(other._evt_dep_factors)
      {}
    
  void addTerm(double val){_eventIndependentProduct.addTerm(val);}   // by value
  void addTerm(const std::complex<double>& z){_eventIndependentProduct.addTerm(z);} // by value
  void addTerm(const counted_ptr<IComplexFitParDependent>& irc){
    _eventIndependentProduct.addTerm(irc);} // by reference
  void addTerm(const counted_ptr<IReturnComplex>& irc){
    _eventIndependentProduct.addTerm(irc);} // by reference
  void addTerm(const counted_ptr<IComplexForEventFitParDependent<EVENT_TYPE> >& irce){
    registerFitParDependence(*irce);
    _evt_dep_factors.push_back(irce);} // by reference
  void addTerm(const counted_ptr<IReturnComplexForEvent<EVENT_TYPE> >& irce){
    _evt_dep_factors.push_back(irce);} // by reference

  void multiply(const ComplexProductForEvent& other, IFitParRegister* newDaddy=0){
    registerFitParDependence(other, newDaddy);
    _eventIndependentProduct.multiply(other);
    for(unsigned int i=0; i< other._evt_dep_factors.size(); i++){
      addTerm(other._evt_dep_factors[i]);
    }
  }

  ComplexProductForEvent& operator*=(double val){addTerm(val);}
  ComplexProductForEvent& operator*=(const std::complex<double>& z){addTerm(z);}
  ComplexProductForEvent& operator*=(const counted_ptr<IReturnComplex>& irc){addTerm(irc);}
  ComplexProductForEvent& operator*=(const counted_ptr<IComplexForEventFitParDependent<EVENT_TYPE> >& irce){addTerm(irce);}
  ComplexProductForEvent& operator*=(const counted_ptr<IReturnComplexForEvent<EVENT_TYPE> >& irce){addTerm(irce);}
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
