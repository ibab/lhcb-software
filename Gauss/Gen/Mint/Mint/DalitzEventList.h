#ifndef DALITZEVENTLIST_HH
#define DALITZEVENTLIST_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:00 GMT

#include <string>

#include "Mint/EventList.h"
#include "Mint/IDalitzEvent.h"
#include "Mint/IDalitzEventList.h"
#include "Mint/IGetDalitzEvent.h"
#include "Mint/DalitzEvent.h"
#include "Mint/DalitzEventPattern.h"
//#include "Mint/IDalitzEventAccess.h"
//#include "Mint/IEventAccess.h"
#include "Mint/DalitzHistoSet.h"

#include "Mint/PlotSet.h"
#include "Mint/CLHEPSystemOfUnits.h"

class TRandom;
class TH1D;
class TH2D;
class TTree;

class FitAmpSum;

namespace MINT{
  class IReturnReal;
}

class DalitzEventList 
: public MINT::EventList<IDalitzEvent, DalitzEvent >
{
  static const std::string _className;
 protected:

 public:
  DalitzEventList();
  DalitzEventList(const DalitzEventList& other);
  DalitzEventList(TNtupleD*);

  const std::string& className() const{return _className;}

  //  virtual bool Add(const DalitzEvent& evt);
  // virtual bool Add(const MINT::counted_ptr<DalitzEvent>& evtPtr);

  int generatePhaseSpaceEvents(int NumEvents
			       , const DalitzEventPattern& pat
			       , TRandom* rnd=0
			       );
  int generateEvents(unsigned int NumEvents
		     , const DalitzEventPattern& pat
		     , MINT::IGetRealEvent<IDalitzEvent>* amps
		     , TRandom* rnd
		     );

  TH1D* makePlot(const std::vector<int> sij
		 , const std::string& name
		 , MINT::IReturnReal* weightFunction=0
		 , int nbins = 100
		 , double units = GeV*GeV
		 , char opt = 's'
		 );

  TH2D* makePlot2D(const std::vector<int> sijIndicesX
		   ,const std::vector<int> sijIndicesY
		   , const std::string& name
		   , MINT::IReturnReal* weightFunction =0
		   , int nbins =10
		   , double units = GeV*GeV
		   , char opt ='s'// = s, m
		   );

  TNtupleD* makePlotNtp(  const std::string& name_prefix="DalitzPlotNtp"
			  , MINT::IReturnReal* weightFunction =0
			  , double units = GeV*GeV
			  );

  DalitzHistoSet histoSet() const;
  DalitzHistoSet weightedHistoSet() const;
  DalitzHistoSet reWeightedHistoSet(IGetDalitzEvent* w) ;
  DalitzHistoSet weighedReWeightedHistoSet(IGetDalitzEvent* w) ;

  bool makePlots(const std::string& filename) const;

 /*
    units could be GeV*GeV, or MeV*MeV, where GeV
    and MeV are from CLHEPSystemsOfUnits.h
    weight is a function/class ptr. Set to zero to
    just fill the histograms with events (weight=1)
  */


  PlotSet makeAllPlots( const std::string& name_prefix
			, MINT::IReturnReal* weightFunction=0
			, int nbins1D = 100
			, int nbins2D = 10
			, double units = GeV*GeV
			);
  

  bool save(const std::string& fname = "DalitzEvents.root") const;
  bool fromFile(const std::string& fname = "DalitzEvents.root");

  TNtupleD* makeNtuple() const;
  TNtupleD* makeNtuple(const std::string& ntpName) const;

  bool saveAsNtuple(const std::string& fname="DalitzEvents.root")const;
  bool saveAsNtuple(const std::string& fname
		    , const std::string& ntpName
		    ) const;
  bool fromNtuple(TTree* ntp);
  bool fromNtuple(TTree* ntp, double num);

  bool fromNtupleFile(const std::string& fname="DalitzEvents.root");

};

#endif
//
