#ifndef DALITZ_MC_INTEGRATOR_HH
#define DALITZ_MC_INTEGRATOR_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:02 GMT

#include "Mint/IDalitzIntegrator.h"
#include "Mint/IFastAmplitudeIntegrable.h"

#include "Mint/DalitzEventPattern.h"

#include "Mint/IDalitzEvent.h"
#include "Mint/DalitzEventPtrList.h"

#include "TRandom.h"

#include "Mint/IEventGenerator.h"

#include "Mint/DalitzHistoSet.h"
#include "Mint/IReturnRealForEvent.h"
#include "Mint/MinuitParameterSet.h"

/*
  WARNING: This piece of code is much slower and less accurate than
  "FastAmplitudeIntegrator", as used by DalitzPdfBaseFastInteg.
  Still, keep the code for x-checks.
 */

class DalitzMCIntegrator : virtual public IDalitzIntegrator{

  
  class integrationWeight : public MINT::IReturnRealForEvent<IDalitzEvent>{
    MINT::IReturnRealForEvent<IDalitzEvent>* _externalPdf;
  public:
    integrationWeight(MINT::IReturnRealForEvent<IDalitzEvent>* externalPdf);
    void setWeight(MINT::IReturnRealForEvent<IDalitzEvent>* pdf);
    double RealVal(IDalitzEvent& evt);
  };
  
  long int _Ncalls;
  double _sumT;

  mutable double _mean, _variance;
  const static int _minEvents=1000;
  int _numEvents;
  double _weightSum;

  bool _initialised;
 protected:
  MINT::MinuitParameterSet* _mps;
  DalitzEventPattern _pat;
  MINT::IReturnRealForEvent<IDalitzEvent>* _w;
  integrationWeight _iw;
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
                       , MINT::IReturnRealForEvent<IDalitzEvent>* weightFunction=0
                       , MINT::IEventGenerator<IDalitzEvent>* eventGenerator=0
                       , TRandom* rnd = gRandom
                       , double precision = 1.e-2
                       );
    DalitzMCIntegrator();
    
    bool initialise(const DalitzEventPattern& pattern
                    , MINT::IReturnRealForEvent<IDalitzEvent>* weightFunction=0
                    , MINT::IEventGenerator<IDalitzEvent>* eventGenerator=0
                    , TRandom* rnd = gRandom
                    , double precision = 1.e-2
                    );

  bool resetIntegrand(MINT::IReturnRealForEvent<IDalitzEvent>* 
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
    
  MINT::MinuitParameterSet* getMPS();
  const MINT::MinuitParameterSet* getMPS() const;  

  virtual void doFinalStats(MINT::Minimiser* mini=0);

  DalitzHistoSet histoSet(){
        double den= getVal();
        den *= _weightSum;
        if(den <= 0) den=1;
        return _events.reWeightedHistoSet(&_iw)/den;
  }
    
  virtual ~DalitzMCIntegrator(){};

};
#endif
//
