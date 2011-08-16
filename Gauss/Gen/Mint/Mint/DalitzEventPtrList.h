#ifndef DALITZEVENPTRTLIST_HH
#define DALITZEVENPTRTLIST_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:00 GMT

#include <string>

#include "EventPtrList.h"
#include "IDalitzEvent.h"
#include "IDalitzEventList.h"
#include "DalitzEventPattern.h"
#include "DalitzHistoSet.h"
//#include "IDalitzEventAccess.h"
//#include "IEventAccess.h"

#include "TNtupleD.h"

#include "IReturnReal.h"

#include "DalitzEvent.h"
#include "counted_ptr.h"

#include "PlotSet.h"
#include "CLHEPSystemOfUnits.h"

#include "IGetDalitzEvent.h"

class TRandom;
class TH1D;
class TH2D;
class TNtupleD;

class FitAmpSum;

class DalitzEventPtrList : 
public MINT::EventPtrList<IDalitzEvent, DalitzEvent>
  , public virtual IDalitzEventList
{
  static const std::string _className;
 protected:

 public:
  DalitzEventPtrList();
  DalitzEventPtrList(const DalitzEventPtrList& other);

  const std::string& className() const{return _className;}

  int generatePhaseSpaceEvents(int NumEvents
			       , const DalitzEventPattern& pat
			       , TRandom* rnd=0
			       );
  int generateEvents(unsigned int NumEvents
		     , const DalitzEventPattern& pat
		     , MINT::IGetRealEvent<IDalitzEvent>* amps
		     , TRandom* rnd
		     );

  DalitzHistoSet histoSet() const;
  DalitzHistoSet weightedHistoSet() const;
  DalitzHistoSet reWeightedHistoSet(IGetDalitzEvent* w) const;
  DalitzHistoSet weighedReWeightedHistoSet(IGetDalitzEvent* w) const;

  bool save(const std::string& fname = "DalitzEvents.root") const;
  bool fromFile(const std::string& fname = "DalitzEvents.root");

  TNtupleD* makeNtuple() const;
  TNtupleD* makeNtuple(const std::string& ntpName) const;

  bool saveAsNtuple(const std::string& fname="DalitzEvents.root")const;
  bool saveAsNtuple(const std::string& fname
		    , const std::string& ntpName
		    ) const;
  bool fromNtuple(TTree* ntp);
  bool fromNtupleFile(const std::string& fname="DalitzEvents.root");


};

#endif
//
