#ifndef FITAMPLIST_HH
#define FITAMPLIST_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:03 GMT

#include <complex>
#include <iostream>

#include "Mint/counted_ptr.h"

#include "Mint/IGetDalitzEvent.h"

#include "Mint/IDalitzEventAccess.h"
#include "Mint/DalitzEventAccess.h"
#include "Mint/DalitzBoxSet.h"
#include "Mint/DalitzBWBoxSet.h"

#include "Mint/FitAmpPairList.h"


#include "Mint/IGetRealEvent.h"
#include "Mint/IGetRealEvent.h"
#include "Mint/IGetComplexEvent.h"
#include "Mint/IIntegrationCalculator.h"

class FitAmplitude;
namespace MINT{
  class MinuitParameterSet;
}

class FitAmpList
: public DalitzEventAccess
{
 protected:
  std::string _paraFName; // default: read from stdin;
  std::vector<FitAmplitude*> _fitAmps;
  MINT::MinuitParameterSet* _minuitParaSet;
  MINT::counted_ptr<IGetDalitzEvent> _efficiency;
  std::string _opt; // option (not yet used)

  virtual void deleteAll();
 public:
  FitAmpList(const DalitzEventPattern& pat
	     , const char* fname=0
	     , MINT::MinuitParameterSet* pset=0
	     , const std::string& prefix=""
	     , const std::string& opt=""
	    );

  FitAmpList(const DalitzEventPattern& pat
	    , MINT::MinuitParameterSet* pset
	    , const std::string& prefix=""
	     , const std::string& opt=""
	    );
  FitAmpList(const DalitzEventPattern& pat
	    , const std::string& prefix
	     , const std::string& opt=""
	    );

  FitAmpList(IDalitzEventAccess* events
	    , const char* fname=0
	    , MINT::MinuitParameterSet* pset=0
	    , const std::string& prefix=""
	     , const std::string& opt=""
	    );
  
  FitAmpList(IDalitzEventAccess* events
	    , MINT::MinuitParameterSet* pset
	    , const std::string& prefix=""
	     , const std::string& opt=""
	    );
  FitAmpList(IDalitzEventAccess* events
	    , const std::string& prefix
	     , const std::string& opt=""
	    );
  
  FitAmpList(IDalitzEventList* events
	    , const char* fname=0
	    , MINT::MinuitParameterSet* pset=0
	    , const std::string& prefix=""
	     , const std::string& opt=""
	    );
  
  FitAmpList(IDalitzEventList* events
	    , MINT::MinuitParameterSet* pset
	    , const std::string& prefix=""
	     , const std::string& opt=""
	    );
  FitAmpList(IDalitzEventList* events
	    , const std::string& prefix
	     , const std::string& opt=""
	    );
  
  FitAmpList(const FitAmpList& other);
  /* 
     The copy constructor copies like this: There'll be 'physical'
     copies of all Amplitudes, but the FitParameters remain the
     same (pointers to the same FitParameter Object).  This is
     useful for the CP-conj coding as it is now, but perhaps a bit
     counter-intuitive.  Needs to be reviewed at some point. This
     behaviour is defined in the copy constructor of the
     FitAmplitude class.
  */

  // following two routines are the same for now:
  virtual int addCopyWithSameFitParameters(const FitAmpList& other
					   , double factor=1);
  virtual int add(const FitAmpList& other, double factor=1);

  virtual unsigned int size() const;
  virtual FitAmplitude* getAmpPtr(int i);
  const FitAmplitude* getAmpPtr(int i) const;

  virtual MINT::MinuitParameterSet* getMinuitParameterSet(){return _minuitParaSet;};
 
  virtual bool CPConjugateSameFitParameters();
  virtual MINT::counted_ptr<FitAmpList> GetCPConjugateSameFitParameters() const;

  // if you inherit from this class, you must re-implement
  // the clone routine below, otherwise error.
  virtual MINT::counted_ptr<FitAmpList> GetCloneSameFitParameters() const;
  // ... see example in FitAmpSum. The Trick is the different new...

  virtual bool createAllAmps(const std::string& prefix="");
  virtual bool createAllAmps(const DalitzEventPattern& thePattern
			     , const std::string& prefix="");

  virtual bool createAmpsFromFile(){return false;}// to be implemented later
  
  DalitzBoxSet makeBoxes(MINT::IGetRealEvent<IDalitzEvent>* pdf
			 , double nSigma = 2
			 );
  DalitzBoxSet makeBoxes(const DalitzEventPattern& pat
			 , MINT::IGetRealEvent<IDalitzEvent>* pdf
			 , double nSigma=2);
  
  DalitzBWBoxSet makeBWBoxes( MINT::IGetRealEvent<IDalitzEvent>* pdf
			      , TRandom* rnd=gRandom
			      );
  DalitzBWBoxSet makeBWBoxes(const DalitzEventPattern& pat
			     , MINT::IGetRealEvent<IDalitzEvent>* pdf
			     , TRandom* rnd=gRandom);


  void setAllAmpsTo(const std::complex<double> z);

  virtual ~FitAmpList();

  void printLargestAmp(std::ostream& os = std::cout);
  void printAllAmps(std::ostream& os = std::cout);
  void printNonZero(std::ostream& os = std::cout) const;
  virtual void print(std::ostream& os=std::cout) const;


  void setEfficiency(MINT::counted_ptr<IGetDalitzEvent> eff);
  double efficiency();

  virtual void multiply(double r); // by value
  virtual void multiply(const std::complex<double>& z); // by value
  virtual void multiply(const MINT::counted_ptr<MINT::IReturnComplex> irc); // by ref
  
  FitAmpList& operator*=(double r);
  FitAmpList& operator*=(const std::complex<double>& z);
  FitAmpList& operator*=(const MINT::counted_ptr<MINT::IReturnComplex>& irc);

  FitAmpList operator*(double r) const;
  FitAmpList operator*(const std::complex<double>& z) const;
  FitAmpList operator*(const MINT::counted_ptr<MINT::IReturnComplex>& irc) const;


  friend class FitAmplitude;


  FitAmpList& operator=(const FitAmpList& other);
  FitAmpList& operator+=(const FitAmpList& other);
  FitAmpList operator+(const FitAmpList& other) const;
};

FitAmpList operator*(double r, const FitAmpList& rhs);
FitAmpList operator*(const std::complex<double>& z, const FitAmpList& rhs);
FitAmpList operator*(const MINT::counted_ptr<MINT::IReturnComplex>& irc
		     , const FitAmpList& rhs);


#endif
//
