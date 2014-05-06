#ifndef NEG2LL_HH
#define NEG2LL_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:56 GMT

#include "Mint/Minimisable.h"
#include "Mint/EventLoop.h"
#include "Mint/IPdf.h"

#include <iostream>

namespace MINT{

template<typename T>
class Neg2LL : public Minimisable, public EventLoop<T>{
  int _NCalls;
 protected:
  IPdf<T>* _pdf;
 public:
  
  Neg2LL( IPdf<T>* pdf
	  , IEventList<T>* erptr = 0
	  , MinuitParameterSet* mps = 0)
    : Minimisable(mps)
    , EventLoop<T>(erptr)
    , _NCalls(0)
    , _pdf(pdf)
    { 
      init();
    }
  Neg2LL( IPdf<T>* pdf
	  , MinuitParameterSet* mps = 0)
    : Minimisable(mps)
    , EventLoop<T>(0)
    , _NCalls(0)
    , _pdf(pdf)
    { 
      init();
    }
  Neg2LL(const Neg2LL<T>& other)
    : Minimisable(other)
    , EventLoop<T>(other)
    , _NCalls(other._NCalls)
    , _pdf(other._pdf)
    { 

    }

  virtual void beginFit(){
    if(OK()) pdf()->beginFit();
  };
  virtual void parametersChanged(){
    if(OK()) pdf()->parametersChanged();
  };

  virtual void endFit(){
    if(OK()) pdf()->endFit();
  };
  
  IPdf<T>* pdf(){
    return _pdf;
  }
  bool init(){
    if (!OK_elseShout()) return false;
    //    pdf()->setEventRecord( EventLoop<T>::getEventRecord() );
    return true;
  }

  bool OK() const{
    return 0 != _pdf;
  }
  bool OK_elseShout()const{
    if(! OK()){
      std::cout << "ERROR in Neg2LL"
		<< "_pdf Pointer is zero."
		<< std::endl;
    }
    return OK();
  }
  double logPdf(){
    bool dbThis = false;
    if(! OK_elseShout()) return -9999;
    if(dbThis) std::cout << "about to call pdf()->getVal() " << std::endl;
    double valPdf = pdf()->getVal();
    if(dbThis) std::cout << " that worked! the value is " 
	      << valPdf << std::endl;
    if(valPdf <= 0){
      if(dbThis) std::cout << "ERROR in Neg2LL::logPdf()"
			   << " the pdf is " << valPdf
			   << " which is <= 0." 
			   << std::endl;
      return -9999.e20 * (1.0 + fabs(valPdf));
    }
    return log(valPdf);
  }
  double getVal(){
    _NCalls ++;
    bool verbose=false;
    bool dbThis=false;
    int printFreq = 100;
    printFreq =    100;
    if(_NCalls >   500) printFreq =   1000;
    if(_NCalls > 10000) printFreq =   5000;
 
    bool printout = (0 == _NCalls%printFreq) || _NCalls <= 1;
    printout |= dbThis;
    verbose  |= dbThis;

    double sum=0;

    if(0 == pdf()){
      std::cout << "ERROR in Neg2LL::getVal!"
		<< "_pdf Pointer is zero."
		<< std::endl;
      return -9999.0;
    }
    if(verbose && printout){
      std::cout << "Neg2LL::getVal after " << _NCalls << " calls."
		<< " pdf ptr is non zero - that's a start." 
		<< std::endl;
    }
    pdf()->setEventRecord( EventLoop<T>::getEventRecord() );
    EventLoop<T>::getFirstEvent();
    if(verbose && printout){
      std::cout << "Neg2LL::getVal: got first event " << std::endl;
      std::cout << "PDF for first event: " << _pdf->getVal()
		<< std::endl;
    }
    int counter=0;
    while(EventLoop<T>::getNextEvent()){
      ++counter;
      //      if(printout) std::cout << " sum " << sum << std::endl;
      sum += logPdf();
    };
    pdf()->resetEventRecord();

    if(printout){
      std::cout << "Neg2LL::getVal after " << _NCalls << " calls."
		<< "for " << counter
		<< " events, I'm about to return " 
		<< -2.0*sum << std::endl;
    }
    return -2.* sum;
  }

  double getNewVal(){ // forces re-calculation after parameter change
    parametersChanged();
    return getVal();
  }
    
};
} // namespace MINT

#endif
//
