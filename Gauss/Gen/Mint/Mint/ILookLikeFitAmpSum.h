#ifndef MINTDALITZ_ILOOKLIKE_FITAMPSUM_HH
#define MINTDALITZ_ILOOKLIKE_FITAMPSUM_HH

#include "ILookLikeFitAmpSum.h"
#include "IIntegrationCalculator.h"
#include "IntegCalculator.h"
#include "IGetRealEvent.h"
#include "IDalitzEvent.h"
#include "IFastAmplitudeIntegrable.h"

#include "counted_ptr.h"
#include "DalitzBWBoxSet.h"
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
