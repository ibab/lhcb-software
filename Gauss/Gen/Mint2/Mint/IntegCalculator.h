#ifndef INTEG_CALCULATOR_HH
#define INTEG_CALCULATOR_HH

#include "Mint/IIntegrationCalculator.h"
#include "Mint/FitAmpPairList.h"
#include "Mint/counted_ptr.h"
#include "Mint/DalitzHistoSet.h"
#include "Mint/IDalitzEvent.h"
#include "Mint/IReturnRealForEvent.h"
#include "Mint/FitFractionList.h"
#include <iostream>

namespace MINT{
  class Minimiser;
}

class IntegCalculator : public virtual IIntegrationCalculator{
 protected:
  FitAmpPairList _withEff, _noEff;
  bool _onlyOneFitAmpPairList;

  FitAmpPairList& withEff();
  FitAmpPairList& noEff();

  static std::string dirNameWithEff();
  static std::string dirNameNoEff();
  bool makeDirectories(const std::string& asSubdirOf=".")const;
 public:
  IntegCalculator();
  IntegCalculator(const IntegCalculator& other);
  IntegCalculator(const FitAmpPairList& wEff);
  virtual MINT::counted_ptr<IIntegrationCalculator> 
    clone_IIntegrationCalculator() const;

  const FitAmpPairList& withEff()const {return _withEff;}
  const FitAmpPairList& noEff()const {return _noEff;}

  void setEfficiency(MINT::counted_ptr<MINT::IReturnRealForEvent<IDalitzEvent> > eff);
  void unsetEfficiency();
  double efficiency(IDalitzEvent* evtPtr);
  
  virtual void addAmps(FitAmplitude* a1, FitAmplitude* a2);
  virtual void addEvent(IDalitzEvent* evtPtr, double weight=1); // << for compatibility
  virtual void addEvent(IDalitzEvent& evt, double weight=1); // << default
  virtual void addEvent(MINT::counted_ptr<IDalitzEvent> evtPtr
			, double weight=1);

  virtual void reAddEvent(IDalitzEvent* evtPtr, double weight=1);
  virtual void reAddEvent(IDalitzEvent& evt, double weight=1); // <<< default
  virtual void reAddEvent(MINT::counted_ptr<IDalitzEvent> evtPtr
			  , double weight=1);

  virtual bool add(const IntegCalculator& other);
  virtual bool add(const IntegCalculator* other);
  virtual bool add(const MINT::const_counted_ptr<IntegCalculator>& other);

  virtual bool append(const IntegCalculator& other);
  virtual bool append(const IntegCalculator* other);
  virtual bool append(const MINT::const_counted_ptr<IntegCalculator>& other);

  virtual int numEvents()   const;
  virtual double integral() const;
  virtual double variance() const;

  virtual std::complex<double> ComplexSum() const;

  virtual void Gradient(MINT::MinuitParameterSet* mps, Double_t* grad){
        withEff().Gradient(mps,grad);
  }
  virtual void GradientForLasso(MINT::MinuitParameterSet* mps, Double_t* grad){
        withEff().GradientForLasso(mps,grad);
  }  
    
  double sumOfSqrtFitFractions() {
        return withEff().sumOfSqrtFitFractions();
  }     
    
  int numberOfFitFractionsLargerThanThreshold(double threshold){
        return withEff().numberOfFitFractionsLargerThanThreshold(threshold);
  }  
    
  virtual bool makeAndStoreFractions(MINT::Minimiser* mini=0);
  virtual double getFractionChi2() const;

  virtual DalitzHistoSet histoSet() const;
  virtual DalitzHistoSet un_normalised_histoSetRe() const;
  virtual DalitzHistoSet un_normalised_histoSetIm() const;
  virtual void saveEachAmpsHistograms(const std::string& prefix) const;
  virtual std::vector<DalitzHistoSet> GetEachAmpsHistograms();

  virtual DalitzHistoSet interferenceHistoSet() const;
  virtual void saveInterferenceHistograms(const std::string& prefix) const;
  virtual std::vector<DalitzHistoSet> GetInterferenceHistograms();

  virtual void doFinalStats(MINT::Minimiser* mini=0);

  virtual bool save(const std::string& dirname) const;
  virtual bool retrieve(const std::string& commaSeparatedList);
  virtual bool retrieveSingle(const std::string& dirname);

  virtual FitFractionList getFractions() const;

  virtual void print(std::ostream& os=std::cout) const;

  bool needToReIntegrate() const;
  void startIntegration();
  void startReIntegration();
  void endIntegration();
 
  virtual ~IntegCalculator(){}

};

#endif
//
