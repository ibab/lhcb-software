#ifndef SIGNAL_GENERATOR_HH
#define SIGNAL_GENERATOR_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:59 GMT

#include "IEventGenerator.h"
#include "IDalitzEvent.h"
#include "IDalitzEventList.h"

#include "BaseGenerator.h"

#include "DalitzEventPattern.h"
#include "MinuitParameterSet.h"

#include "DalitzEventList.h"
#include "DiskResidentEventList.h"

#include "IFastAmplitudeIntegrable.h"
#include "DalitzBWBoxSet.h"

#include "counted_ptr.h"

#include <string>


// HepMC
//#include "HepMC/GenEvent.h"

class SignalGenerator 
: public BaseGenerator
, virtual public MINT::IEventGenerator<IDalitzEvent>{
 protected:
  MINT::MinuitParameterSet _myOwnPSet;
  MINT::counted_ptr<IFastAmplitudeIntegrable> _counted_amps;
  IFastAmplitudeIntegrable* _amps;
  MINT::counted_ptr<MINT::IUnweightedEventGenerator<IDalitzEvent> >  _boxes;

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

  void MattTest()
  {
	  std::cout << "############# Matt's Test #############" << std::endl;
	  std::cout << "############# Matt's Test #############" << std::endl;
	  std::cout << "############# Matt's Test #############" << std::endl;
	  std::cout << "############# Matt's Test #############" << std::endl;
	  std::cout << "############# Matt's Test #############" << std::endl;
	  std::cout << "############# Matt's Test #############" << std::endl;
	  std::cout << "############# Matt's Test #############" << std::endl;
	  std::cout << "############# Matt's Test #############" << std::endl;
	  std::cout << "############# Matt's Test #############" << std::endl;

  }
  virtual bool ensureFreshEvents();
};

#endif
//
