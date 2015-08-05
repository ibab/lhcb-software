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

class FitAmplitude;
namespace MINT{
  class MinuitParameterSet;
}

class FitAmpList
{
 protected:
  DalitzEventPattern _pat;
  std::string _paraFName; // default: read from stdin;
  std::vector<FitAmplitude*> _fitAmps;
  MINT::MinuitParameterSet* _minuitParaSet;
  MINT::counted_ptr<MINT::IReturnRealForEvent<IDalitzEvent> > _efficiency;
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

  virtual MINT::MinuitParameterSet* getMPS();

  virtual unsigned int size() const;

  virtual FitAmplitude* getAmpPtr(int i);
  const FitAmplitude* getAmpPtr(int i) const;
 
  virtual bool CPConjugateSameFitParameters();
  virtual MINT::counted_ptr<FitAmpList> GetCPConjugateSameFitParameters() const;

  // if you inherit from this class, you must re-implement
  // the clone routine below, otherwise error.
  virtual MINT::counted_ptr<FitAmpList> GetCloneSameFitParameters() const;
  // ... see example in FitAmpSum. The Trick is the different new...

  virtual bool createAllAmps(const DalitzEventPattern& thePattern
			     , const std::string& prefix="");

  virtual bool createAmpsFromFile(){return false;}// to be implemented later
  
  //  DalitzBoxSet makeBoxes(MINT::IReturnRealForEvent<IDalitzEvent>* pdf
  //			 , double nSigma = 2
  //			 );
  DalitzBoxSet makeBoxes(const DalitzEventPattern& pat
			 , MINT::IReturnRealForEvent<IDalitzEvent>* pdf
			 , double nSigma=2);
  
  //  DalitzBWBoxSet makeBWBoxes( MINT::IReturnRealForEvent<IDalitzEvent>* pdf
  //			      , TRandom* rnd=gRandom
  //			      );
  DalitzBWBoxSet makeBWBoxes(const DalitzEventPattern& pat
			     , MINT::IReturnRealForEvent<IDalitzEvent>* pdf
			     , TRandom* rnd=gRandom);


  void setAllAmpsTo(const std::complex<double> z);

  virtual ~FitAmpList();

  void printLargestAmp(IDalitzEvent& evt, std::ostream& os = std::cout);
  void printAllAmps(std::ostream& os = std::cout)const;
  void printAllAmps(IDalitzEvent& evt, std::ostream& os = std::cout);
  void printNonZeroWithValue(IDalitzEvent& evt, std::ostream& os = std::cout);
  void printNonZero(std::ostream& os = std::cout) const;
  virtual void print(std::ostream& os=std::cout) const;


  void setEfficiency(const MINT::counted_ptr<MINT::IReturnRealForEvent<IDalitzEvent> >& eff);
  double efficiency(IDalitzEvent& evt);

  virtual void multiply(double r); // by value
  virtual void multiply(const std::complex<double>& z); // by value
  virtual void multiply(const MINT::counted_ptr<MINT::IReturnComplex> irc); // by ref
  virtual void multiply(MINT::counted_ptr<MINT::IReturnComplexForEvent<IDalitzEvent> > irc); // by ref
  // (last one not const because getVal(evt) is not const)

  FitAmpList& operator*=(double r);
  FitAmpList& operator*=(const std::complex<double>& z);
  FitAmpList& operator*=(const MINT::counted_ptr<MINT::IReturnComplex>& irc);

  FitAmpList operator*(double r) const;
  FitAmpList operator*(const std::complex<double>& z) const;
  FitAmpList operator*(const MINT::counted_ptr<MINT::IReturnComplex>& irc) const;

  void normalizeAmps(DalitzEventList& evtList);    

  friend class FitAmplitude;


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
