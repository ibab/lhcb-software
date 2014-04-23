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
  : EventPtrList<DalitzEvent>() 
{
}

DalitzEventPtrList::DalitzEventPtrList( const DalitzEventPtrList& other ) 
  : IEventList<DalitzEvent>()
  , EventPtrList<DalitzEvent>(other) 
{
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

TNtupleD* DalitzEventPtrList::makeNtuple()const{
  return makeNtuple(className());
}

/* Works under the assumptions that all events in the list
   follow the same pattern, e.g. all are KKpipi events.
   Probably still works if at least all are 3 or all are 4 body
   events.
*/

TNtupleD* DalitzEventPtrList::makeNtuple(const std::string& ntpName ) const{
  
  if(this->empty()) return (TNtupleD*) 0;
  std::string varNameString= (*(this->begin()))->makeNtupleVarnames();
  TNtupleD* ntp = new TNtupleD(className().c_str()
			       , ntpName.c_str()
			       , varNameString.c_str());
  
  int arraySize = (*(this->begin()))->ntupleVarArraySize();
  Double_t *array = new Double_t[arraySize];


  for(vector<counted_ptr<DalitzEvent> >::const_iterator it = this->begin();
      it != this->end(); it++){

    bool success = (*it)->fillNtupleVarArray(array, arraySize);
    if(! success){
      cout << "ERROR in DalitzEventListt::makeNtuple"
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
    DalitzEvent evt((*this)[i]);
    hs.addEvent(evt);
  }
  return hs;
}
DalitzHistoSet DalitzEventPtrList::weightedHistoSet() const{
  // mainly for diagnostics
  DalitzHistoSet hs;
  for(unsigned int i=0; i< this->size(); i++){
    DalitzEvent evt((*this)[i]);
    hs.addEvent(evt, evt.getWeight());
  }
  return hs;
}
DalitzHistoSet DalitzEventPtrList::reWeightedHistoSet(IReturnRealForEvent<IDalitzEvent>* w){
  // mainly for diagnostics
  DalitzHistoSet hs;
  if(0 == w) return hs;
  for(unsigned int i=0; i< this->size(); i++){
    DalitzEvent evt((*this)[i]);
    hs.addEvent(evt, w->RealVal(evt) );
  }
  return hs;
}

DalitzHistoSet DalitzEventPtrList::weighedReWeightedHistoSet(IReturnRealForEvent<IDalitzEvent>* w){
  // mainly for diagnostics
  DalitzHistoSet hs;
  if(0 == w) return hs;
  for(unsigned int i=0; i< this->size(); i++){
    DalitzEvent evt((*this)[i]);
    hs.addEvent(evt, w->RealVal(evt) * evt.getWeight());
  }
  return hs;
}


bool DalitzEventPtrList::save(const std::string& fname)const{
  return saveAsNtuple(fname);
}
bool DalitzEventPtrList::fromFile(const std::string& fname){
  return fromNtupleFile(fname);
}

bool DalitzEventPtrList::saveAsNtuple(const std::string& fname
				   ) const{
  return saveAsNtuple(fname, className());
}

bool DalitzEventPtrList::saveAsNtuple(const std::string& fname
				   , const std::string& ntpName
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
  TNtupleD* ntp = makeNtuple(ntpName);
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
