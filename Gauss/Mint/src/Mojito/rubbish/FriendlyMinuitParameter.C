#ifndef FRIENDLY_MINUIT_PARAMETER_HH
#define FRIENDLY_MINUIT_PARAMETER_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:12 GMT

#include "TMinuit.h"

FriendlyMinuitParameter::FriendlyMinuitParameter()
  : _myMinuit(0)
  , _nP(-1)
{}
const TMinuit* FriendlyMinuitParameter::getMinuit() const{
  if(0 == _myMinuit){
    std::cout << "ERROR in FriendlyMinuitParameter"
	      << " you are asking for access to _myMinuit"
	      << " although it hasn't been set" 
	      << " - will probably crash."
	      << std::endl;
  }
  return _myMinuit;
}
TMinuit* FriendlyMinuitParameter::getMinuit(){
  if(0 == _myMinuit){
    std::cout << "ERROR in FriendlyMinuitParameter"
	      << " you are asking for access to _myMinuit"
	      << " although it hasn't been set" 
	      << " - will probably crash"
	      << std::endl;
  }
  return _myMinuit;
}
int FriendlyMinuitParameter::iFixInit() const{ return _iFixInit;}
int FriendlyMinuitParameter::meanInit() const{ return _meanInit;}
int FriendlyMinuitParameter::stepInit() const{ return _stepInit;}
int FriendlyMinuitParameter::minInit() const{ return _minInit;}
int FriendlyMinuitParameter::maxInit() const{ return _maxInit;}
std::string FriendlyMinuitParameter::nameInit()const {return _nameInit;}

int FriendlyMinuitParameter::parNumber() const{
  if(_pN < 0 || _pN > getMinuit()->fMaxpar){
    cout << "ERROR IN FriendlyMinuitParameter "
	 << nameInit()
	 << " index out of bounds" << endl;
    throw "index out of bounds in FriendlyMinuitParameter";
  }
  return _pN;
}
void FriendlyMinuitParameter::setParNumber(int i){
  _pn = i;
}
void FriendlyMinuitParameter::associate(TMinuit* tm){
  myMinuit=tm;
}

bool FriendlyMinuitParameter::MinuitOK() const{
  return (0!= getMinuit()) && (_pn > 0) && ( _pn < getMinuit()->fMaxpar);
}
double FriendlyMinuitParameter::mean() const{
  if(MinuitOK()){
    return getMinuit()->fU[parNumber()];
  }else{
    return meanInit();
  }
}
double FriendlyMinuitParameter::min() const{
  if(MinuitOK()){
    return getMinuit()->fAlim[parNumber()];
  }else{
    return minInit();
  }
}
double FriendlyMinuitParameter::max() const{
  if(MinuitOK()){
    return getMinuit()->fBlim[parNumber()];
  }else{
    return maxInit();
  }
}
double FriendlyMinuitParameter::errPos() const{
  if(MinuitOK()){
    return getMinuit()->fErp[parNumber()];
  }else{
    return stepInit();
  }
}
double FriendlyMinuitParameter::errNeg() const{
  if(MinuitOK()){
    return getMinuit()->fErn[parNumber()];
  }else{
    return stepInit();
  }
}
double FriendlyMinuitParameter::err() const{
  if(MinuitOK()){
    return getMinuit()->fWerr[parNumber()];
  }else{
    return stepInit();
  }
}
//
