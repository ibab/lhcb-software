#ifndef DALITZ_MCMC_HH
#define DALITZ_MCMC_HH
// author: Nunya Business (ya.mum@bristol.ac.uk)
// status:  Mon 19 Dec 2013 19:17:59 GMT

#include "TGenPhaseSpace.h"

#include "Mint/DalitzEventList.h"
#include "Mint/DalitzEventPattern.h"
#include "Mint/FitAmpSum.h"

class DalitzMCMC{ 
 public:
  DalitzMCMC( const DalitzEventPattern& pat,
	      const unsigned int& seed=0 );

  /**
     Uses the Metropolis-Hastings algorithm (Markov Chain) to sample
     a Dalitz plot ampitude-squared probability density and fill a
     DalitzEventList.
  */
  void FillEventList( DalitzEventList& evtList,
		      const unsigned int& NEvents );

  /**
     Performs hit-and-miss using TGenPhaseSpace to generate a uniformly
     distributed phase space event.
  */
  DalitzEvent GetFlatEvent();

 private:
  const DalitzEventPattern _pat;
  const unsigned int _nbody;

  MINT::MinuitParameterSet _myOwnPSet;
  FitAmpSum _pdf;

  TLorentzVector _P;
  TGenPhaseSpace _event;
};

#endif
//
