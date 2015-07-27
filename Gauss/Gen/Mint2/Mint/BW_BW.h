#ifndef BREITWIGNER_BLATTWEISSKOPF_HH
#define BREITWIGNER_BLATTWEISSKOPF_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:04 GMT

#include <complex>
#include <string>
#include <iostream>
#include <vector>
#include <stack>

#include "Mint/ILineshape.h"
#include "Mint/AssociatedDecayTree.h"
#include "Mint/DalitzCoordinate.h"
#include "Mint/MultiQuarkContent.h"
#include "Mint/GaussFct.h"
#include "Mint/IGenFct.h"
#include "Mint/ResonancePropertiesList.h"
#include "Mint/FitParDependent.h"
#include "ResonancePropertiesFitRef.h"
#include "Mint/NamedParameter.h"

#include "TRandom.h"

class ParticleProperties;

// Breit-Wigner with Blatt-Weisskopf penetration factors.
// Can only do 2 body decays for now... (it's the BW penetration factors)
class BW_BW : virtual public ILineshape, public MINT::FitParDependent{
 private:
  mutable IDalitzEvent* _eventPtr;
  //  mutable std::stack<IDalitzEvent*> _oldPointers;

  mutable double _prSq, _prSqForGofM, _pABSq, //_mumsPDGMass, _mumsPDGWidth, 
    _mumsRecoMass2, _mumsRecoMass, _Fr_BELLE, _Fr_PDG_BL, _GofM;
  mutable int _mumsPID;
  mutable bool _mumsPID_set;

  // mutable double _mumsPDGRadius;

  mutable int _mumsParity, _dgtrsInternalParity;

  mutable int _twoLPlusOne;

  mutable std::vector<double> _daughterRecoMass2
    , _daughterPDGMass, _daughterWidth;

  mutable bool _substitutePDGForReco;

  mutable MINT::counted_ptr<IGenFct> _genFct;

  double peakShift() const;
  double peakPosition() const;

  void makeGeneratingFunction() const;

 protected:

  MINT::NamedParameter<int> _normBF;
  const AssociatedDecayTree& _theDecay; 
  double _gen_s_mi, _gen_s_ma;

  bool substitutePDGForReco() const{
    return _substitutePDGForReco;
  }

  virtual double twoBody_dgtPsq_in_MumsFrame(double mumsMass
					     , double mA
					     , double mB)const; 
  virtual double twoBody_dgtPsq_in_MumsPDGFrame() const;
  virtual double twoBody_recodgtPsq_in_MumsPDGFrame() const;
  virtual double twoBody_dgtPsq_in_MumsRecoFrame();
  
  bool setEventPtr(IDalitzEvent& evt) const;
  IDalitzEvent* getEvent() const;

 public:
  virtual double prSq() const;
  virtual double prSqForGofM() const;
  virtual double pABSq();

  virtual double prSqMax() const;
 protected:
  
  bool setAllFitParameters();

  mutable ResonancePropertiesList* _RPL;
  ResonancePropertiesFitRef* _fittableResonancePropertiesPtr;
  MINT::FitParRef* _fittableGlobalRadiusPtr;

  ResonancePropertiesList* resonancePropertiesList() const;
  const ResonanceProperties* resonanceProperties() const;

  virtual const ParticleProperties* mumsProperties() const;
  virtual ResonancePropertiesFitRef& mumsFittableProperties() const;

  virtual int mumsPID() const;
  virtual double mumsMass() const;
  //virtual double mumsPDGMass() const;
  virtual double mumsWidth() const;
  //virtual double mumsPDGWidth() const;
  virtual double mumsRadius() const; // individual resonance radius
  //virtual double mumsPDGRadius() const; // fixed default value
  virtual double Radius() const; // global resonance radius (equal for all resonances)
    
  virtual int twoLPlusOne() const;
  virtual int lowestPossibleTwoLPlusOne() const;
  // none of all this can handle half-integer spins
  // (i.e. only mesons - so far - should be easy to
  //  adjust one day, but needs to be done).
  virtual int maxDaughterSpinSum() const;
  virtual int minDaughterSpinSum() const;
  virtual int minDaughterSpinSum2() const;
  virtual int minDaughterSpinSum3() const;
  virtual int minDaughterSpinSum4() const;
  virtual int maxDaughterPairSpinSum(int i, int j) const;
  virtual int minDaughterPairSpinSum(int i, int j) const;

  virtual std::string mumsSpin() const;
  virtual int mumsSpinValue() const;
  virtual int mumsParity() const;

  virtual bool startOfDecayChain() const;

  virtual bool parityConservingL(int L)const;

  virtual double mumsRecoMass2() const;
  virtual double mumsRecoMass() const;
  virtual MultiQuarkContent mumsQuarkContent() const;

  bool nonResonant() const;

  virtual int numDaughters() const;
  virtual TLorentzVector daughterP4(int i) const;

  virtual double daughterPDGMass( const int& i ) const;
  virtual double daughterWidth(int i) const;
  virtual double daughterRecoMass2(int i) const;
  virtual double daughterRecoMass(int i) const;
  virtual std::string daughterSpin(int i) const;
  virtual int daughterSpinValue(int i) const;
  virtual MultiQuarkContent daughterQuarkContent(int i) const;
  virtual int daughterP(int i)const;
  virtual int dgtrsInternalParity() const;
  
  virtual MultiQuarkContent dgtrsQuarkContent() const;

  virtual bool isWeakDecay() const;

  /*
     Unnormalised Blatt-Weisskopf Barrier Factors
  */
  virtual double Fr_PDG_BL();
  virtual double Fr_BELLE(double prSquared);
  virtual double Fr_BELLE_Max();

  virtual double Fr();
  virtual double FrForGofM();
  virtual double FrMax();
  virtual double GofM();
  
  virtual std::complex<double> BreitWigner();
  
  //  virtual bool thisIsD() const;
  
  
  virtual void resetInternals();

  virtual void resetPDG();

 public:
  BW_BW( const AssociatedDecayTree& decay);
  BW_BW(const BW_BW& other);
  virtual ~BW_BW();

  virtual std::complex<double> getVal(IDalitzEvent& evt);
    
  virtual void print(IDalitzEvent& evt, std::ostream& out = std::cout);
  virtual void print(std::ostream& out = std::cout) const;

  virtual DalitzCoordinate getDalitzCoordinate(double nSigma=3) const;
  virtual MINT::counted_ptr<IGenFct> generatingFunction() const;

  virtual void setGenerationLimits(double mi, double ma);

  virtual std::string name() const{
    return "BW_BW("+_theDecay.oneLiner() +")";
  }


  virtual std::complex<double> EvtGenValue(IDalitzEvent& evt);
};

std::ostream& operator<<(std::ostream& out, const BW_BW& amp);

#endif
//
