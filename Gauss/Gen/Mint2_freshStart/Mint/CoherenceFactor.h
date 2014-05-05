#ifndef COHERENCE_FACTOR_OUTDATED_HH
#define COHERENCE_FACTOR_OUTDATED_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:59 GMT
//
// Using Equations 5 (and around that) in
// Phys. Rev. D 68, 033003 (2003), 
// http://prola.aps.org/abstract/PRD/v68/i3/e033003
//

#include "Mint/IDalitzEvent.h"

#include "Mint/IEventGenerator.h"

#include "Mint/IReturnComplexForEvent.h"
#include "Mint/IReturnRealForEvent.h"

#include "Mint/FitAmpSum.h"

#include "Mint/counted_ptr.h"

#include <complex>
#include <iostream>

class CoherenceFactor{

  //  MINT::counted_ptr<IDalitzEvent> _dummyEvent;

  FitAmpSum* _A, *_Abar;
  double _CSAbs;
  double _CSPhase;

  FitAmpSum _A_plus_Abar;
  MINT::counted_ptr< MINT::IEventGenerator<IDalitzEvent> > _myOwnGenAplusAbar;
  MINT::IReturnRealForEvent<IDalitzEvent>* _eff;
  double _precision;

  double _sumASq,    _sumASqSquared;
  double _sumAbarSq, _sumAbarSqSquared;
  std::complex<double> _sumAxAbarStar, _sumAxAbarStarSquared;
  // watchi it, _sumAxAbarStarSquared is (sum real^2, sum imag^2) 
  long int _Nevents;


  MINT::IReturnComplexForEvent<IDalitzEvent>* A(){
    return (MINT::IReturnComplexForEvent<IDalitzEvent>*)_A;}
  MINT::IReturnComplexForEvent<IDalitzEvent>* Abar(){
    return (MINT::IReturnComplexForEvent<IDalitzEvent>*) _Abar;}
  std::complex<double> A_Value(IDalitzEvent& evt);
  std::complex<double> Abar_Value(IDalitzEvent& evt);

  MINT::counted_ptr< MINT::IEventGenerator<IDalitzEvent> > getGenerator();

  MINT::counted_ptr<IDalitzEvent> newEvent();
  
  std::complex<double> integAxAbarStar()const;
  double integA()const;
  double integAbar()const;

  double integASq()const;
  double integAbarSq()const;

  std::complex<double> complexVar(std::complex<double> s
				  , std::complex<double> s_sq
				  )const;
  double realAorAbarVar(double s, double sq)const;

  std::complex<double> cvarAxAbarStar() const;
  double varA()const;
  double varAbar()const;

  std::complex<double> Rval() const;

  double getEff(IDalitzEvent& evt);
  double getEff(MINT::counted_ptr<IDalitzEvent> evtPtr){
    if(0 == evtPtr) return 0;
    return getEff(*evtPtr);// just for backward compatibility.
  }

 public:
  CoherenceFactor(FitAmpSum& A, FitAmpSum& Abar
		  , double CSAbs = 1
		  , double CSPhase = 0.0
		  , MINT::IReturnRealForEvent<IDalitzEvent>* eff=0
		  , double prec=1.e-3);

  std::complex<double> var()const;
  std::complex<double> sigma()const;
  double varAbs()const;
  double sigmaAbs()const;
  double varAngle()const;
  double sigmaAngle()const;
  void printResult(std::ostream& os = std::cout) const;

  void setPrecision(double prec){_precision = prec;}
  double estimatedPrecision() const;

  std::complex<double> getCVal();

};
#endif
//
