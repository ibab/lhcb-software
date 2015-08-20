#ifndef FITAMPLISTBASE_HH
#define FITAMPLISTBASE_HH
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

class FitAmpListBase
{
 protected:
  std::vector<FitAmplitude*> _fitAmps;
  std::vector<MINT::counted_ptr<FitAmpListBase> > _fitAmpLists;
  // this construct allows us to have amplitudes that are groups of other
  // amplitudes, that are not broken up - which can be quite useful, for
  // example for a model-independent amplitude.

  MINT::counted_ptr<MINT::IReturnRealForEvent<IDalitzEvent> > _efficiency;

  virtual void deleteAll();
 public:
  FitAmpListBase();
  FitAmpListBase(const FitAmpListBase& other);
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
  virtual int addCopyWithSameFitParameters(const FitAmpListBase& other
					   , double factor=1);
  virtual bool addAmplitude(FitAmplitude* fa);
  virtual int add(const FitAmpListBase& other, double factor=1);
  virtual int addAsList(const FitAmpListBase& other, double factor=1);

  virtual unsigned int size() const;

  virtual FitAmplitude* getAmpPtr(unsigned int i);
  const FitAmplitude* getAmpPtr(unsigned int i) const;
 
  virtual bool CPConjugateSameFitParameters();
  virtual MINT::counted_ptr<FitAmpListBase> GetCPConjugateSameFitParameters() const;

  // if you inherit from this class, you must re-implement
  // the clone routine below, otherwise error.
  virtual MINT::counted_ptr<FitAmpListBase> GetCloneSameFitParameters() const;
  // ... see example in FitAmpSum. The Trick is the different new...

  // dirty triple hack:
  virtual MINT::counted_ptr<FitAmpPairList> makeFitAmpPairList(){
    return 0;
  }
  virtual MINT::counted_ptr<IIntegrationCalculator> makeIntegrationCalculator(){
    return 0;
  }
  virtual MINT::counted_ptr<IntegCalculator> makeIntegCalculator(){
    return 0;
  }
  
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

  virtual ~FitAmpListBase();

  virtual void printLargestAmp(IDalitzEvent& evt, std::ostream& os = std::cout);
  virtual void printAllAmps(std::ostream& os = std::cout)const;
  virtual void printAllAmps(IDalitzEvent& evt, std::ostream& os = std::cout);
  virtual void printNonZeroWithValue(IDalitzEvent& evt, std::ostream& os = std::cout);
  virtual void printNonZero(std::ostream& os = std::cout) const;
  virtual void print(std::ostream& os=std::cout) const;


  void setEfficiency(const MINT::counted_ptr<MINT::IReturnRealForEvent<IDalitzEvent> >& eff);
  double efficiency(IDalitzEvent& evt);

  virtual void multiply(double r); // by value
  virtual void multiply(const std::complex<double>& z); // by value
  virtual void multiply(const MINT::counted_ptr<MINT::IReturnComplex> irc); // by ref
  virtual void multiply(MINT::counted_ptr<MINT::IReturnComplexForEvent<IDalitzEvent> > irc); // by ref
  // (last one not const because getVal(evt) is not const)

  FitAmpListBase& operator*=(double r);
  FitAmpListBase& operator*=(const std::complex<double>& z);
  FitAmpListBase& operator*=(const MINT::counted_ptr<MINT::IReturnComplex>& irc);

  FitAmpListBase operator*(double r) const;
  FitAmpListBase operator*(const std::complex<double>& z) const;
  FitAmpListBase operator*(const MINT::counted_ptr<MINT::IReturnComplex>& irc) const;

  void normalizeAmps(DalitzEventList& evtList);    

  friend class FitAmplitude;


  FitAmpListBase& operator=(const FitAmpListBase& other);
  FitAmpListBase& operator+=(const FitAmpListBase& other);
  FitAmpListBase operator+(const FitAmpListBase& other) const;
};

FitAmpListBase operator*(double r, const FitAmpListBase& rhs);
FitAmpListBase operator*(const std::complex<double>& z, const FitAmpListBase& rhs);
FitAmpListBase operator*(const MINT::counted_ptr<MINT::IReturnComplex>& irc
		     , const FitAmpListBase& rhs);

std::ostream& operator<<(std::ostream& os, const FitAmpListBase& fal);

#endif
//
