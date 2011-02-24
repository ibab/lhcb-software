// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:00 GMT
#include "DalitzEventPtrList.h"
#include "DalitzEvent.h"
#include "FitAmpSum.h"
#include "AllPossibleSij.h"

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

//
