// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:00 GMT
#include "Mint/DalitzEventPtrList.h"
#include "Mint/DalitzEvent.h"
#include "Mint/FitAmpSum.h"
#include "Mint/AllPossibleSij.h"

#include "TRandom.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TNtupleD.h"
#include "TObjString.h"

#include <iostream>
#include <ctime>

using namespace std;
using namespace MINT;

const std::string DalitzEventPtrList::_className("DalitzEventPtrList");

DalitzEventPtrList::DalitzEventPtrList() 
  : EventPtrList<IDalitzEvent, DalitzEvent>() 
{
}

DalitzEventPtrList::DalitzEventPtrList( const DalitzEventPtrList& other ) 
  : ILoopable()
  , IBasicEventAccess<IDalitzEvent>()
  , IEventList<IDalitzEvent>()
  , EventPtrList<IDalitzEvent, DalitzEvent>(other) 
{
}

int DalitzEventPtrList::generateEvents(unsigned int NumEvents
				    , const DalitzEventPattern& pat
				    , IGetRealEvent<IDalitzEvent>* amps
				    , TRandom* rnd
				    ){
  time_t startTime = time(0);
  int startSize = this->size();
  int N_toStartWith = 50000;
  if(pat.size() >= 5){
    N_toStartWith *= 10;
  }
  DalitzEventPtrList newList;
  newList.generatePhaseSpaceEvents(N_toStartWith, pat, rnd);

  double max = -9999;
  newList.findMaxAndThrowAwayData(max, amps, NumEvents, rnd); 
  // throws away data & tells us estimated absolute max
  this->Add(newList);

  amps->setEventRecord(&newList);
  int counter = 0;
  while(this->size()-startSize < NumEvents){
    counter++;
    bool printit = ! (counter%50000);
    counted_ptr<DalitzEvent> evt(new DalitzEvent(pat, rnd));

    newList.clear();
    newList.Add(evt);
    newList.Start();
    newList.Next();
    double d=amps->RealVal();
    
    if(d > max){
      cout << "DalitzEventPtrList::generateEvents: ERROR: We're in some trouble: amps value = " << d
	   << "\n is larger than the estimated maximum of " << max
	   << "\n This could mean the data are worthless."
	   << " I'll continue generating anyway, but"
	   << "\n I'll re-set max to this maximum + 10% safety margin";
      max = d*1.1;
      cout << " = " << max << endl;
    }

    if(printit){
      cout << " DalitzEventPtrList::generateEvents INFO: current size " 
	   << this->size()
	   << " target: " << NumEvents
	   << "\n this took " << difftime(time(0), startTime)/60 
	   << " minutes"
	   << endl;
      cout << " current event gives amps = " << d
	   << " for max = " << max
	   << " ... accepted? ";
    }
    if(rnd->Rndm()*max < d){
      if(printit) cout << " yes";
      this->Add(evt);
    }else{
      if(printit) cout << "no";
    }
    if(printit) cout << endl;
  }
  amps->resetEventRecord();

  cout << "DalitzEventPtrList::generateEvents: generating " << this->size() 
       << " Events took: " <<  difftime(time(0), startTime)/60 << " minutes"
       << endl;
  return this->size();
}
int DalitzEventPtrList::generatePhaseSpaceEvents(int NumEvents
					      , const DalitzEventPattern& pat
					      , TRandom* rnd
					      ){

  cout << "DalitzEventPtrList::generatePhaseSpaceEvents "
       << " about to generate " << NumEvents 
       << " events to phase space" << endl;
  time_t tstart = time(0);
  int reportN = NumEvents/10; // 10 x at least;
  if(reportN > 10000) reportN = 10000; // at least once every 10k event
  for(int i=0; i< NumEvents; i++){
    if( i < 5 || (!(i%reportN))){
      cout << "DalitzEventPtrList::generatePhaseSpaceEvents " 
	   << " generating " << i+1 << "th event."
	   << " with " << pat << " and " << rnd << endl;
    }
    counted_ptr<DalitzEvent> evt(new DalitzEvent(pat, rnd));
    Add(evt);
  }
  double delT = difftime(time(0), tstart);
  cout << " This took " << delT << " s";
  if(delT > 0){
    cout << " this is " << NumEvents/delT << " evts/s"
	 << " or " << (NumEvents/delT) * 60<< " evts/min";
  }
  cout << endl;

  return this->size();
}


// saving and retrieving:

TNtupleD* DalitzEventPtrList::makeNtuple(const bool addSij)const{
  return makeNtuple(className(), addSij);
}

/* Works under the assumptions that all events in the list
   follow the same pattern, e.g. all are KKpipi events.
   Probably still works if at least all are 3 or all are 4 body
   events.
*/

