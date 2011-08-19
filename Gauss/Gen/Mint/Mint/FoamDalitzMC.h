#ifndef MINTS_FOAM_DALTIZ_MC_GENERATOR_HH
#define MINTS_FOAM_DALTIZ_MC_GENERATOR_HH

#include "Mint/BaseGenerator.h"
#include "Mint/IDalitzEvent.h"
#include "Mint/IEventGenerator.h"
#include "Mint/IFastAmplitudeIntegrable.h"

#include "Mint/DalitzEventPattern.h"

#include "TFoam.h"

class FoamDalitzMC : BaseGenerator
, virtual public MINT::IEventGenerator<IDalitzEvent>{
 protected:
  TFoam _foam;
  
 public:
  FoamDalitzMC(const DalitzEventPattern& pat, TRandom* rnd=gRandom);
  FoamDalitzMC(IFastAmplitudeIntegrable* amps, TRandom* rnd=gRandom);

  virtual MINT::counted_ptr<IDalitzEvent> tryDalitzEvent();
  virtual MINT::counted_ptr<IDalitzEvent> newDalitzEvent();



  // the below is required by MINT::IEventGenerator<IDalitzEvent>
  virtual MINT::counted_ptr<IDalitzEvent> newEvent()=0;

  virtual bool exhausted() const{return false;}
  virtual bool ensureFreshEvents();

};

#endif
//
