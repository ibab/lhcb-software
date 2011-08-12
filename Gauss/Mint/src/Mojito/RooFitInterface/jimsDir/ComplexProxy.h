// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:05 GMT
#ifndef DALITZCOMPLEXPROXY
#define DALITZCOMPLEXPROXY
#include "TObject.h"
#include "Rtypes.h"

#include <complex>
#include "RooRealProxy.h"
#include "AbsComplexBrackets.h"

class ComplexProxy : public RooRealProxy, public AbsComplexBrackets{
  bool typecheck() const{
    if( ! dynamic_cast<AbsComplexBrackets*>(_arg)){
      cout << " ComplexProxy: Parameter " 
	   << _arg->GetName()
	   << " does not implement AbsComplexBrackets. ";
      throw "wrong type";
    }
  }

 public:
  ComplexProxy() : RooRealProxy() {}
  ComplexProxy(const char* name, const char* desc
	       , RooAbsArg* owner, RooAbsReal& ref
               , Bool_t valueServer=kTRUE
	       , Bool_t shapeServer=kFALSE
	       , Bool_t proxyOwnsArg=kFALSE) :
    RooRealProxy(name, desc, owner, ref, valueServer
		 , shapeServer, proxyOwnsArg){
    typecheck();
  }

  ComplexProxy(const char* name, RooAbsArg* owner
	       , const ComplexProxy& other) :
    RooRealProxy(name, owner, other){
    typecheck();
  }

  /*
  ComplexProxy(const ComplexProxy& other) :
    RooRealProxy(other){}
  */
  virtual TObject* Clone(const char* newName=0) const { 
    return new ComplexProxy(*this); }
  virtual ~ComplexProxy(){};

  // Accessors
  //  inline operator Double_t() const { return _isFund?((RooAbsReal*)_arg)->_value:((RooAbsReal*)_arg)->getVal(_nset) ; }
  std::complex<Double_t> getCVal() const { 
    Double_t toMakeItDoWhatHasToBeDone = (Double_t) *this;
    AbsComplexBrackets* acb = dynamic_cast<AbsComplexBrackets*>(_arg);
    return acb->getCVal();
  }

  ClassDef(ComplexProxy, 0)
};

#endif
//
