#ifndef BASE_GENERATOR_HH
#define BASE_GENERATOR_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:59 GMT

#include "Mint/Mint/Events/IEventGenerator.h"
#include "Mint/Mojito/DalitzEvents/IDalitzEvent.h"
#include "Mint/Mojito/DalitzEvents/IDalitzEventList.h"

#include "Mint/Mojito/DalitzEvents/DalitzEventPattern.h"
#include "Mint/Mint/Minimiser/MinuitParameterSet.h"

#include "Mint/Mojito/DalitzEvents/DalitzEventList.h"
#include "Mint/Mojito/DalitzEvents/DiskResidentEventList.h"

#include "Mint/Mojito/FitAmplitude/FitAmpSum.h"
#include "Mint/Mojito/BreitWignerMC/DalitzBWBoxSet.h"

#include "Mint/Mint/IReturnIntefaces/IReturnReal.h"

#include "Mint/Mint/Utils/counted_ptr.h"

#include "TRandom.h"

#include <string>

class BaseGenerator : virtual public MINT::IEventGenerator<IDalitzEvent> {
 protected:
  std::string _fname;
  bool _unWeighted;
  MINT::counted_ptr<DiskResidentEventList> _evtList;

  bool saveEvents(){return 0 != _evtList;}
  //virtual MINT::counted_ptr<DalitzEvent> tryDalitzEvent()=0;

  TRandom* _rnd;
  
  MINT::counted_ptr<MINT::IReturnReal> _mums_p;
  MINT::counted_ptr<MINT::IReturnReal> _mums_cosTheta;
  MINT::counted_ptr<MINT::IReturnReal> _mums_phi;
  
  TVector3 mothers3Momentum() const;

 public:
  BaseGenerator(TRandom* rnd=gRandom);
  BaseGenerator(const BaseGenerator& other);

  void setMothers3Momentum(MINT::counted_ptr<MINT::IReturnReal> p
			   , MINT::counted_ptr<MINT::IReturnReal> cosTheta
			   = (MINT::counted_ptr<MINT::IReturnReal>)0
			   , MINT::counted_ptr<MINT::IReturnReal> phi
			   = (MINT::counted_ptr<MINT::IReturnReal>)0
			   );
  void unsetMothers3Momentum();
  bool mothers3MomentumIsSet() const;

  void setSaveEvents(const std::string& fname="GeneratorEvents.root"
		     , const std::string& opt="RECREATE");
  void dontSaveEvents();
  void setUnweighted(bool uw=true){_unWeighted = uw;}
  void setWeighted(bool w=true){_unWeighted = ! w;}

  void FillEventList(DalitzEventList& evtList, int NEvents);
  void FillEventList(DiskResidentEventList& evtList, int NEvents);

  // this one below is required by MINT::IEventGenerator<IDalitzEvent>
  virtual MINT::counted_ptr<IDalitzEvent> newEvent()=0;

  virtual bool exhausted() const{return false;}
  virtual bool ensureFreshEvents();

  virtual ~BaseGenerator();
};

#endif
//
