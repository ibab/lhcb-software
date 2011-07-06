#ifndef ROOFIT_EVENT_INTERFACE_HH
#define ROOFIT_EVENT_INTERFACE_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:11 GMT

#include "RooRealVar.h"
#include "DalitzEventAccess.h"
#include "IDalitzEventList.h"
#include "CLHEPSystemOfUnits.h"

class RooFitEventInterface : public DalitzEventAccess{
 protected:
  RooRealVar _t01;

  RooRealVar _s12;
  RooRealVar _s13;
  RooRealVar _s14;

  RooRealVar _s23;
  RooRealVar _s24;

  RooRealVar _s34;

  RooRealVar _t40;
 
 public:
  RooFitEventInterface(IDalitzEventList* events)
    : DalitzEventAccess(events)
    , _t01("t01", "t01", 0)
    , _s12("s12", "s12", 0)
    , _s13("s13", "s13", 0)
    , _s14("s14", "s14", 0)  
    , _s23("s23", "s23", 0)
    , _s24("s24", "s24", 0)
    , _s34("s34", "s34", 0)
    , _t40("t40", "t40", 0)
    {}
    
  
  bool update(){
    double GeV2 = GeV*GeV;
    if(0 == getEvent()) return false;
    _t01.setVal(getEvent()->t(0,1)/GeV2);
    _s12.setVal(getEvent()->s(1,2)/GeV2);
    _s13.setVal(getEvent()->s(1,3)/GeV2);
    _s23.setVal(getEvent()->s(2,3)/GeV2);

    if(getEvent()->eventPattern().size() >= 5){ // 5=mum + 4 dgtr
      _s14.setVal(getEvent()->s(1,4)/GeV2);
      _s34.setVal(getEvent()->s(3,4)/GeV2);
      _t40.setVal(getEvent()->t(4,0)/GeV2);
    }
    return true;
  }

  bool setAll(){
    return update();
  }

  RooRealVar& t01(){
    if(0 != getEvent()) _t01.setVal(getEvent()->t(0,1)/(GeV*GeV));
    return _t01;}

  RooRealVar& s12(){
    if(0 != getEvent()) _s12.setVal(getEvent()->s(1,2)/(GeV*GeV));
    return _s12;}

  RooRealVar& s13(){
    if(0 != getEvent()) _s13.setVal(getEvent()->s(1,3)/(GeV*GeV));
    return _s13;}

  RooRealVar& s14(){
    if(0 != getEvent()) _s14.setVal(getEvent()->s(1,4)/(GeV*GeV));
    return _s14;}

  RooRealVar& s23(){
    if(0 != getEvent()) _s23.setVal(getEvent()->s(2,3)/(GeV*GeV));
    return _s23;}

  RooRealVar& s34(){
    if(0 != getEvent()) _s34.setVal(getEvent()->s(3,4)/(GeV*GeV));
    return _s34;}

  RooRealVar& t40(){
    if(0 != getEvent()) _t40.setVal(getEvent()->t(4,0)/(GeV*GeV));
    return _t40;}

};

#endif
//
