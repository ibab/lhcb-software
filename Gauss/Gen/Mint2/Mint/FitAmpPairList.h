#ifndef FIT_AMP_PAIR_LIST_HH
#define FIT_AMP_PAIR_LIST_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:03 GMT

#include "Mint/FitAmpPair.h"
#include "Mint/DalitzHistoSet.h"
#include "Mint/FitAmpPairCovariance.h"
#include "Mint/FitFractionList.h"

#include "Mint/IReturnRealForEvent.h"
#include "Mint/IIntegrationCalculator.h"

#include "Mint/IDalitzEvent.h"

#include "Mint/counted_ptr.h"
#include <vector>


class FitAmplitude;
class IDalitzEvent;
namespace MINT{
  class Minimiser;
}

class FitAmpPairList 
: public std::vector<FitAmpPair>
, virtual public IIntegrationCalculator
{
  int _Nevents;
  double _sum;
  double _sumsq;

  double _psSum;
  double _psSumSq;

  mutable FitAmpPairCovariance _cov;

  MINT::counted_ptr<MINT::IReturnRealForEvent<IDalitzEvent> > _efficiency;

  FitFractionList _singleAmpFractions, _interferenceFractions;

  double phaseSpaceIntegral()const; // only for debug

  std::string dirName() const;
  bool makeDirectory(const std::string& asSubdirOf=".")const;
  virtual double oldVariance() const;

  bool reset();
 public:
  FitAmpPairList();
  FitAmpPairList(const FitAmpPairList& other);
  MINT::counted_ptr<IIntegrationCalculator> 
    clone_IIntegrationCalculator() const;

  virtual void addAmps(FitAmplitude* a1, FitAmplitude* a2); // for backwards compatibility, will be removed
  virtual void addAmps(FitAmplitude& a1, FitAmplitude& a2);
  virtual void addEvent(IDalitzEvent* evtPtr, double weight=1);
  virtual void addEvent(MINT::counted_ptr<IDalitzEvent> evtPtr
			, double weight=1);
  virtual void reAddEvent(IDalitzEvent* evtPtr, double weight=1);
  virtual void reAddEvent(MINT::counted_ptr<IDalitzEvent> evtPtr
			, double weight=1);

  bool isCompatibleWith(const FitAmpPairList& other) const;
  virtual bool add(const FitAmpPairList& otherList);
  virtual bool add(const FitAmpPairList* otherListPtr);
  virtual bool add(MINT::const_counted_ptr<FitAmpPairList> otherListPtr);

  virtual bool append(const FitAmpPairList& otherListPtr);
  virtual bool append(const FitAmpPairList* otherListPtr);
  virtual bool append(MINT::const_counted_ptr<FitAmpPairList> otherListPtr);

  virtual int numEvents() const;
  virtual double integral() const;
  virtual double variance() const;
  virtual double sumOfVariances() const;
  virtual void Gradient(MINT::MinuitParameterSet* mps, Double_t* grad);
  virtual void GradientForLasso(MINT::MinuitParameterSet* mps, Double_t* grad);
    
  double sumOfSqrtFitFractions();
  int numberOfFitFractionsLargerThanThreshold(double threshold);
    
  FitFractionList getFractions() const{return _singleAmpFractions;}
  FitFractionList getInterferenceTerms() const{return _interferenceFractions;}

  void setEfficiency(MINT::counted_ptr<MINT::IReturnRealForEvent<IDalitzEvent> > eff);
  void unsetEfficiency();
  double efficiency(IDalitzEvent* evtPtr);

  virtual bool makeAndStoreFractions(MINT::Minimiser* mini=0){
    return makeAndStoreFractions("FitAmpResults.txt", mini);}
  virtual bool makeAndStoreFractions(const std::string& fname
				     , MINT::Minimiser* min=0);
  virtual double getFractionChi2() const;
  
  virtual DalitzHistoSet histoSet() const;
  void saveEachAmpsHistograms(const std::string& prefix) const;
  std::vector<DalitzHistoSet> GetEachAmpsHistograms();

  DalitzHistoSet interferenceHistoSet() const;
  void saveInterferenceHistograms(const std::string& prefix) const;
  std::vector<DalitzHistoSet> GetInterferenceHistograms();

  virtual void doFinalStats(MINT::Minimiser* min=0);

  virtual bool save(const std::string& asSubdirOf=".") const;
  virtual bool retrieve(const std::string& asSubdirOf=".") ;

  virtual void print(std::ostream& os=std::cout) const;

  bool needToReIntegrate() const;
  void startIntegration();
  void startReIntegration();
  void endIntegration();


  FitAmpPairList& operator+=(const FitAmpPairList& other);
  FitAmpPairList operator+(const FitAmpPairList& other) const;
};

std::ostream& operator<<(std::ostream& os, const FitAmpPairList& fap);

#endif
//
