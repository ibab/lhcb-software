#ifndef FITAMPLITUDE_HH
#define FITAMPLITUDE_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:03 GMT

#include <string>

#include "Mint/counted_ptr.h"

#include "Mint/DecayTree.h"
#include "Mint/DalitzEventPattern.h"

#include "Mint/NamedParameter.h"
#include "Mint/FitComplex.h"
#include "Mint/FitComplexPolar.h"
#include "Mint/AmpInitialiser.h"
#include "Mint/FastAmplitude.h"

#include "Mint/IBasicEventAccess.h"
#include "Mint/IDalitzEventAccess.h"

#include "Mint/ComplexProduct.h"
#include <complex>

class FitAmplitude : public FastAmplitude
, virtual public MINT::IGetRealEvent<IDalitzEvent>
  , virtual public MINT::IReturnComplex
{
 public:
  enum STRING_USAGE{FULL_NAME, PREFIX, DEFAULT};
 protected:
  // members
  MINT::counted_ptr<MINT::FitComplex> _FitAmpPhase; // what we want to fit.
  MINT::NamedParameter<double> _fitFraction; // not in fit; input or result.
  MINT::ComplexProduct _preFactors;

  std::string _name;
  static std::string longestNameInList();

  // methods
  FitAmplitude(const FitAmplitude& other);
  FitAmplitude(const FitAmplitude& other
	       , IDalitzEventAccess* newEvents
	       );
  FitAmplitude(const FitAmplitude& other
	       , IDalitzEventList* newEvents
	       );
 public:
  static void AutogenerateFitFile(const std::string& fname =
				  "protoFitAmplitudeFile.txt"
				  , const DalitzEventPattern& pat = 
				  DalitzEventPattern::NoPattern
				  );
  

  FitAmplitude(const std::string& yourOwnName
	       , const AmpInitialiser& treeWithOpts
	       , IDalitzEventAccess* events
	       , const char* fname=0
	       , MINT::MinuitParameterSet* pset=0
	       , STRING_USAGE useStringAs = FULL_NAME
	       );
  FitAmplitude(const std::string& yourOwnName
	       , const AmpInitialiser& treeWithOpts
	       , IDalitzEventList* events
	       , const char* fname=0
	       , MINT::MinuitParameterSet* pset=0
	       , STRING_USAGE useStringAs = FULL_NAME
	       );
  FitAmplitude(const std::string& yourOwnName
	       , const AmpInitialiser& treeWithOpts
	       , IDalitzEventAccess* events
	       , MINT::MinuitParameterSet* pset
	       , STRING_USAGE useStringAs = FULL_NAME
	       );
  FitAmplitude(const std::string& yourOwnName
	       , const AmpInitialiser& treeWithOpts
	       , IDalitzEventList* events
	       , MINT::MinuitParameterSet* pset
	       , STRING_USAGE useStringAs = FULL_NAME
	       );
  // you decide name and pass decay as DecayTree

  FitAmplitude(const AmpInitialiser& treeWithOpts
	       , IDalitzEventAccess* events
	       , const char* fname=0
	       , MINT::MinuitParameterSet* pset=0
	       );
  FitAmplitude(const AmpInitialiser& treeWithOpts
	       , IDalitzEventList* events
	       , const char* fname=0
	       , MINT::MinuitParameterSet* pset=0
	       );
  FitAmplitude(const AmpInitialiser& treeWithOpts
	       , IDalitzEventAccess* events
	       , MINT::MinuitParameterSet* pset
	       );
  FitAmplitude(const AmpInitialiser& treeWithOpts
	       , IDalitzEventList* events
	       , MINT::MinuitParameterSet* pset
	       );
  // derives name from tree

  FitAmplitude(const std::string& StandardisedDecayTreeName
	       , IDalitzEventAccess* events
	       , const char* fname=0
	       , MINT::MinuitParameterSet* pset=0
	       );
  FitAmplitude(const std::string& StandardisedDecayTreeName
	       , IDalitzEventList* events
	       , const char* fname=0
	       , MINT::MinuitParameterSet* pset=0
	       );
  FitAmplitude(const std::string& StandardisedDecayTreeName
	       , IDalitzEventAccess* events
	       , MINT::MinuitParameterSet* pset
	       );
  FitAmplitude(const std::string& StandardisedDecayTreeName
	       , IDalitzEventList* events
	       , MINT::MinuitParameterSet* pset
	       );
  // derives tree from name (if known by NamedDecayTreeList
  // there is no parsing of decay descriptors or any such thing)

  bool isZero() const{ return FitAmpPhase().isZero();}

  bool isConstant() const;
  bool canBeIgnored() const;

  bool CPConjugateSameFitParameters();
  FitAmplitude GetCPConjugateSameFitParameters() const;

  inline FitAmplitude GetCopySameFitParameters() const{
    // just the copy constructor, but with a name that
    // explains the behaviour.
    return FitAmplitude(*this);
  }

  void setFraction(double fr){
    _fitFraction = fr;
  }
  double getFraction()const{
    return (double) _fitFraction;
  }

  //  std::complex<double> preFactors();
  std::complex<double> preFactors(){
    return _preFactors.ComplexVal();
  }
  MINT::FitComplex& FitAmpPhase(){
    return *_FitAmpPhase;}
  const MINT::FitComplex& FitAmpPhase() const{
    return *_FitAmpPhase;}

  //  std::complex<double> AmpPhase();
  std::complex<double> AmpPhase(){
    if(isZero()) return 0;
    return FitAmpPhase() * preFactors();
  }

  virtual double boxFactor(){
/*     std::cout << "boxFacror in FitAmplitude " << this->theDecay().oneLiner() << " = " */
/* 	 << "norm(" << AmpPhase() << ") * " << Amplitude::boxFactor() */
/* 	 << " = " << norm(AmpPhase()) * Amplitude::boxFactor() << std::endl; */
    return norm(AmpPhase()) * Amplitude::boxFactor();
  }

  std::complex<double> getVal();
  /*
  inline std::complex<double> getVal(){
    std::complex<double> ap(AmpPhase());
    if(0.0 == ap) return 0;
    return  ap * getValWithoutFitParameters();
  }
  */

  std::complex<double> getVal(IDalitzEvent* evt);

  //std::complex<double> getValWithoutFitParameters();
  std::complex<double> getValWithoutFitParameters(){
    return FastAmplitude::getVal();
  }

  virtual std::complex<double> getSmootherLargerVal();
  virtual std::complex<double> getSmootherLargerVal(IDalitzEvent* evt);

  virtual std::complex<double> getSmootherLargerValWithoutFitParameters();

  virtual std::complex<double> getValAtResonance();
  //  double gaussProb();

  double Prob(){
    std::complex<double> z = getVal();
    return z.real()*z.real() + z.imag()*z.imag();
  }
  std::string name() const{
    return _name;
  }


  void multiply(double r); // by value
  void multiply(const std::complex<double>& z); // by value
  void multiply(const MINT::counted_ptr<MINT::IReturnComplex>& irc); // by ref
  
  FitAmplitude& operator*=(double r);
  FitAmplitude& operator*=(const std::complex<double>& z);
  FitAmplitude& operator*=(const MINT::counted_ptr<MINT::IReturnComplex>& irc);

  void print(std::ostream& os = std::cout) const;



  /*
  std::complex<double> getValWithoutFitParameters(){
    return FastAmplitude::getVal();
  }
  
  std::complex<double> getVal(IDalitzEvent* evt){
    //bool dbThis=false;
    this->setEvent(evt);
    std::complex<double> result = this->getVal();
    this->resetEventRecord();
    return result;
  }
  
  std::complex<double> getVal(){
    bool dbThis=false;    
    //  if(FitAmpPhase().getAmp() == 0.0) return 0;
    if(AmpPhase() == 0.0) return 0;
    std::complex<double> valA = getValWithoutFitParameters();
    
    if(dbThis){
      cout << " FitAmplitude::getVal() for decay " 
	   << theDecay().oneLiner()
	   << "  total = " << AmpPhase() * valA
	   << "\n    >  M()   = " << FitAmpPhase().getVal()
	   << "\n    >  rB*exp()   = " << preFactors()
	   << "\n    >  Amp   = " << valA
	   << endl;
    }
  return  AmpPhase() * valA;
  }
  */

  ~FitAmplitude();

  friend class FitAmpList;
};

std::ostream& operator<<(std::ostream& os, const FitAmplitude& fa);
#endif
//
