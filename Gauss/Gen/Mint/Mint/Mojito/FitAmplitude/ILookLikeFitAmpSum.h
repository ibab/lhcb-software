#ifndef MINTDALITZ_ILOOKLIKE_FITAMPSUM_HH
#define MINTDALITZ_ILOOKLIKE_FITAMPSUM_HH

#include "Mint/Mojito/FitAmplitude/ILookLikeFitAmpSum.h"
#include "Mint/Mojito/DalitzIntegrator/IIntegrationCalculator.h"
#include "Mint/Mojito/FitAmplitude/IntegCalculator.h"
#include "Mint/Mint/Events/IGetRealEvent.h"
#include "Mint/Mojito/DalitzEvents/IDalitzEvent.h"
#include "Mint/Mojito/DalitzIntegrator/IFastAmplitudeIntegrable.h"

#include "Mint/Mint/Utils/counted_ptr.h"
#include "Mint/Mojito/BreitWignerMC/DalitzBWBoxSet.h"
#include "TRandom.h"
#include <iostream>

class ILookLikeFitAmpSum
: virtual public MINT::IGetRealEvent<IDalitzEvent>
, virtual public IFastAmplitudeIntegrable
{
 public:
  virtual MINT::counted_ptr<IIntegrationCalculator> makeIntegrationCalculator()=0;
  virtual MINT::counted_ptr<IntegCalculator> makeIntegCalculator()=0;

  virtual double RealVal()=0; // | sum A |^2

  virtual MINT::counted_ptr<MINT::IUnweightedEventGenerator<IDalitzEvent> > 
    makeEventGenerator(TRandom* rnd=gRandom)=0;

  virtual void print(std::ostream& os=std::cout) const=0;

  virtual DalitzBWBoxSet makeBWBoxes(TRandom* rnd=gRandom)=0;

  virtual ~ILookLikeFitAmpSum(){};

};

#endif
//
