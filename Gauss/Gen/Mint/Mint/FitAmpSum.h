#ifndef FITAMPSUM_HH
#define FITAMPSUM_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:03 GMT

#include <complex>
#include <iostream>

#include "Mint/IDalitzEventAccess.h"
#include "Mint/DalitzEventAccess.h"
#include "Mint/DalitzBoxSet.h"
#include "Mint/DalitzBWBoxSet.h"
#include "Mint/IntegCalculator.h"

#include "Mint/IGetRealEvent.h"
#include "Mint/IGetComplexEvent.h"
#include "Mint/IFastAmplitudeIntegrable.h"
#include "Mint/IUnweightedEventGenerator.h"
#include "Mint/IIntegrationCalculator.h"

#include "Mint/ILookLikeFitAmpSum.h"

#include "Mint/FitAmpList.h"

// note - this inherits now from FitAmpList, many
// methods formerly defined here are now defined there.

class FitAmpSum 
: virtual public MINT::IGetRealEvent<IDalitzEvent>
, virtual public MINT::IGetComplexEvent<IDalitzEvent>
, virtual public IFastAmplitudeIntegrable
, virtual public ILookLikeFitAmpSum
, public FitAmpList
{
 protected:
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

  FitAmpSum(IDalitzEventAccess* events
	    , const char* fname=0
	    , MINT::MinuitParameterSet* pset=0
	    , const std::string& prefix=""
	     , const std::string& opt=""
	    );
  
  FitAmpSum(IDalitzEventAccess* events
	    , MINT::MinuitParameterSet* pset
	    , const std::string& prefix=""
	     , const std::string& opt=""
	    );
  FitAmpSum(IDalitzEventAccess* events
	    , const std::string& prefix
	     , const std::string& opt=""
	    );
  
  FitAmpSum(IDalitzEventList* events
	    , const char* fname=0
	    , MINT::MinuitParameterSet* pset=0
	    , const std::string& prefix=""
	     , const std::string& opt=""
	    );
  
  FitAmpSum(IDalitzEventList* events
	    , MINT::MinuitParameterSet* pset
	    , const std::string& prefix=""
	     , const std::string& opt=""
	    );
  FitAmpSum(IDalitzEventList* events
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
  virtual MINT::counted_ptr<FitAmpList> GetCloneSameFitParameters() const;
  // same behaviour for FitParameters as for copy constructor.

  virtual DalitzBoxSet makeBoxes(double nSigma = 2){
    return FitAmpList::makeBoxes(this, nSigma);}
  virtual DalitzBoxSet makeBoxes(const DalitzEventPattern& pat
				 , double nSigma=2){
    return FitAmpList::makeBoxes(pat, this, nSigma);}

  virtual DalitzBWBoxSet makeBWBoxes(TRandom* rnd=gRandom){
    return FitAmpList::makeBWBoxes(this, rnd);}
  virtual DalitzBWBoxSet makeBWBoxes(const DalitzEventPattern& pat
				     , TRandom* rnd=gRandom){
    return FitAmpList::makeBWBoxes(pat, this, rnd);}


  virtual std::complex<double> getVal();
  virtual std::complex<double> getVal(IDalitzEvent* evt);

  /*
  virtual std::complex<double> getSmootherLargerVal();
  virtual std::complex<double> getSmootherLargerVal(IDalitzEvent* evt);
  */

  virtual MINT::counted_ptr<IIntegrationCalculator> makeIntegrationCalculator();
  virtual MINT::counted_ptr<IntegCalculator> makeIntegCalculator();

  virtual double Prob(){
    std::complex<double> res = getVal();
    return res.real()*res.real() + res.imag()*res.imag();
  }

  /*
  virtual double SmootherLargerProb(){
    std::complex<double> res = getSmootherLargerVal();
    return res.real()*res.real() + res.imag()*res.imag();
  }
  */

  virtual double RealVal(){
    return Prob();
  }

  /*
  virtual double SmootherLargerRealVal(){
    return SmootherLargerProb();
  }
  */

  virtual std::complex<double> ComplexVal(){return getVal();}

  virtual MINT::counted_ptr<MINT::IUnweightedEventGenerator<IDalitzEvent> > 
    makeEventGenerator(TRandom* rnd=gRandom){
    MINT::counted_ptr<MINT::IUnweightedEventGenerator<IDalitzEvent> > 
      ptr(new DalitzBWBoxSet(makeBWBoxes(rnd)));
    return ptr;
  }

  virtual void print(std::ostream& os=std::cout) const;
  virtual void printNonZero(std::ostream& os=std::cout) const;
  virtual void printValues(std::ostream& os=std::cout) ;

  virtual ~FitAmpSum();

  friend class FitAmplitude;

  FitAmpSum& operator*=(double r);
  FitAmpSum& operator*=(const std::complex<double>& z);
  FitAmpSum& operator*=(const MINT::counted_ptr<MINT::IReturnComplex>& irc);

  FitAmpSum operator*(double r) const;
  FitAmpSum operator*(const std::complex<double>& z) const;
  FitAmpSum operator*(const MINT::counted_ptr<MINT::IReturnComplex>& irc) const;


  FitAmpSum& operator=(const FitAmpSum& other);
  FitAmpSum& operator=(const FitAmpList& other);
  FitAmpSum& operator+=(const FitAmpSum& other);
  FitAmpSum operator+(const FitAmpSum& other) const;

};

FitAmpSum operator*(double r, const FitAmpSum& rhs);
FitAmpSum operator*(const std::complex<double>& z, const FitAmpSum& rhs);
FitAmpSum operator*(const MINT::counted_ptr<MINT::IReturnComplex>& irc
		     , const FitAmpSum& rhs);


#endif
//
