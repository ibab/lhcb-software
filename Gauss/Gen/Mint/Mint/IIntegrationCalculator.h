#ifndef I_INTEGRATION_CALCULATOR_HH
#define I_INTEGRATION_CALCULATOR_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:02 GMT

#include "Mint/IDalitzEvent.h"
#include "Mint/counted_ptr.h"
#include "Mint/DalitzHistoSet.h"
#include "Mint/FitFractionList.h"
#include <iostream>

namespace MINT{
  class Minimiser;
}

class IIntegrationCalculator{
 public:
  virtual void addEvent(IDalitzEvent* evtPtr, double weight=1)=0;
  virtual void addEvent(MINT::counted_ptr<IDalitzEvent> evtPtr
			, double weight=1)=0;
  virtual int numEvents()   const=0;
  virtual double integral() const=0;
  virtual double variance() const=0;

  virtual bool makeAndStoreFractions(MINT::Minimiser* mini=0)=0;
  virtual FitFractionList getFractions()const=0;
  //  virtual FitFractionList getInterferenceTerms()const=0;
  
  virtual double getFractionChi2() const=0;

  virtual DalitzHistoSet histoSet() const=0;
  virtual void saveEachAmpsHistograms(const std::string& prefix) const=0;
  virtual void doFinalStats(MINT::Minimiser* mini=0)=0;

  virtual bool save(const std::string& dirname) const=0;
  virtual bool retrieve(const std::string& dirname)=0;

  virtual void print(std::ostream& os=std::cout) const=0;

  virtual MINT::counted_ptr<IIntegrationCalculator> 
    clone_IIntegrationCalculator() const=0;

  virtual ~IIntegrationCalculator(){}
};

#endif
//
