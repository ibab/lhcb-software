// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:00 GMT
#include "Mint/DalitzEventList.h"
#include "Mint/DalitzEvent.h"
#include "Mint/FitAmpSum.h"
#include "Mint/AllPossibleSij.h"

#include "Mint/IReturnReal.h"

#include "TRandom.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TNtupleD.h"
#include "TObjString.h"

#include "TTree.h"

#include "Mint/counted_ptr.h"

#include <iostream>
#include <ctime>

using namespace std;
using namespace MINT;

const std::string DalitzEventList::_className("DalitzEventList");


DalitzEventList::DalitzEventList() 
  : EventList<IDalitzEvent, DalitzEvent>() 
{
}

DalitzEventList::DalitzEventList( const DalitzEventList& other ) 
  : ILoopable()
  , IBasicEventAccess<IDalitzEvent>()
  , IEventList<IDalitzEvent>()
  , EventList<IDalitzEvent, DalitzEvent>(other) 
{
}

DalitzEventList::DalitzEventList(TNtupleD* ntp) 
  : EventList<IDalitzEvent, DalitzEvent>() 
{
  bool success = fromNtuple(ntp);
  if(! success){
    cout << "ERROR in DalitzEventList::DalitzEventList(TNtupleD* ntp)"
	 << "\n   > problem creating myself from ntuple"
	 << endl;
  }
}

