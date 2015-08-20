#ifndef MULTITOPHAT_HH
#define MULTITOPHAT_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:03 GMT

#include <iostream>

#include "Mint/counted_ptr.h"

#include "Mint/DalitzBoxSet.h"
#include "Mint/DalitzBWBoxSet.h"
#include "Mint/IntegCalculator.h"

#include "Mint/IReturnRealForEvent.h"
#include "Mint/IReturnComplexForEvent.h"
#include "Mint/IFastAmplitudeIntegrable.h"
#include "Mint/IIntegrationCalculator.h"

#include "Mint/ILookLikeFitAmpSum.h"

#include "Mint/FitAmpListBase.h"

class MultiTopHat 
 : virtual public MINT::IReturnRealForEvent<IDalitzEvent>
 , virtual public MINT::IReturnComplexForEvent<IDalitzEvent>
 , virtual public IFastAmplitudeIntegrable
 , virtual public ILookLikeFitAmpSum
 , public FitAmpListBase
{
 protected:
 public:
  MultiTopHat();
  MultiTopHat(const MultiTopHat& other);
  MultiTopHat(const FitAmpListBase& other);
  /* 
     The copy constructor copies like this: There'll be 'physical'
     copies of all Amplitudes, but the FitParameters remain the
     same (pointers to the same FitParameter Object).  This is
     useful for the CP-conj coding as it is now, but perhaps a bit
     counter-intuitive.  needs to be reviewed at some point. This
     behaviour is defined in the copy constructor of the
     FitAmplitude class.
  */
  virtual MINT::counted_ptr<FitAmpListBase> GetCloneSameFitParameters() const;


  virtual DalitzBoxSet makeBoxes(const DalitzEventPattern& pat
				 , double nSigma=2){
    return FitAmpListBase::makeBoxes(pat, this, nSigma);}

  virtual DalitzBWBoxSet makeBWBoxes(const DalitzEventPattern& pat
				     , TRandom* rnd=gRandom){
    return FitAmpListBase::makeBWBoxes(pat, this, rnd);}


  std::complex<double> getVal(IDalitzEvent& evt);
  std::complex<double> getVal(IDalitzEvent* evtPtr){
    if(0 == evtPtr) return 0;
    return getVal(*evtPtr);
  }


  virtual MINT::counted_ptr<IIntegrationCalculator> makeIntegrationCalculator();
  virtual MINT::counted_ptr<IntegCalculator> makeIntegCalculator();
  virtual MINT::counted_ptr<FitAmpPairList> makeFitAmpPairList();

  virtual double Prob(IDalitzEvent& evt){
    std::complex<double> res = getVal(evt);
    return res.real()*res.real() + res.imag()*res.imag();
  }

  virtual std::complex<double> ComplexVal(IDalitzEvent& evt){return getVal(evt);}

  virtual double RealVal(IDalitzEvent& evt){
    return Prob(evt);
  }

  /*
  virtual double SmootherLargerRealVal(){
    return SmootherLargerProb();
  }
  */

  virtual MINT::counted_ptr<MINT::IUnweightedEventGenerator<IDalitzEvent> > 
    makeEventGenerator(const DalitzEventPattern& pat, TRandom* rnd=gRandom){
    MINT::counted_ptr<MINT::IUnweightedEventGenerator<IDalitzEvent> > 
      ptr(new DalitzBWBoxSet(makeBWBoxes(pat, rnd)));
    return ptr;
  }

  virtual void print(std::ostream& os=std::cout) const{
    FitAmpListBase::print(os);}
  virtual void printNonZero(std::ostream& os=std::cout) const{
    FitAmpListBase::printNonZero(os);}

  virtual ~MultiTopHat();

  friend class FitAmplitude;

  MultiTopHat& operator*=(double r);
  MultiTopHat& operator*=(const std::complex<double>& z);
  MultiTopHat& operator*=(const MINT::counted_ptr<MINT::IReturnComplex>& irc);

  MultiTopHat operator*(double r) const;
  MultiTopHat operator*(const std::complex<double>& z) const;
  MultiTopHat operator*(const MINT::counted_ptr<MINT::IReturnComplex>& irc) const;


  MultiTopHat& operator=(const MultiTopHat& other);
  MultiTopHat& operator=(const FitAmpListBase& other);
  MultiTopHat& operator+=(const MultiTopHat& other);
  MultiTopHat operator+(const MultiTopHat& other) const;

};

MultiTopHat operator*(double r, const MultiTopHat& rhs);
MultiTopHat operator*(const std::complex<double>& z, const MultiTopHat& rhs);
MultiTopHat operator*(const MINT::counted_ptr<MINT::IReturnComplex>& irc
		     , const MultiTopHat& rhs);


#endif
//
