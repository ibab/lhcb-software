#ifndef SIGNAL_GENERATOR_HH
#define SIGNAL_GENERATOR_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:59 GMT

#include "Mint/IEventGenerator.h"
#include "Mint/IDalitzEvent.h"
#include "Mint/IDalitzEventList.h"

#include "Mint/BaseGenerator.h"

#include "Mint/DalitzEventPattern.h"
#include "Mint/MinuitParameterSet.h"

#include "Mint/DalitzEventList.h"
#include "Mint/DiskResidentEventList.h"

#include "Mint/IFastAmplitudeIntegrable.h"
#include "Mint/DalitzBWBoxSet.h"

#include "Mint/counted_ptr.h"

#include <string>

class SignalGenerator 
: public BaseGenerator
, virtual public MINT::IEventGenerator<IDalitzEvent>{
 protected:
  DalitzEventPattern _pat;
  MINT::MinuitParameterSet _myOwnPSet;
  // has its own parameter set, i.e. its own copy of fitparamerters
  // which ensures they don't change outside SignalGenerator's control

  MINT::counted_ptr<IFastAmplitudeIntegrable> _counted_amps;
  IFastAmplitudeIntegrable* _amps;
  MINT::counted_ptr<MINT::IUnweightedEventGenerator<IDalitzEvent> >  _boxes;

  bool makeBoxes();

  MINT::MinuitParameterSet* myMPS();

 public:
  SignalGenerator(const DalitzEventPattern& pat
		  , TRandom* rnd=gRandom
		  );
  SignalGenerator(const DalitzEventPattern& pat
		  , IFastAmplitudeIntegrable* amps
		  , TRandom* rnd=gRandom
		  );
  SignalGenerator(const DalitzEventPattern& pat
		  , double rB
		  , double phase
		  , TRandom* rnd=gRandom
		  );

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
