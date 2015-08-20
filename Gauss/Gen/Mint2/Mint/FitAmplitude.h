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
#include "Mint/Amplitude.h"

#include "Mint/IDalitzEvent.h"

#include "Mint/ComplexProduct.h"
#include "Mint/ComplexProductForEvent.h"

#include "Mint/FitParDependent.h"
#include "Mint/IFitParRegister.h"

#include <complex>

class FitAmplitude
: virtual public MINT::IReturnRealForEvent<IDalitzEvent>
, virtual public MINT::IReturnComplexForEvent<IDalitzEvent>
, public MINT::FitParDependent
{
 public:
  enum STRING_USAGE{FULL_NAME, PREFIX, DEFAULT};
 protected:
  // members
  Amplitude _amp;
  MINT::counted_ptr<MINT::FitComplex> _FitAmpPhase; // what we want to fit.
  MINT::NamedParameter<double> _fitFraction; // not varied in fit; input or result.
  mutable MINT::ComplexProduct _preFactors; // dirty, fix one day.
  MINT::ComplexProductForEvent<IDalitzEvent> _evt_dep_preFactors;

  std::string _name;
  static std::string longestNameInList();

  // methods
  FitAmplitude(const FitAmplitude& other, IFitParRegister* newDaddy=0);

 public:
  static void AutogenerateFitFile(const std::string& fname =
				  "protoFitAmplitudeFile.txt"
				  , const DalitzEventPattern& pat = 
				  DalitzEventPattern::NoPattern
				  );
  

  FitAmplitude(const std::string& yourOwnName
	       , const AmpInitialiser& treeWithOpts
	       , const char* fname=0
	       , MINT::MinuitParameterSet* pset=0
	       , STRING_USAGE useStringAs = FULL_NAME
	       );

  FitAmplitude(const std::string& yourOwnName
	       , const AmpInitialiser& treeWithOpts
	       , MINT::MinuitParameterSet* pset
	       , STRING_USAGE useStringAs = FULL_NAME
	       );
  // you decide name and pass decay as DecayTree

  FitAmplitude(const AmpInitialiser& treeWithOpts
	       , const char* fname=0
	       , MINT::MinuitParameterSet* pset=0
	       );

  FitAmplitude(const AmpInitialiser& treeWithOpts
	       , MINT::MinuitParameterSet* pset
	       );

  // derives name from tree
  FitAmplitude(const std::string& StandardisedDecayTreeName
	       , const char* fname=0
	       , MINT::MinuitParameterSet* pset=0
	       );
  FitAmplitude(const std::string& StandardisedDecayTreeName
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
  std::complex<double> preFactors() const{
    return _preFactors.ComplexVal();
  }
  const MINT::ComplexProductForEvent<IDalitzEvent>& eventDependentPrefactors() const{
    return _evt_dep_preFactors;
  }
  MINT::ComplexProductForEvent<IDalitzEvent>& eventDependentPrefactors(){
    return _evt_dep_preFactors;
  }

  MINT::FitComplex& FitAmpPhase(){
    return *_FitAmpPhase;}
  const MINT::FitComplex& FitAmpPhase() const{
    return *_FitAmpPhase;}

  //  std::complex<double> AmpPhase();
  std::complex<double> AmpPhase() const{
    if(isZero()) return 0;
    return FitAmpPhase() * preFactors();
  }

  // a few things directly related to Amplitude
  // (some of these should probably go away
  // one day, but keep them for now, for compatibility
  // with previous scheme (where FitAmplitude inherited
  // from Amplitude)
  Amplitude& amp(){return _amp;}
  const Amplitude& amp() const{return _amp;}

  DecayTree theBareDecay() const{
    return amp().theBareDecay();
  }
  DalitzEventPattern getTreePattern() const{
    return amp().getTreePattern();
  }
  DalitzBoxSet MakeBoxes(const DalitzEventPattern& pat
				 , double nSigma=3){
    return amp().MakeBoxes(pat, nSigma);
  }
  DalitzBWBoxSet MakeBWBoxes(const DalitzEventPattern &pat
				     , TRandom* rnd=gRandom
				     ){
    return amp().MakeBWBoxes(pat, rnd);
  }




  virtual double boxFactor(){
/*     std::cout << "boxFacror in FitAmplitude " << this->theDecay().oneLiner() << " = " */
/* 	 << "norm(" << AmpPhase() << ") * " << Amplitude::boxFactor() */
/* 	 << " = " << norm(AmpPhase()) * Amplitude::boxFactor() << std::endl; */
    return norm(AmpPhase()) * amp().boxFactor();
  }

  virtual std::complex<double> getVal(IDalitzEvent& evt);

  virtual std::complex<double> getVal(IDalitzEvent* evt); //for backwards compatibility

  virtual std::complex<double> getValWithoutFitParameters(IDalitzEvent& evt){
    return amp().getVal(evt);
  }

  double Prob(IDalitzEvent& evt){
    std::complex<double> z = getVal(evt);
    return z.real()*z.real() + z.imag()*z.imag();
  }

  virtual std::complex<double> ComplexVal(IDalitzEvent& evt){ return getVal(evt); }
  virtual double RealVal(IDalitzEvent& evt){ return Prob(evt); }


  std::string name() const{
    return _name;
  }


  void multiply(double r); // by value
  void multiply(const std::complex<double>& z); // by value
  void multiply(const MINT::counted_ptr<MINT::IComplexFitParDependent>& irc); // by ref
  void multiply(const MINT::counted_ptr<MINT::IReturnComplex>& irc); // by ref
  void multiply(const MINT::counted_ptr<MINT::IReturnComplexForEvent<IDalitzEvent> >& irce); // by ref
  void multiply(const MINT::counted_ptr<MINT::IComplexForEventFitParDependent<IDalitzEvent> >& irce); // by ref

  FitAmplitude& operator*=(double r);
  FitAmplitude& operator*=(const std::complex<double>& z);
  FitAmplitude& operator*=(const MINT::counted_ptr<MINT::IComplexFitParDependent>& irc);
  FitAmplitude& operator*=(const MINT::counted_ptr<MINT::IReturnComplex>& irc);
  FitAmplitude& operator*=(const MINT::counted_ptr<MINT::IReturnComplexForEvent<IDalitzEvent> >& irce);
  FitAmplitude& operator*=(const MINT::counted_ptr<MINT::IComplexForEventFitParDependent<IDalitzEvent> >& irce);

  void print(std::ostream& os = std::cout) const;


  ~FitAmplitude();

  friend class FitAmpList;
  friend class FitAmpListBase;
};

std::ostream& operator<<(std::ostream& os, const FitAmplitude& fa);
#endif
//
