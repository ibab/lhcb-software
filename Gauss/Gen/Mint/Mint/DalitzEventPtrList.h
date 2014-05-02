#ifndef DALITZEVENPTRTLIST_HH
#define DALITZEVENPTRTLIST_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:00 GMT

#include <string>

#include "Mint/EventPtrList.h"
#include "Mint/IDalitzEvent.h"
#include "Mint/DalitzEventPattern.h"
#include "Mint/DalitzHistoSet.h"
//#include "Mint/IDalitzEventAccess.h"
//#include "Mint/IEventAccess.h"

#include "TNtupleD.h"

#include "Mint/IReturnReal.h"

#include "Mint/DalitzEvent.h"
#include "Mint/counted_ptr.h"

#include "Mint/PlotSet.h"
#include "Mint/CLHEPSystemOfUnits.h"

#include "Mint/IReturnRealForEvent.h"

class TRandom;
class TH1D;
class TH2D;
class TNtupleD;

class FitAmpSum;

class DalitzEventPtrList : 
public MINT::EventPtrList<DalitzEvent>
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
		     , MINT::IReturnRealForEvent<IDalitzEvent>* amps
		     , TRandom* rnd
		     );

  DalitzHistoSet histoSet() const;
  DalitzHistoSet weightedHistoSet() const;
  DalitzHistoSet reWeightedHistoSet(MINT::IReturnRealForEvent<IDalitzEvent>* w) ;
  DalitzHistoSet weighedReWeightedHistoSet(MINT::IReturnRealForEvent<IDalitzEvent>* w);

  bool save(const std::string& fname = "DalitzEvents.root", const bool addSij = false) const;
  bool fromFile(const std::string& fname = "DalitzEvents.root");

  TNtupleD* makeNtuple(const bool addSij = false) const;
  TNtupleD* makeNtuple(const std::string& ntpName, const bool addSij = false) const;

  bool saveAsNtuple(const std::string& fname="DalitzEvents.root", const bool addSij = false)const;
  bool saveAsNtuple(const std::string& fname
		    , const std::string& ntpName
		    , const bool addSij = false
		    ) const;
  bool fromNtuple(TTree* ntp);
  bool fromNtupleFile(const std::string& fname="DalitzEvents.root");


};

#endif
//