int DalitzEventList::generateEvents(unsigned int NumEvents
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
  DalitzEventList newList;
  newList.generatePhaseSpaceEvents(N_toStartWith, pat, rnd);

  double max = -9999;
  newList.findMaxAndThrowAwayData(max, amps, NumEvents, rnd); 
  // throws away data & tells us estimated absolute max
  // Not the way we usually generate events anymore.
  this->Add(newList);

  amps->setEventRecord(&newList);
  int counter = 0;
  while(this->size()-startSize < NumEvents){
    counter++;
    bool printit = ! (counter%50000);
    DalitzEvent evt(pat, rnd);
    //counted_ptr<DalitzEvent> evtPtr(new DalitzEvent(pat, rnd));
    newList.clear();
    //newList.Add(evtPtr);
    newList.Add(evt);
    newList.Start();
    newList.Next();
    double d=amps->RealVal();
    
    if(d > max){
      cout << "DalitzEventList::generateEvents: ERROR: We're in some trouble: amps value = " << d
	   << "\n is larger than the estimated maximum of " << max
	   << "\n This could mean the data are worthless."
	   << " I'll continue generating anyway, but"
	   << "\n I'll re-set max to this maximum + 10% safety margin";
      max = d*1.1;
      cout << " = " << max << endl;
    }

    if(printit){
      cout << " DalitzEventList::generateEvents INFO: current size " 
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
      //      this->Add(evtPtr);
      this->Add(evt);
    }else{
      if(printit) cout << "no";
    }
    if(printit) cout << endl;
  }
  amps->resetEventRecord();

  cout << "DalitzEventList::generateEvents: generating " << this->size() 
       << " Events took: " <<  difftime(time(0), startTime)/60 << " minutes"
       << endl;
  return this->size();
}
int DalitzEventList::generatePhaseSpaceEvents(int NumEvents
					      , const DalitzEventPattern& pat
					      , TRandom* rnd
					      ){

  cout << "DalitzEventList::generatePhaseSpaceEvents "
       << " about to generate " << NumEvents 
       << " events to phase space" << endl;
  time_t tstart = time(0);
  int reportN = NumEvents/10; // 10 x at least;
  if(reportN > 10000) reportN = 10000; // at least once every 10k event
  for(int i=0; i< NumEvents; i++){
    if( i < 5 || (!(i%reportN))){
      cout << "DalitzEventList::generatePhaseSpaceEvents " 
	   << " generating " << i+1 << "th event."
	   << " with " << pat << " and " << rnd << endl;
    }
    Add(DalitzEvent(pat, rnd));
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
  
TH1D* DalitzEventList::makePlot(const std::vector<int> sijIndices
				, const std::string& name
				, IReturnReal* weightFunction //=0
				, int nbins //=100
				, double units //=1
				, char opt // = s, m
				){
  if(this->empty()) return (TH1D*)0;
  
  double eps=0.1;
  double mi = getFirstEvent()->sijMin(sijIndices)/units;
  double ma = getFirstEvent()->sijMax(sijIndices)/units;
  if(opt=='m'){
    mi = sqrt(mi);
    ma = sqrt(ma);
  }
  mi -= (ma-mi)*eps;
  ma += (ma-mi)*eps;

  TH1D* histo = new TH1D(name.c_str(), name.c_str(), nbins, mi, ma);

  Start();
  while(Next()){
    double w=1;
    if(0 != weightFunction) w = weightFunction->RealVal();
    double x=currentEvent()->sij(sijIndices)/units;
    if(opt == 'm') x = sqrt(x);
    histo->Fill(x, w);
  }
  return histo;
}

TH2D* DalitzEventList::makePlot2D(const std::vector<int> sijIndicesX
				  ,const std::vector<int> sijIndicesY
				  , const std::string& name
				  , IReturnReal* weightFunction //=0
				  , int nbins //=20
				  , double units //=1
				  , char opt // = s, m
				  ){
  if(this->empty()) return (TH2D*)0;
    
  
  double eps = 0.125;
  double miX = getFirstEvent()->sijMin(sijIndicesX)/units;
  double maX = getFirstEvent()->sijMax(sijIndicesX)/units;
  double miY = getFirstEvent()->sijMin(sijIndicesY)/units;
  double maY = getFirstEvent()->sijMax(sijIndicesY)/units;
  if(opt=='m'){
    miX = sqrt(miX);
    maX = sqrt(maX);
    miY = sqrt(miY);
    maY = sqrt(maY);
  }
  miX -= (maX - miX)*eps;
  maX += (maX - miX)*eps;
  miY -= (maY - miY)*eps;
  maY += (maY - miY)*eps;

  TH2D* histo = new TH2D(name.c_str()
			 , name.c_str()
			 , nbins, miX, maX
			 , nbins, miY, maY);

  Start();
  while(Next()){
    double w=1;
    if(0 != weightFunction) w = weightFunction->RealVal();
    double x=currentEvent()->sij(sijIndicesX)/units;
    double y=currentEvent()->sij(sijIndicesY)/units;
    if(opt == 'm'){
      x = sqrt(x);
      y = sqrt(y);
    }
    histo->Fill(x, y, w);
  }
  return histo;
}

TNtupleD* DalitzEventList::makePlotNtp(const std::string& name_prefix
				       , IReturnReal* weightFunction  // =0
				       , double units  // = GeV*GeV
				       ){
  std::string ntp_str="";
  int nd = getFirstEvent()->eventPattern().size()-1;
  if(nd < 3) return (TNtupleD*)0;
  AllPossibleSij sijList(nd);

  int arraySize = 0;
  for(namedVMap::const_iterator it = sijList.begin();
      it != sijList.end(); it++){    
    std::string entry_name = "s" + it->first + ":";
    ntp_str += entry_name;
    arraySize++;
  }
  for(namedVMap::const_iterator it = sijList.begin();
      it != sijList.end(); it++){    
    std::string entry_name = "m" + it->first + ":";
    ntp_str += entry_name;
    arraySize++;
  }
  ntp_str += "weight:";
  arraySize++;
  ntp_str += "fcn"; // last one doesn't get a ":

  std::string ntp_id = className() + "_sij";
  std::string name   = name_prefix + "_sij";
  TNtupleD* ntp = new TNtupleD(ntp_id.c_str()
			       , name.c_str()
			       , ntp_str.c_str());

  Double_t* array = new Double_t[arraySize];

  while(Next()){
    int arrayIndex = 0;
    for(namedVMap::const_iterator it = sijList.begin();
	it != sijList.end(); it++){
      array[arrayIndex]=currentEvent()->sij(it->second)/units;
      arrayIndex++;
    }
    for(namedVMap::const_iterator it = sijList.begin();
	it != sijList.end(); it++){    
      std::string entry_name = "m" + it->first;
      double sijtemp = currentEvent()->sij(it->second)/units;
      if(sijtemp >=0){
	array[arrayIndex]=sqrt(sijtemp);
      }else{
	array[arrayIndex] = -9999.;
      }
      arrayIndex++;
    }      
    array[arrayIndex] = currentEvent()->getWeight();
    arrayIndex++;
    double fcn=1;
    if(0 != weightFunction) fcn = weightFunction->RealVal();
    array[arrayIndex] = fcn;
    arrayIndex++;

    ntp->Fill(array);
  }
  delete[] array;

  return ntp;
}
PlotSet DalitzEventList::makeAllPlots( const std::string& name_prefix
				       , IReturnReal* weightFunction //=0
				       , int nbins1D //= 100
				       , int nbins2D //= 20
				       , double units //= GeV*GeV
				       ){
  PlotSet ps;
  if(this->empty()) return ps;

  int nd = getFirstEvent()->eventPattern().size()-1;
  if(nd < 3) return ps;

  AllPossibleSij sijList(nd);
  
  for(namedVMap::const_iterator it = sijList.begin();
      it != sijList.end(); it++){    
    std::string name = name_prefix + "_s" + it->first;
      
    TH1D* newPlots = makePlot(it->second
			      , name
			      , weightFunction
			      , nbins1D
			      , units
			      , 's');
    if(0 != newPlots) ps.push_back(newPlots);
    
    name = name_prefix + "_m" + it->first;
    TH1D* newPlotm = makePlot(it->second
			      , name
			      , weightFunction
			      , nbins1D
			      , units
			      , 'm');
    if(0 != newPlotm) ps.push_back(newPlotm);
    namedVMap::const_iterator jt = it;
    jt++;
    for(; jt != sijList.end(); jt++){    
      std::string name2D = name_prefix + jt->first + "_vs_" + it->first;

      TH2D* newPlot2D = makePlot2D(it->second, jt->second, name2D
				   , weightFunction, nbins2D, units
				   , 's');

      if(0 != newPlot2D) ps.push_back(newPlot2D);
    }
  }
  TNtupleD* newPlotNtp = makePlotNtp(name_prefix, weightFunction, units);
  ps.push_back(newPlotNtp);

  return ps;  
}


TNtupleD* DalitzEventList::makeNtuple()const{
  return makeNtuple(className());
}

/* Works under the assumptions that all events in the list
   follow the same pattern, e.g. all are KKpipi events.
   Probably still works if at least all are 3 or all are 4 body
   events.
*/

TNtupleD* DalitzEventList::makeNtuple(const std::string& ntpName ) const{
  
  if(this->empty()) return (TNtupleD*) 0;
  std::string varNameString= ((this->begin()))->makeNtupleVarnames();
  TNtupleD* ntp = new TNtupleD(className().c_str()
			       , ntpName.c_str()
			       , varNameString.c_str());

  ntp->SetDirectory(0);

  int arraySize = ((this->begin()))->ntupleVarArraySize();
  Double_t *array = new Double_t[arraySize];


  for(vector<DalitzEvent>::const_iterator it = this->begin();
      it != this->end(); it++){

    bool success = (it)->fillNtupleVarArray(array, arraySize);
    if(! success){
      cout << "ERROR in DalitzEventList::makeNtuple"
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

bool DalitzEventList::save(const std::string& fname)const{
  return saveAsNtuple(fname);
}
bool DalitzEventList::fromFile(const std::string& fname){
  return fromNtupleFile(fname);
}

bool DalitzEventList::saveAsNtuple(const std::string& fname
				   ) const{
  return saveAsNtuple(fname, className());
}

bool DalitzEventList::saveAsNtuple(const std::string& fname
				   , const std::string& ntpName
				   ) const{
  if(this->empty()){
    cout << "WARNING in DalitzEventList::saveAsNtuple!"
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
  //delete ntp;
  return true;
}

bool DalitzEventList::fromNtuple(TTree* ntp){
  bool dbThis=false;
  if(dbThis) cout << "about to read ntuple with ptr " << ntp << endl;
  if(0==ntp) return false;
  if(ntp->GetEntries() <=0) return false;
  if(dbThis) cout << " number of entries: " << ntp->GetEntries() << endl;
  //if(dbThis) cout << " number of variables " << ntp->GetNvar() << endl;
  bool success=true;
  for(Long64_t i=0; i< ntp->GetEntries(); i++){
    if(dbThis){
      cout << "DalitzEventList::fromNtuple "
	   << " getting " << i << " th entry" << endl;
    }
    ntp->GetEntry(i);
    if(dbThis) cout << " got it" << endl;
    DalitzEvent evt;
    //    success &= evt.fromNtuple(ntp);
    success &= evt.fromTree(ntp);
    if(dbThis) cout << " made event" << endl;
    if(! success){
      cout << "ERROR in DalitzEventList::fromNtuple"
	   << ", call to DalitzEvent::fromNtuple returned false!"
	   << endl;
      return false;
    }
    this->Add(evt);
    if(dbThis) cout << " added event" << endl;
  }
  if(dbThis) cout << "DalitzEventList::fromNtuple worked!" << endl;
  return success;
}

bool DalitzEventList::fromNtuple(TTree* ntp, double num){
  bool dbThis=false;
  if(dbThis) cout << "about to read ntuple with ptr " << ntp << endl;
  if(0==ntp) return false;
  if(ntp->GetEntries() <=0) return false;
  if(dbThis) cout << " number of entries: " << ntp->GetEntries() << endl;
  //if(dbThis) cout << " number of variables " << ntp->GetNvar() << endl;
  TRandom Rand(500);
  bool success=true;
  for(Long64_t i=0; i< ntp->GetEntries(); i++){
    if(dbThis){
      cout << "DalitzEventList::fromNtuple "
	   << " getting " << i << " th entry" << endl;
    }
    double rand = Rand.Rndm();
    if (rand < num)
    {
		ntp->GetEntry(i);
		if(dbThis) cout << " got it" << endl;
		DalitzEvent evt;
		//    success &= evt.fromNtuple(ntp);
		success &= evt.fromTree(ntp);
		if(dbThis) cout << " made event" << endl;
		if(! success){
		  cout << "ERROR in DalitzEventList::fromNtuple"
		   << ", call to DalitzEvent::fromNtuple returned false!"
		   << endl;
		  return false;
		}
		this->Add(evt);
		if(dbThis) cout << " added event" << endl;
    }
  }
  if(dbThis) cout << "DalitzEventList::fromNtuple worked!" << endl;
  return success;
}

bool DalitzEventList::fromNtupleFile(const std::string& fname){
  TFile f(fname.c_str());
  f.cd();
  TTree* ntp = (TTree*) f.Get(className().c_str());
  if(0 == ntp){
    cout << "ERROR in DalitzEventList::fromNtupleFile"
	 << "\n   > Can't get ntuple for filename = "
	 << "\n   > " << fname << endl;
    return false;
  }
  return fromNtuple(ntp);
}

DalitzHistoSet DalitzEventList::histoSet() const{
  DalitzHistoSet hs;
  for(unsigned int i=0; i< this->size(); i++){
    hs.addEvent(&((*this)[i]));
  }
  return hs;
}
DalitzHistoSet DalitzEventList::weightedHistoSet() const{
  // mainly for diagnostics
  DalitzHistoSet hs;
  for(unsigned int i=0; i< this->size(); i++){
    hs.addEvent(&((*this)[i]), ((*this)[i]).getWeight());
  }
  return hs;
}
DalitzHistoSet DalitzEventList::reWeightedHistoSet(IGetDalitzEvent* w){
  // mainly for diagnostics
  DalitzHistoSet hs;
  if(0 == w) return hs;
  for(unsigned int i=0; i< this->size(); i++){
    w->setEvent( &((*this)[i]));
    hs.addEvent(&((*this)[i]), w->RealVal());
    w->resetEventRecord();
  }
  return hs;
}

DalitzHistoSet DalitzEventList::weighedReWeightedHistoSet(IGetDalitzEvent* w){
  DalitzHistoSet hs;
  if(0 == w) return hs;
  for(unsigned int i=0; i< this->size(); i++){
    w->setEvent( &((*this)[i]));
    hs.addEvent(&((*this)[i]), w->RealVal() * ((*this)[i]).getWeight());
    w->resetEventRecord();
  }
  return hs;
}

bool DalitzEventList::makePlots(const std::string& filename) const{
  histoSet().save(filename);
  return true;
}
//
