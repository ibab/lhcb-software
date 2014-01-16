#ifndef BREITWIGNER_BLATTWEISSKOPF_HH
#define BREITWIGNER_BLATTWEISSKOPF_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:04 GMT

#include <complex>
#include <string>
#include <iostream>
#include <vector>

#include "Mint/ILineshape.h"
#include "Mint/AssociatedDecayTree.h"
#include "Mint/DalitzEventAccess.h"
#include "Mint/IDalitzEventAccess.h"
//#include "fitSetup.h"

#include "Mint/DalitzCoordinate.h"

#include "Mint/MultiQuarkContent.h"

#include "Mint/GaussFct.h"

#include "Mint/IGenFct.h"

#include "TRandom.h"

class ParticleProperties;

// Breit-Wigner with Blatt-Weisskopf penetration factors.
// Can only do 2 body decays for now... (it's the BW penetration factors)
class BW_BW : public DalitzEventAccess, virtual public ILineshape{
 private:
  mutable double _prSq, _prSqForGofM, _pABSq, _mumsPDGMass, _mumsWidth, 
    _mumsRecoMass2, _mumsRecoMass, _Fr_BELLE, _Fr_PDG_BL, _GofM;
  mutable int _mumsPID;
  mutable bool _mumsPID_set;

  mutable double _mumsRadius;

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
  
 public:
  virtual double prSq() const;
  virtual double prSqForGofM() const;
  virtual double pABSq();

  virtual double prSqMax() const;
 protected:
  
  virtual const ParticleProperties* mumsProperties() const;
  virtual double mumsPDGMass() const;
  virtual int mumsPID() const;

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

  virtual double mumsWidth() const;
  virtual std::string mumsSpin() const;
  virtual int mumsSpinValue() const;
  virtual double mumsRadius() const;
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
  BW_BW( const AssociatedDecayTree& decay
	 , IDalitzEventAccess* events);
  BW_BW(const BW_BW& other);
  virtual ~BW_BW();

  virtual std::complex<double> getVal();
  virtual std::complex<double> getValAtResonance();
  virtual std::complex<double> getSmootherLargerVal();
  virtual void print(std::ostream& out = std::cout);
  virtual void print(std::ostream& out = std::cout) const;

  virtual DalitzCoordinate getDalitzCoordinate(double nSigma=3) const;
  virtual MINT::counted_ptr<IGenFct> generatingFunction() const;

  virtual void setGenerationLimits(double mi, double ma);

  virtual std::string name() const{
    return "BW_BW("+_theDecay.oneLiner() +")";
  }


  virtual std::complex<double> EvtGenValue();
};

std::ostream& operator<<(std::ostream& out, const BW_BW& amp);

#endif
//
