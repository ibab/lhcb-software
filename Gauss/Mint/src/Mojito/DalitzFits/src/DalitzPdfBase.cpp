// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:01 GMT
#include "Mint/DalitzPdfBase.h"

#include <iostream>

using namespace std;
using namespace MINT;

bool DalitzPdfBase::getNorm(){
  _integrating = true;
  if( ! _mcint.initialised()){
    IDalitzEvent* evtPtr = getEvent();
    if(0==evtPtr){
      cout << "gammaPdf::getNorm: can't get event!"
	   << endl;
      _integrating = false;
      return false;
    }
    _mcint.initialise(evtPtr->eventPattern()
		      , this
		      , _generator
		      , gRandom
		      , _precision
		      );
  }
  _norm = _mcint.getVal();
  _integrating = false;
  return _norm > 0;
}

void DalitzPdfBase::setIntegrationPrecision(double prec){
  _precision = prec;
  _mcint.setPrecision(_precision);
}

void DalitzPdfBase::parametersChanged(){
  getNorm();
}
DalitzPdfBase::DalitzPdfBase(IDalitzEventAccess* events
			     , IEventGenerator<IDalitzEvent>* generator
			     , double prec
			     ) 
  : PdfBase<IDalitzEvent>(events)
    , _norm(-1)
    , _precision(prec)
    , _generator(generator)
    , _integrating(0)
{
  
}
DalitzPdfBase::DalitzPdfBase(IDalitzEventList* events
			     , IEventGenerator<IDalitzEvent>* generator
			     , double prec
			     ) 
  : PdfBase<IDalitzEvent>(events)
  , _norm(-1)
  , _precision(prec)
  , _generator(generator)
  , _integrating(0)
{
  
}

bool DalitzPdfBase::integrating(){
  return _integrating;
}
double DalitzPdfBase::phaseSpace(){
  IDalitzEvent* evt = getEvent();
  if(0 == evt) return 0;
  return evt->phaseSpace();
}
double DalitzPdfBase::getVal(){
  if(_integrating) return getVal_withPs();
  else return getVal_noPs();
}
double DalitzPdfBase::getVal(IDalitzEvent* evt){
  this->setEvent(evt);
  double result = this->getVal();
  this->resetEventRecord();
  return result;
}

double DalitzPdfBase::getVal_noPs(){
  bool dbthis = false;
  if(_integrating){
    // shouldn't really do that - use getVal or getVal_withPs
    // when you integrate (automatically done in getVal()):
    return un_normalised_noPs();
  }else{
    if(_norm == -1) getNorm();
    double num = un_normalised_noPs();
    if(dbthis)cout  << "un_normalised / norm: " 
		    << num << " / " <<_norm 
		    << " = " << num/_norm
		    << endl;
    return num/_norm;
  }
}
double DalitzPdfBase::getVal_noPs(IDalitzEvent* evt){
  this->setEvent(evt);
  double result = this->getVal_noPs();
  this->resetEventRecord();
  return result;
}

double DalitzPdfBase::getVal_withPs(){
  bool dbthis = false;
  if(_integrating){
    return un_normalised_noPs()*phaseSpace();
  }else{
    if(_norm == -1) getNorm();
    double num = un_normalised_noPs()*phaseSpace();
    if(dbthis)cout  << "un_normalised / norm: " 
		    << num << " / " <<_norm 
		    << " = " << num/_norm
		    << endl;
    return num/_norm;
  }
}

double DalitzPdfBase::getVal_withPs(IDalitzEvent* evt){
  this->setEvent(evt);
  double result = this->getVal_withPs();
  this->resetEventRecord();
  return result;
}

//
