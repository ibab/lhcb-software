#ifndef DALITZ_MC_INTEGRATOR_HH
#define DALITZ_MC_INTEGRATOR_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:02 GMT

#include "IGetRealEvent.h"
#include "IDalitzIntegrator.h"

#include "DalitzEventPattern.h"

#include "IDalitzEvent.h"
#include "DalitzEventPtrList.h"

#include "TRandom.h"

#include "IEventGenerator.h"

/*
  WARNING: This piece of code is much slower and less accurate than
  "FastAmplitudeIntegrator", as used by DalitzPdfBaseFastInteg.
  Still, keep the code for x-checks.
 */

class DalitzMCIntegrator : virtual public IDalitzIntegrator{
  mutable double _mean, _variance;
  const static int _minEvents=1000;
  int _numEvents;

  bool _initialised;
 protected:

  DalitzEventPattern _pat;
  MINT::IGetRealEvent<IDalitzEvent>* _w;
  DalitzEventPtrList _events;
  TRandom* _rnd;
  double _precision;

  int updateEventSet(int Nevents);
  double evaluateSum();
  int determineNumEvents();
  int generateEnoughEvents();
  int addEvents(int Nevents);

  MINT::IEventGenerator<IDalitzEvent>* _generator;
 public:
  DalitzMCIntegrator(const DalitzEventPattern& pattern
		     , MINT::IGetRealEvent<IDalitzEvent>* weightFunction=0
		     , MINT::IEventGenerator<IDalitzEvent>* eventGenerator=0
		     , TRandom* rnd = gRandom
		     , double precision = 1.e-2
		     );
  DalitzMCIntegrator();
  
  bool initialise(const DalitzEventPattern& pattern
		     , MINT::IGetRealEvent<IDalitzEvent>* weightFunction=0
		     , MINT::IEventGenerator<IDalitzEvent>* eventGenerator=0
		     , TRandom* rnd = gRandom
		     , double precision = 1.e-2
		  );

  bool resetIntegrand(MINT::IGetRealEvent<IDalitzEvent>* 
		      weightFunction = 0
		      );

  void setPrecision(double prec){
    _precision=prec;
    if(initialised()){
      generateEnoughEvents();
    }
  }

  // warning - this will not re-evaluate the #events needed for
  // the precision etc.

  bool initialised() const{
    return _initialised;
  }

  double getVal();

  double RealVal(){
    return getVal();
  }

  virtual void doFinalStats(MINT::Minimiser* mini=0);

};
#endif
//
