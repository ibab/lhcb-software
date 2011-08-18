#ifndef SIGNAL_GENERATOR_HH
#define SIGNAL_GENERATOR_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:59 GMT

#include "Mint/Mint/Events/IEventGenerator.h"
#include "Mint/Mojito/DalitzEvents/IDalitzEvent.h"
#include "Mint/Mojito/DalitzEvents/IDalitzEventList.h"

#include "Mint/Mojito/DalitzEventGeneration/BaseGenerator.h"

#include "Mint/Mojito/DalitzEvents/DalitzEventPattern.h"
#include "Mint/Mint/Minimiser/MinuitParameterSet.h"

#include "Mint/Mojito/DalitzEvents/DalitzEventList.h"
#include "Mint/Mojito/DalitzEvents/DiskResidentEventList.h"

#include "Mint/Mojito/DalitzIntegrator/IFastAmplitudeIntegrable.h"
#include "Mint/Mojito/BreitWignerMC/DalitzBWBoxSet.h"

#include "Mint/Mint/Utils/counted_ptr.h"

#include <string>

class SignalGenerator 
: public BaseGenerator
, virtual public MINT::IEventGenerator<IDalitzEvent>{
 protected:
  MINT::MinuitParameterSet _myOwnPSet;
  MINT::counted_ptr<IFastAmplitudeIntegrable> _counted_amps;
  IFastAmplitudeIntegrable* _amps;
  MINT::counted_ptr<MINT::IUnweightedEventGenerator<IDalitzEvent> >  _boxes;

  bool makeBoxes();
 public:
  SignalGenerator(const DalitzEventPattern& pat, TRandom* rnd=gRandom);
  SignalGenerator(IFastAmplitudeIntegrable* amps, TRandom* rnd=gRandom);
  SignalGenerator(const DalitzEventPattern& pat
		  , double rB
		  , double phase
		  , TRandom* rnd=gRandom);
  virtual MINT::counted_ptr<IDalitzEvent> tryDalitzEvent();
  virtual MINT::counted_ptr<IDalitzEvent> newDalitzEvent();

  // this one below is required by MINT::IEventGenerator<IDalitzEvent>
  // similar to above, except it's a pointer to 
  // IDalitzEvent, not DalitzEvent.
  //virtual MINT::counted_ptr<IDalitzEvent> newEvent();
  virtual MINT::counted_ptr<IDalitzEvent> newEvent();

  virtual bool exhausted() const{return false;}

  virtual bool ensureFreshEvents();
};

#endif
//
