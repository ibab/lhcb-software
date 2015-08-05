#ifndef IFAST_AMPLITUDE_INTEGRABLE_HH
#define IFAST_AMPLITUDE_INTEGRABLE_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:02 GMT

#include "Mint/counted_ptr.h"
#include "Mint/IIntegrationCalculator.h"
#include "Mint/IReturnRealForEvent.h"
#include "Mint/IDalitzEvent.h"
#include "Mint/IUnweightedEventGenerator.h"
#include "TRandom.h"
#include "Mint/DalitzEventPattern.h"

#include "Mint/IntegCalculator.h"

#include "Mint/DalitzEventPattern.h"

#include <iostream>

class IFastAmplitudeIntegrable 
: virtual public MINT::IReturnRealForEvent<IDalitzEvent>{
 public:
  virtual MINT::counted_ptr<IIntegrationCalculator> makeIntegrationCalculator()=0;
  virtual MINT::counted_ptr<IntegCalculator> makeIntegCalculator()=0;
  virtual MINT::counted_ptr<FitAmpPairList> makeFitAmpPairList()=0;

  virtual MINT::counted_ptr<MINT::IUnweightedEventGenerator<IDalitzEvent> > 
    makeEventGenerator(const DalitzEventPattern& pat
		       , TRandom* rnd=gRandom)=0;

  virtual void print(std::ostream& os=std::cout) const=0;

    virtual void Gradient(IDalitzEvent& evt,Double_t* grad,MINT::MinuitParameterSet* mps){
        std::cout << "Gradient of pdf is not implemented. Please implement me or set useAnalyticGradient to 0 in your options file. I'll crash now. " << std::endl;
        throw "crash";
    }
  virtual bool useAnalyticGradient() {return false;}
  
  virtual ~IFastAmplitudeIntegrable(){}
};

#endif
//
