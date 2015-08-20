#ifndef FITAMPSUM_HH
#define FITAMPSUM_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:03 GMT

#include <complex>
#include <iostream>

#include "Mint/DalitzBoxSet.h"
#include "Mint/DalitzBWBoxSet.h"
#include "Mint/IntegCalculator.h"
#include "Mint/IReturnRealForEvent.h"
#include "Mint/IReturnComplexForEvent.h"
#include "Mint/IFastAmplitudeIntegrable.h"
#include "Mint/IUnweightedEventGenerator.h"
#include "Mint/IIntegrationCalculator.h"
#include "Mint/NamedParameter.h"
#include "Mint/ILookLikeFitAmpSum.h"
#include "Mint/FitAmpList.h"

// note - this inherits now from FitAmpList, and that from FitAmpLisBase
// many methods formerly defined here are now defined there.

class FitAmpSum 
: virtual public MINT::IReturnRealForEvent<IDalitzEvent>
, virtual public MINT::IReturnComplexForEvent<IDalitzEvent>
, virtual public IFastAmplitudeIntegrable
, virtual public ILookLikeFitAmpSum
, public FitAmpList
{
 protected:
 MINT::NamedParameter<int> _useAnalyticGradient;     
 public:
  FitAmpSum(const DalitzEventPattern& pat
	    , const char* fname=0
	    , MINT::MinuitParameterSet* pset=0
	    , const std::string& prefix=""
	     , const std::string& opt=""
	    );

  FitAmpSum(const DalitzEventPattern& pat
	    , MINT::MinuitParameterSet* pset
	    , const std::string& prefix=""
	     , const std::string& opt=""
	    );

  FitAmpSum(const DalitzEventPattern& pat
	    , const std::string& prefix
	     , const std::string& opt=""
	    );
  
  FitAmpSum(const FitAmpSum& other);
  FitAmpSum(const FitAmpList& other);
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
  // same behaviour for FitParameters as for copy constructor.


  // if "virtual" works as I think it should
  // the below is not necessary - leave it in
  // for now, maybe I had a reason. But might
  // in future just use FitAmpList's routines.
  virtual DalitzBoxSet makeBoxes(const DalitzEventPattern& pat
                                 , double nSigma=2){
    return FitAmpList::makeBoxes(pat, this, nSigma);}

  virtual DalitzBWBoxSet makeBWBoxes(const DalitzEventPattern& pat
                                     , TRandom* rnd=gRandom){
    return FitAmpList::makeBWBoxes(pat, this, rnd);}


  virtual std::complex<double> getVal(IDalitzEvent& evt);
  virtual std::complex<double> getVal(IDalitzEvent* evt);// for backward compatibility
  virtual void Gradient(IDalitzEvent& evt,Double_t* grad, MINT::MinuitParameterSet* mps);
  virtual bool useAnalyticGradient() {return _useAnalyticGradient;}

  /*
  virtual std::complex<double> getSmootherLargerVal();
  virtual std::complex<double> getSmootherLargerVal(IDalitzEvent* evt);
  */

  virtual MINT::counted_ptr<IIntegrationCalculator> makeIntegrationCalculator();
  virtual MINT::counted_ptr<IntegCalculator> makeIntegCalculator();
  virtual MINT::counted_ptr<FitAmpPairList> makeFitAmpPairList();

  virtual double Prob(IDalitzEvent& evt){
    std::complex<double> res = getVal(evt);
    return res.real()*res.real() + res.imag()*res.imag();
  }

  /*
  virtual double SmootherLargerProb(){
    std::complex<double> res = getSmootherLargerVal();
    return res.real()*res.real() + res.imag()*res.imag();
  }
  */

  virtual double RealVal(IDalitzEvent& evt){
    return Prob(evt);
  }

  /*
  virtual double SmootherLargerRealVal(){
    return SmootherLargerProb();
  }
  */

  virtual std::complex<double> ComplexVal(IDalitzEvent& evt){return getVal(evt);}

  virtual MINT::counted_ptr<MINT::IUnweightedEventGenerator<IDalitzEvent> > 
    makeEventGenerator(const DalitzEventPattern& pat, TRandom* rnd=gRandom){
    MINT::counted_ptr<MINT::IUnweightedEventGenerator<IDalitzEvent> > 
      ptr(new DalitzBWBoxSet(makeBWBoxes(pat, rnd)));
    return ptr;
  }

  virtual void print(std::ostream& os=std::cout) const;
  virtual void printNonZero(std::ostream& os=std::cout) const;
  virtual void printValues(IDalitzEvent& evt, std::ostream& os=std::cout) ;

  virtual ~FitAmpSum();

  friend class FitAmplitude;

  FitAmpSum& operator*=(double r);
  FitAmpSum& operator*=(const std::complex<double>& z);
  FitAmpSum& operator*=(const MINT::counted_ptr<MINT::IReturnComplex>& irc);
  FitAmpSum& operator*=(const MINT::counted_ptr<MINT::IReturnComplexForEvent<IDalitzEvent> >& ircfe);

  FitAmpSum operator*(double r) const;
  FitAmpSum operator*(const std::complex<double>& z) const;
  FitAmpSum operator*(const MINT::counted_ptr<MINT::IReturnComplex>& irc) const;
  FitAmpSum operator*(const MINT::counted_ptr<MINT::IReturnComplexForEvent<IDalitzEvent> >& ircfe) const;


  FitAmpSum& operator=(const FitAmpSum& other);
  FitAmpSum& operator=(const FitAmpList& other);
  FitAmpSum& operator+=(const FitAmpSum& other);
  FitAmpSum operator+(const FitAmpSum& other) const;

};

FitAmpSum operator*(double r, const FitAmpSum& rhs);
FitAmpSum operator*(const std::complex<double>& z, const FitAmpSum& rhs);
FitAmpSum operator*(const MINT::counted_ptr<MINT::IReturnComplex>& irc
		     , const FitAmpSum& rhs);
FitAmpSum operator*(const MINT::counted_ptr<MINT::IReturnComplexForEvent<IDalitzEvent> >& ircfe
		     , const FitAmpSum& rhs);


#endif
//
