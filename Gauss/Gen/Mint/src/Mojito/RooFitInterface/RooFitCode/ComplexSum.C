// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:11 GMT
#include "ComplexSum.h"
#include "TIterator.h"
#include <iostream>
#include "AbsComplex.h"

using namespace std;

//ClassImp(ComplexSum)
RooRealVar ComplexSum::null("null", "null", 0);
RooRealVar ComplexSum::unit("unit", "unit", 1);

complex<Double_t> ComplexSum::getSum() const{
  //  cout << "getSum creating iterator from " << resonances.GetName() << endl;
  TIterator* resIter = resonances.createIterator() ;
  AbsComplexBrackets* resPtr;
  complex<Double_t> sum = 0;
  while((resPtr = dynamic_cast<AbsComplexBrackets*>(resIter->Next()))){ 
    // yes, it's an assignment in a logical statement.

    complex<Double_t> thisVal = resPtr->getCVal();
    if(debug)cout << GetName() << " adding " 
		  << (dynamic_cast<RooAbsReal*>(resPtr))->GetName() 
		  << endl;
    sum += thisVal;
  }
  delete resIter;
  if(debug)  cout << GetName() << "::getSum() returning " << sum << endl;
  return sum;
}

void ComplexSum::addValues(std::map<std::string, Double_t>& valMap) const{
  TIterator* resIter = resonances.createIterator() ;
  AbsComplexBrackets* resPtr;
  complex<Double_t> sum = 0;
  while((resPtr = dynamic_cast<AbsComplexBrackets*>(resIter->Next()))){ 
    // yes, it's an assignment in a logical statement.

    complex<Double_t> thisVal = resPtr->getCVal();
    string thisName = (dynamic_cast<RooAbsReal*>(resPtr))->GetName();
    Double_t thisRealVal = thisVal.real() * thisVal.real()
      +                    thisVal.imag() * thisVal.imag();

    valMap["incoherentSum"] += thisRealVal;
    valMap[thisName]        += thisRealVal;

    if(debug)cout << GetName() << " adding " 
		  << thisName 
		  << ", " << thisVal 
		  << ", " << thisRealVal
		  << endl;
    sum += thisVal;

    
  }
  delete resIter;
  if(debug)  cout << GetName() << "::getSum() returning " << sum << endl;

  double totalVal = sum.real()*sum.real()
    +               sum.imag()*sum.imag();

  valMap["total"] += totalVal;
}

ComplexSum::ComplexSum(const char* name
		       , const char* title
		       , RooArgList& _resonances
		       , RooAbsReal& _amp
		       , RooAbsReal& _phase
		       )
  : AbsComplex(name, title)
  , debug(false)
  , _result(0,0)
  , resonances("resonances", "resonances", this)
  , amp("amp", "amp", this, _amp)
  , phase("phase", "phase", this, _phase)
{
  cout << "ComplexSum: Got 1 list: " << endl;
  TIterator* resIter = _resonances.createIterator() ;
  RooAbsArg* resPtr;

  while((resPtr = (RooAbsArg*) resIter->Next())){
    // yes, it's an assignment

    if(debug)cout << " ComplexSum::ComplexSum: adding variable "
		  <<  resPtr->GetName() 
		  << endl;

    if (!dynamic_cast<AbsComplexBrackets*>(resPtr)) {
      cout << "ERROR in ComplexSum::ComplexSum: variable "
	   <<  resPtr->GetName() 
	   << " is not complex "
	   << endl;
      throw "wrong type";
    }else{
      resonances.add(*resPtr);
    }
  }
  delete resIter;
  cout << "ComplexSum: done constructing " << endl;
}

ComplexSum::ComplexSum(const ComplexSum& other
		       , const char* name
		       )
  : AbsComplex(other, name)
  , debug(other.debug)
  , _result(other._result)
  , resonances("resonances", this, other.resonances)
  , amp("amp", this, other.amp)
  , phase("phase", this, other.phase)
{
  /*cout << "ComplexSum " << GetName() << " got copied."
       << "\n Sum before copy: " << other.getSum()
       << "\n Sum after copy:  " << this->getSum()
       << endl;*/
}

Double_t ComplexSum::evaluate() const{
  complex<Double_t> sum=getSum();
  _result = sum * getComplexFactor();
  if(debug)  cout << GetName() << " returning "
		  << sum << " * " << getComplexFactor()
		  << " = " << _result << endl;
  Double_t modsquared = _result.real()*_result.real() 
    + _result.imag() * _result.imag();
  return modsquared;
}

//
