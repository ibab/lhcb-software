#ifndef DALITZEVENPTRTLIST_HH
#define DALITZEVENPTRTLIST_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:00 GMT

#include <string>

#include "Mint/Mint/Events/EventPtrList.h"
#include "Mint/Mojito/DalitzEvents/IDalitzEvent.h"
#include "Mint/Mojito/DalitzEvents/IDalitzEventList.h"
#include "Mint/Mojito/DalitzEvents/DalitzEventPattern.h"
#include "Mint/Mojito/BreitWignerMC/DalitzHistoSet.h"
//#include "Mint/Mojito/DalitzEvents/IDalitzEventAccess.h"
//#include "Mint/Mint/Events/IEventAccess.h"

#include "TNtupleD.h"

#include "Mint/Mint/IReturnIntefaces/IReturnReal.h"

#include "Mint/Mojito/DalitzEvents/DalitzEvent.h"
#include "Mint/Mint/Utils/counted_ptr.h"

#include "Mint/Mojito/DalitzEvents/PlotSet.h"
#include "Mint/Mint/Utils/CLHEPSystemOfUnits.h"

#include "Mint/Mojito/DalitzEvents/IGetDalitzEvent.h"

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
