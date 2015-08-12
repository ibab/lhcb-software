// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:01 GMT
#include "Mint/DalitzPdfBase.h"

#include <iostream>

using namespace std;
using namespace MINT;

bool DalitzPdfBase::getNorm(){  
  if(_pat.size() <= 0) return false;

  _integrating = true;

  _mcint.initialise(_pat
		    , this
		    , _generator
		    , gRandom
		    , _precision
		    );
  _norm = _mcint.getVal();
  _integrating = false;
  return _norm > 0;
}

void DalitzPdfBase::setIntegrationPrecision(double prec){
  _precision = prec;
  _mcint.setPrecision(_precision);
}


void DalitzPdfBase::beginFit(){
  //getNorm();
}
void DalitzPdfBase::parametersChanged(){
  getNorm(); // should implement caching here.
}
void DalitzPdfBase::endFit(){

}

DalitzPdfBase::DalitzPdfBase( IEventGenerator<IDalitzEvent>* generator
			     , double prec
			     ) 
  : PdfBase<IDalitzEvent>()
  , _pat()
  , _norm(-1)
  , _precision(prec)
  , _generator(generator)
  , _integrating(0)
{
  
}

bool DalitzPdfBase::integrating(){
  return _integrating;
}
double DalitzPdfBase::phaseSpace(IDalitzEvent& evt){
  return evt.phaseSpace();
}
double DalitzPdfBase::getVal(IDalitzEvent& evt){
  if(_pat.empty()) _pat = evt.eventPattern();
  if(_integrating) return getVal_withPs(evt);
  else return getVal_noPs(evt);
}

double DalitzPdfBase::getVal_noPs(IDalitzEvent& evt){
  bool dbthis = false;
  if(_pat.empty()) _pat = evt.eventPattern();

  if(_integrating){
    // shouldn't really do that - use getVal or getVal_withPs
    // when you integrate (automatically done in getVal()):
    return un_normalised_noPs(evt);
  }else{
    if(-1 == _norm) getNorm(); // not threadsave
    //if(-1 == _norm){
    //  cout << "DalitzPdfBase::getVal_noPs: _norm = " 
    //	   << _norm << endl;
    //  throw "what's going on here?";
    //}
    double num = un_normalised_noPs(evt);
    if(dbthis)cout  << "un_normalised / norm: " 
		    << num << " / " <<_norm 
		    << " = " << num/_norm
		    << endl;
    return num/_norm;
  }
}

double DalitzPdfBase::getVal_withPs(IDalitzEvent& evt){
  bool dbthis = false;
  if(_pat.empty()) _pat = evt.eventPattern();
  if(_integrating){
    return un_normalised_noPs(evt)*phaseSpace(evt);
  }else{
    if(_norm == -1) getNorm(); // not threadsave
    //if(-1 == _norm){
    //  cout << "DalitzPdfBase::getVal_withPs: _norm = " 
    //	   << _norm << endl;
    //  throw "what's going on here?";
    //}
    double num = un_normalised_noPs(evt)*phaseSpace(evt);
    if(dbthis)cout  << "un_normalised / norm: " 
		    << num << " / " <<_norm 
		    << " = " << num/_norm
		    << endl;
    return num/_norm;
  }
}

//
//
