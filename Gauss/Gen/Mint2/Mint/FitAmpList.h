#ifndef FITAMPLIST_HH
#define FITAMPLIST_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:03 GMT

#include <complex>
#include <iostream>
#include <ostream>

#include "Mint/counted_ptr.h"

#include "Mint/DalitzBoxSet.h"
#include "Mint/DalitzBWBoxSet.h"

#include "Mint/FitAmpPairList.h"


#include "Mint/IReturnRealForEvent.h"
#include "Mint/IReturnComplexForEvent.h"
#include "Mint/IIntegrationCalculator.h"
#include "Mint/IntegCalculator.h"
#include "Mint/FitAmpListBase.h"

class FitAmplitude;
namespace MINT{
  class MinuitParameterSet;
}

class FitAmpList : public FitAmpListBase
{
 protected:
  DalitzEventPattern _pat;
  std::string _paraFName; // default: read from stdin;

  MINT::MinuitParameterSet* _minuitParaSet;

  std::string _opt; // option (not yet used)

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

  virtual MINT::MinuitParameterSet* getMPS();

  virtual MINT::counted_ptr<FitAmpListBase> GetCPConjugateSameFitParameters() const;

  // if you inherit from this class, you must re-implement
  // the clone routine below, otherwise error.
  virtual MINT::counted_ptr<FitAmpListBase> GetCloneSameFitParameters() const;
  // ... see example in FitAmpSum. The Trick is the different new...

  virtual bool createAllAmps(const DalitzEventPattern& thePattern
			     , const std::string& prefix="");

  virtual bool createAmpsFromFile(){return false;}// to be implemented later
  
  
  virtual ~FitAmpList();

  virtual void printLargestAmp(IDalitzEvent& evt, std::ostream& os = std::cout);
  virtual void printAllAmps(std::ostream& os = std::cout)const{
    FitAmpListBase::printAllAmps(os);}

  virtual void printAllAmps(IDalitzEvent& evt, std::ostream& os = std::cout);
  virtual void printNonZeroWithValue(IDalitzEvent& evt, std::ostream& os = std::cout);
  virtual void printNonZero(std::ostream& os = std::cout) const{
    FitAmpListBase::printNonZero(os);}


  friend class FitAmplitude;

  FitAmpList& operator*=(double r);
  FitAmpList& operator*=(const std::complex<double>& z);
  FitAmpList& operator*=(const MINT::counted_ptr<MINT::IReturnComplex>& irc);

  FitAmpList operator*(double r) const;
  FitAmpList operator*(const std::complex<double>& z) const;
  FitAmpList operator*(const MINT::counted_ptr<MINT::IReturnComplex>& irc) const;

  FitAmpList& operator=(const FitAmpList& other);
  FitAmpList& operator+=(const FitAmpList& other);
  FitAmpList operator+(const FitAmpList& other) const;
};

FitAmpList operator*(double r, const FitAmpList& rhs);
FitAmpList operator*(const std::complex<double>& z, const FitAmpList& rhs);
FitAmpList operator*(const MINT::counted_ptr<MINT::IReturnComplex>& irc
		     , const FitAmpList& rhs);

std::ostream& operator<<(std::ostream& os, const FitAmpList& fal);

#endif
//
