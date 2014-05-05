// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:59 GMT
#include "Mint/BaseGenerator.h"
#include "Mint/CLHEPPhysicalConstants.h"

#include <ctime>
#include <iostream>

using namespace std;
using namespace MINT;

BaseGenerator::BaseGenerator(TRandom* rnd)
  : _unWeighted(true)
  , _evtList(0)
  , _rnd(rnd)
  , _mums_p(0)
  , _mums_cosTheta(0)
  , _mums_phi(0)
{

}
BaseGenerator::BaseGenerator(const BaseGenerator& other)
  : IEventGenerator<IDalitzEvent>()
  , _unWeighted(other._unWeighted)
  , _evtList(other._evtList)
  , _rnd(other._rnd)
  , _mums_p(other._mums_p)
  , _mums_cosTheta(other._mums_cosTheta)
  , _mums_phi(other._mums_phi)
{
}

void BaseGenerator::setMothers3Momentum(counted_ptr<IReturnReal> p
					, counted_ptr<IReturnReal> cosTheta
					, counted_ptr<IReturnReal> phi
					){
  _mums_p = p;
  _mums_cosTheta = cosTheta;
  _mums_phi = phi;
}
void BaseGenerator::unsetMothers3Momentum(){
  _mums_p = _mums_cosTheta = _mums_phi = counted_ptr<IReturnReal>(0);
}

bool BaseGenerator::mothers3MomentumIsSet() const{
  return (0 != _mums_p);
}

TVector3 BaseGenerator::mothers3Momentum() const{
  if(mothers3MomentumIsSet()){
    double p, cT, phi;
    if(0 != _mums_p) p = _mums_p->RealVal();
    else p=0;
    if(0 != _mums_cosTheta) cT = _mums_cosTheta->RealVal();
    else cT = _rnd->Rndm()*2.0 - 1.0;
    if(0 != _mums_phi) phi = _mums_phi->RealVal();
    else phi = _rnd->Rndm()*2.0*pi;
    
    double sT2 = 1.0 - cT*cT;
    double sT;
    if(sT2 < 0) sT=0;
    else sT = sqrt(sT2);

    TVector3 v(p*cos(phi)*sT, p*sin(phi)*sT, p*cT);

    return v;
  }else{
    TVector3 v(0,0,0);
    return v;
  }
}

void BaseGenerator::setSaveEvents(const std::string& fname
				  , const std::string& opt){
  _evtList = 
    counted_ptr<DiskResidentEventList>(new DiskResidentEventList(fname, opt));
}
void BaseGenerator::dontSaveEvents(){
  _evtList = 
    counted_ptr<DiskResidentEventList>(0);
}

void BaseGenerator::FillEventList(DiskResidentEventList& evtList, int NEvents){
  bool dbThis=true;
  if(dbThis) cout << "BaseGenerator::FillEventList (dr) called" << endl;
  time_t t0 = time(0);
  for(int i=0; i < NEvents; i++){

    int printEvery  = 1000;
    if(i < 1000) printEvery=100;
    bool printout = ( i%printEvery == 0 || i < 100);
    if(dbThis || printout){
      cout << "BaseGenerator::FillEventList (disk resident)" << endl;
      cout << " about to make event number " << i << endl;
      cout << "   current list size " << evtList.size() << endl;
      double dt = difftime(time(0), t0);
      cout << "     this took " << dt << "s" << endl;
      if(dt > 0) cout << "     or " << evtList.size()/dt << "evt/s" << endl;
    }
    counted_ptr<IDalitzEvent> evtPtr(newEvent());
    DalitzEvent evt(evtPtr.get());
    evtList.Add(evt);
    if(dbThis){
      cout << "BaseGenerator::FillEventList (disk resident)" <<endl;
      cout << "After adding this event to the event list:\n " << evt << endl;
      cout << ".... the last event in the list is:\n " 
	   << evtList.getEvent(evtList.size()-1) << endl;
      cout << "are they the same?" << endl;
    }
  }
}

void BaseGenerator::FillEventList(DalitzEventList& evtList, int NEvents){
  bool dbThis=false;
  if(dbThis) cout << "BaseGenerator::FillEventList (mr) called" << endl;

  time_t t0 = time(0);
  for(int i=0; i < NEvents; i++){

    int printEvery  = 1000;
    if(i < 1000) printEvery=100;
    bool printout = ( i%printEvery == 0 || i < 100);
    if(dbThis || printout){
      cout << "BaseGenerator::FillEventList (memory resident)" << endl;
      cout << " about to make event number " << i << endl;
      cout << "   current list size " << evtList.size() << endl;
      double dt = difftime(time(0), t0);
      cout << "     this took " << dt << "s" << endl;
      if(dt > 0) cout << "     or " << evtList.size()/dt << "evt/s" << endl;
    }
    //    counted_ptr<IDalitzEvent> evt = newEvent();
    if(dbThis) cout << " got new event, now adding it" << endl;
    //    evtList.Add(counted_ptr<DalitzEvent>(new DalitzEvent(evt.get()))); // this is a bit silly - will fix later
    counted_ptr<IDalitzEvent> evtPtr(newEvent());
    DalitzEvent evt(evtPtr.get());
    evtList.Add(evt);
    if(dbThis){
      cout << "BaseGenerator::FillEventList (memory resident)" <<endl;
      cout << "After adding this event to the event list:\n " << evt << endl;
      cout << ".... the last event in the list is:\n " 
	   << evtList[evtList.size()-1] << endl;
      cout << "are they the same?" << endl;
    }

  }
}

bool BaseGenerator::ensureFreshEvents(){
  if(0 == _rnd) _rnd = gRandom;
  _rnd->SetSeed(time(0)*3);
  return true;
}
BaseGenerator::~BaseGenerator(){

}