TNtupleD* DalitzEventPtrList::makeNtuple(const std::string& ntpName, const bool addSij ) const{
  
  if(this->empty()) return (TNtupleD*) 0;
  std::string varNameString= (*(this->begin()))->makeNtupleVarnames(addSij);
  TNtupleD* ntp = new TNtupleD(className().c_str()
			       , ntpName.c_str()
			       , varNameString.c_str());
  
  int arraySize = (*(this->begin()))->ntupleVarArraySize(addSij);
  Double_t *array = new Double_t[arraySize];


  for(vector<counted_ptr<DalitzEvent> >::const_iterator it = this->begin();
      it != this->end(); it++){

    bool success = (*it)->fillNtupleVarArray(array, arraySize, addSij);
    if(! success){
      cout << "ERROR in DalitzEvenPtrtList::makeNtuple"
	   << ", call to DalitzEvent::fillNtupleVarArray"
	   << " returned failure"
	   << endl;
    }else{
      ntp->Fill(array);
    }
  }
  delete[] array;
  return ntp;
}

DalitzHistoSet DalitzEventPtrList::histoSet() const{
  DalitzHistoSet hs;
  for(unsigned int i=0; i< this->size(); i++){
    hs.addEvent(&(*(*this)[i]));
  }
  return hs;
}
DalitzHistoSet DalitzEventPtrList::weightedHistoSet() const{
  // mainly for diagnostics
  DalitzHistoSet hs;
  for(unsigned int i=0; i< this->size(); i++){
    hs.addEvent(&(*(*this)[i]), (*(*this)[i]).getWeight());
  }
  return hs;
}
DalitzHistoSet DalitzEventPtrList::reWeightedHistoSet(IGetDalitzEvent* w) const{
  // mainly for diagnostics
  DalitzHistoSet hs;
  if(0 == w) return hs;
  for(unsigned int i=0; i< this->size(); i++){
    w->setEvent( &(*(*this)[i]));
    hs.addEvent(&(*(*this)[i]), w->RealVal());
    w->resetEventRecord();
  }
  return hs;
}

DalitzHistoSet DalitzEventPtrList::weighedReWeightedHistoSet(IGetDalitzEvent* w) const{
  // mainly for diagnostics
  DalitzHistoSet hs;
  if(0 == w) return hs;
  for(unsigned int i=0; i< this->size(); i++){
    w->setEvent( &(*(*this)[i]));
    hs.addEvent(&(*(*this)[i]), w->RealVal() * (*(*this)[i]).getWeight());
    w->resetEventRecord();
  }
  return hs;
}


bool DalitzEventPtrList::save(const std::string& fname, const bool addSij)const{
  return saveAsNtuple(fname, addSij);
}
bool DalitzEventPtrList::fromFile(const std::string& fname){
  return fromNtupleFile(fname);
}

bool DalitzEventPtrList::saveAsNtuple(const std::string& fname, const bool addSij
				   ) const{
  return saveAsNtuple(fname, className(), addSij);
}

bool DalitzEventPtrList::saveAsNtuple(const std::string& fname
				   , const std::string& ntpName
				   , const bool addSij
				   ) const{
  if(this->empty()){
    cout << "WARNING in DalitzEventPtrList::saveAsNtuple!"
	 << "\n\tyou are trying to save me to the file: "
	 << fname
	 << "\n\tbut I have only " << this->size()
	 << " events."
	 << " I won't create the file."
	 << endl;
    return false;
  }
  TFile f(fname.c_str(), "RECREATE");
  f.cd();
  TNtupleD* ntp = makeNtuple(ntpName, addSij);
  ntp->Write();
  f.Close();
  ntp->Delete("all");

  //  delete ntp;
  return true;
}

bool DalitzEventPtrList::fromNtuple(TTree* ntp){
  bool dbThis=false;
  if(dbThis) cout << "about to read ntuple with ptr " << ntp << endl;
  if(0==ntp) return false;
  if(ntp->GetEntries() <=0) return false;
  if(dbThis) cout << " number of entries: " << ntp->GetEntries() << endl;
  //if(dbThis) cout << " number of variables " << ntp->GetNvar() << endl;
  bool success=true;
  for(Long64_t i=0; i< ntp->GetEntries(); i++){
    if(dbThis){
      cout << "DalitzEventPtrList::fromNtuple "
	   << " getting " << i << " th entry" << endl;
    }
    ntp->GetEntry(i);
    if(dbThis) cout << " got it" << endl;
    counted_ptr<DalitzEvent> evt(new DalitzEvent);
    //    success &= evt.fromNtuple(ntp);
    success &= evt->fromTree(ntp);
    if(dbThis) cout << " made event" << endl;
    if(! success){
      cout << "ERROR in DalitzEventPtrList::fromNtuple"
	   << ", call to DalitzEvent::fromNtuple returned false!"
	   << endl;
      return false;
    }
    this->Add(evt);
    if(dbThis) cout << " added event" << endl;
  }
  if(dbThis) cout << "DalitzEventPtrList::fromNtuple worked!" << endl;
  return success;
}

bool DalitzEventPtrList::fromNtupleFile(const std::string& fname){
  TFile f(fname.c_str());
  f.cd();
  TTree* ntp = (TTree*) f.Get(className().c_str());
  if(0 == ntp){
    cout << "ERROR in DalitzEventPtrList::fromNtupleFile"
	 << "\n   > Can't get ntuple for filename = "
	 << "\n   > " << fname << endl;
    return false;
  }
  return fromNtuple(ntp);
}


//



//
