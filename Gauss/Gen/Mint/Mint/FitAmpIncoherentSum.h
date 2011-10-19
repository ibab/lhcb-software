#ifndef FITAMPINCOHERENTSUM_HH
#define FITAMPINCOHERENTSUM_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:03 GMT

#include <iostream>

#include "Mint/counted_ptr.h"

#include "Mint/IDalitzEventAccess.h"
#include "Mint/DalitzEventAccess.h"
#include "Mint/DalitzBoxSet.h"
#include "Mint/DalitzBWBoxSet.h"
#include "Mint/IntegCalculator.h"

#include "Mint/IGetRealEvent.h"
#include "Mint/IGetComplexEvent.h"
#include "Mint/IFastAmplitudeIntegrable.h"
#include "Mint/IIntegrationCalculator.h"

#include "Mint/ILookLikeFitAmpSum.h"

#include "Mint/FitAmpList.h"

class FitAmpIncoherentSum 
: virtual public MINT::IGetRealEvent<IDalitzEvent>
, virtual public IFastAmplitudeIntegrable
, virtual public ILookLikeFitAmpSum
, public FitAmpList
{
 protected:
  static std::string IncPrefix();
 public:
  FitAmpIncoherentSum(const DalitzEventPattern& pat
	    , const char* fname=0
	    , MINT::MinuitParameterSet* pset=0
	    , const std::string& prefix=""
	    , const std::string& opt=""
	    );

  FitAmpIncoherentSum(const DalitzEventPattern& pat
	    , MINT::MinuitParameterSet* pset
	    , const std::string& prefix=""
	    , const std::string& opt=""
	    );
  FitAmpIncoherentSum(const DalitzEventPattern& pat
	    , const std::string& prefix
	    , const std::string& opt=""
	    );

  FitAmpIncoherentSum(IDalitzEventAccess* events
	    , const char* fname=0
	    , MINT::MinuitParameterSet* pset=0
	    , const std::string& prefix=""
	    , const std::string& opt=""
	    );
  
  FitAmpIncoherentSum(IDalitzEventAccess* events
	    , MINT::MinuitParameterSet* pset
	    , const std::string& prefix=""
	    , const std::string& opt=""
	    );
  FitAmpIncoherentSum(IDalitzEventAccess* events
	    , const std::string& prefix
	    , const std::string& opt=""
	    );
  
  FitAmpIncoherentSum(IDalitzEventList* events
	    , const char* fname=0
	    , MINT::MinuitParameterSet* pset=0
	    , const std::string& prefix=""
	    , const std::string& opt=""
	    );
  
  FitAmpIncoherentSum(IDalitzEventList* events
	    , MINT::MinuitParameterSet* pset
	    , const std::string& prefix=""
	    , const std::string& opt=""
	    );
  FitAmpIncoherentSum(IDalitzEventList* events
	    , const std::string& prefix
	    , const std::string& opt=""
	    );
  
  FitAmpIncoherentSum(const FitAmpIncoherentSum& other);
  FitAmpIncoherentSum(const FitAmpList& other);
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


  double getVal();
  double getVal(IDalitzEvent* evt);

  /*
  double getSmootherLargerVal();
  double getSmootherLargerVal(IDalitzEvent* evt);
  */

  virtual MINT::counted_ptr<IIntegrationCalculator> makeIntegrationCalculator();
  virtual MINT::counted_ptr<IntegCalculator> makeIntegCalculator();

  virtual double Prob(){
    return getVal();
  }

  /*
  virtual double SmootherLargerProb(){
    return getSmootherLargerVal();
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

  virtual MINT::counted_ptr<MINT::IUnweightedEventGenerator<IDalitzEvent> > 
    makeEventGenerator(TRandom* rnd=gRandom){
    MINT::counted_ptr<MINT::IUnweightedEventGenerator<IDalitzEvent> > 
      ptr(new DalitzBWBoxSet(makeBWBoxes(rnd)));
    return ptr;
  }

  virtual ~FitAmpIncoherentSum();

  void printLargestAmp(std::ostream& os = std::cout);
  virtual void print(std::ostream& os=std::cout) const;
  virtual void printNonZero(std::ostream& os=std::cout) const;

  friend class FitAmplitude;

  FitAmpIncoherentSum& operator*=(double r);
  FitAmpIncoherentSum& operator*=(const std::complex<double>& z);
  FitAmpIncoherentSum& operator*=(const MINT::counted_ptr<MINT::IReturnComplex>& irc);

  FitAmpIncoherentSum operator*(double r) const;
  FitAmpIncoherentSum operator*(const std::complex<double>& z) const;
  FitAmpIncoherentSum operator*(const MINT::counted_ptr<MINT::IReturnComplex>& irc) const;


  FitAmpIncoherentSum& operator=(const FitAmpIncoherentSum& other);
  FitAmpIncoherentSum& operator=(const FitAmpList& other);
  FitAmpIncoherentSum& operator+=(const FitAmpIncoherentSum& other);
  FitAmpIncoherentSum operator+(const FitAmpIncoherentSum& other) const;

};

FitAmpIncoherentSum operator*(double r, const FitAmpIncoherentSum& rhs);
FitAmpIncoherentSum operator*(const std::complex<double>& z, const FitAmpIncoherentSum& rhs);
FitAmpIncoherentSum operator*(const MINT::counted_ptr<MINT::IReturnComplex>& irc
		     , const FitAmpIncoherentSum& rhs);


#endif
//
