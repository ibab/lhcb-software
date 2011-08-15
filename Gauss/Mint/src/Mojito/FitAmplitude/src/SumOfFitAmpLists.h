#ifndef MINTDALITZ_SUM_OF_FIT_AMP_LISTS_HH
#define MINTDALITZ_SUM_OF_FIT_AMP_LISTS_HH

#include "ILookLikeFitAmpSum.h"
#include "IIntegrationCalculator.h"
#include "IFastAmplitudeIntegrable.h"
#include "IGetRealEvent.h"
#include "IDalitzEvent.h"

#include "counted_ptr.h"
#include "DalitzBWBoxSet.h"
#include "TRandom.h"
#include <iostream>


class SumOfFitAmpLists 
: virtual public MINT::IGetRealEvent<IDalitzEvent>
  , virtual public IFastAmplitudeIntegrable
  , virtual public ILookLikeFitAmpSum
  , DalitzEventAccess
{
  std::vector<MINT::counted_ptr<ILookLikeFitAmpSum> > _listOfLists;

 public:
  SumOfFitAmpLists(const DalitzEventPattern& pat);
  SumOfFitAmpLists(IDalitzEventAccess* eventAccess);
  SumOfFitAmpLists(IDalitzEventList* events);

  SumOfFitAmpLists(const MINT::counted_ptr<ILookLikeFitAmpSum>& list);
  SumOfFitAmpLists(const MINT::counted_ptr<ILookLikeFitAmpSum>& list_1
		  , const MINT::counted_ptr<ILookLikeFitAmpSum>& list_2);
  SumOfFitAmpLists(const MINT::counted_ptr<ILookLikeFitAmpSum>& list_1
		  , const MINT::counted_ptr<ILookLikeFitAmpSum>& list_2
		  , const MINT::counted_ptr<ILookLikeFitAmpSum>& list_3);
  SumOfFitAmpLists(const MINT::counted_ptr<ILookLikeFitAmpSum>& list_1
		  , const MINT::counted_ptr<ILookLikeFitAmpSum>& list_2
		  , const MINT::counted_ptr<ILookLikeFitAmpSum>& list_3
		  , const MINT::counted_ptr<ILookLikeFitAmpSum>& list_4);
  SumOfFitAmpLists(const MINT::counted_ptr<ILookLikeFitAmpSum>& list_1
		  , const MINT::counted_ptr<ILookLikeFitAmpSum>& list_2
		  , const MINT::counted_ptr<ILookLikeFitAmpSum>& list_3
		  , const MINT::counted_ptr<ILookLikeFitAmpSum>& list_4
		  , const MINT::counted_ptr<ILookLikeFitAmpSum>& list_5);
  SumOfFitAmpLists(const MINT::counted_ptr<ILookLikeFitAmpSum>& list_1
		  , const MINT::counted_ptr<ILookLikeFitAmpSum>& list_2
		  , const MINT::counted_ptr<ILookLikeFitAmpSum>& list_3
		  , const MINT::counted_ptr<ILookLikeFitAmpSum>& list_4
		  , const MINT::counted_ptr<ILookLikeFitAmpSum>& list_5
		  , const MINT::counted_ptr<ILookLikeFitAmpSum>& list_6);

  void addList(const MINT::counted_ptr<ILookLikeFitAmpSum>& list);

  
  virtual MINT::counted_ptr<IIntegrationCalculator> makeIntegrationCalculator();
  virtual MINT::counted_ptr<IntegCalculator> makeIntegCalculator();

  virtual double RealVal(); // | sum A |^2

  virtual DalitzBWBoxSet makeBWBoxes(TRandom* rnd=gRandom);
   
  virtual MINT::counted_ptr<MINT::IUnweightedEventGenerator<IDalitzEvent> > 
    makeEventGenerator(TRandom* rnd=gRandom);

  virtual void print(std::ostream& os=std::cout) const;

};

std::ostream& operator<<(std::ostream& os, const SumOfFitAmpLists& sofal);
#endif
//
