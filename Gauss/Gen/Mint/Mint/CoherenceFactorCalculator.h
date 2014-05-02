#ifndef COHERENCE_FACTOR_CALCULATOR_HH
#define COHERENCE_FACTOR_CALCULATOR_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:59 GMT
//
// Using Equations 5 (and around that) in
// Phys. Rev. D 68, 033003 (2003), 
// http://prola.aps.org/abstract/PRD/v68/i3/e033003
//

#include "Mint/IReturnComplexForEvent.h"
#include "Mint/IReturnRealForEvent.h"

#include "Mint/IDalitzEvent.h"

#include "Mint/IEventGenerator.h"

#include "Mint/FitAmpSum.h"

#include "Mint/counted_ptr.h"

#include "Mint/CoherenceFactorStoreAndEvaluate.h"

#include <complex>
#include <iostream>

class CoherenceFactorCalculator{

  //  MINT::counted_ptr<IDalitzEvent> _dummyEvent;

  FitAmpSum _A_plus_Abar;
  MINT::counted_ptr< MINT::IEventGenerator<IDalitzEvent> > _myOwnGenAplusAbar;
  double _precision;
  std::string _name;

  MINT::counted_ptr< MINT::IEventGenerator<IDalitzEvent> > getGenerator();

  MINT::counted_ptr<IDalitzEvent> newEvent();

  std::vector< MINT::counted_ptr< CoherenceFactorStoreAndEvaluate> > _cfList;

 public:
  CoherenceFactorCalculator(FitAmpSum& A, FitAmpSum& Abar
			    , double CSAbs = 1
			    , double CSPhase = 0.0
			    , MINT::IReturnRealForEvent<IDalitzEvent>* eff=0
			    , double prec=1.e-3
			    , const std::string& name = "coherenceFactorC"
			    );

  void printResult(std::ostream& os = std::cout) const;

  std::complex<double> evaluate();

  void setPrecision(double prec);
  double estimatedPrecision() const;

  double effA() const;
  
  double effAbar() const;
  double effAVar() const;
  double effAbarVar() const;
  double effASigma() const;
  double effAbarSigma() const;
};
#endif
//
