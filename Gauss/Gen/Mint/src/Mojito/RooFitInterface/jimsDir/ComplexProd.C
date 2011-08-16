// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:05 GMT
#include "ComplexProd.h"
#include "TIterator.h"

#include <complex>
#include <iostream>
#include "AbsComplex.h"

using namespace std;

ClassImp(ComplexProd)

complex<Double_t> ComplexProd::getProd() const{
  //  cout << "starting getProd from " << GetName() << endl;
  TIterator* resIter = resonances.createIterator() ;
  AbsComplexBrackets* resPtr;
  complex<Double_t> prod = 1;
  while((resPtr = dynamic_cast<AbsComplexBrackets*>(resIter->Next()))){ // yes, it's an assignment
    
    complex<Double_t> thisVal = resPtr->getCVal();
    prod *= thisVal;
  }
  delete resIter;
  //  cout << "getProd: returning " << prod << endl;
  return prod;
}
ComplexProd::ComplexProd(const char* name
		       , const char* title
		       , RooArgList& _resonances
		       )
  : AbsComplexPdf(name, title)
  , _result(0)
  , resonances("resonances", "resonances", this)
{
  cout << "ComplexProd: Got 1 list: " << endl;
  TIterator* resIter = _resonances.createIterator() ;
  RooAbsArg* resPtr;
  while((resPtr = (RooAbsArg*) resIter->Next())){
    // yes, it's an assignment

    cout << " ComplexProd::ComplexProd: adding variable "
	 <<  resPtr->GetName() 
	 << endl;

    if (!dynamic_cast<AbsComplexBrackets*>(resPtr)) {
      cout << "ERROR in ComplexProd::ComplexProd: variable "
	   <<  resPtr->GetName() 
	   << " is not complex "
	   << endl;
      throw "wrong type";
    }else{
      resonances.add(*resPtr);
    }
  }
  delete resIter;
  cout << "ComplexProd: done constructing " << endl;
}

ComplexProd::ComplexProd(const ComplexProd& other
		       , const char* name
		       )
  : AbsComplexPdf(other, name)
  , _result(other._result)
  , resonances("resonances", this, other.resonances)
{
}

Double_t ComplexProd::evaluate() const{
  _result = getProd();
  return _result.real()*_result.real() + _result.imag()*_result.imag();
}

//
