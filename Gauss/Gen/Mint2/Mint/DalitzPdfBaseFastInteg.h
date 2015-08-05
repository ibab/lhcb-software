#ifndef DALITZ_PDF_BASE_FAST_INTEG_HH
#define DALITZ_PDF_BASE_FAST_INTEG_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:01 GMT

#include "Mint/PdfBase.h"
#include "Mint/IReturnRealForEvent.h"

#include "Mint/IEventGenerator.h"
#include "Mint/IDalitzEvent.h"
#include "Mint/DalitzEventPattern.h"
#include "Mint/IReturnRealForEvent.h"
#include "Mint/DalitzEvent.h"
#include "Mint/MinuitParameterSet.h"
#include "Mint/DalitzHistoSet.h"
#include "Mint/FastAmplitudeIntegrator.h"

#include "Mint/counted_ptr.h"

#include <string>

//#define CHECK_INTEGRATOR

#ifdef CHECK_INTEGRATOR
#include "Mint/DalitzMCIntegrator.h"
#endif

#include "Mint/IFastAmplitudeIntegrable.h"

#include "Mint/IDalitzPdf.h"

/*
  As a user, you will need to implement this method:

  virtual double un_normalised_noPs(IDalitzEvent& evt);

  it returns the PDF without the phase-space factor,
  no need for normalisation. Typical implementation if you 
  have a FitAmpSum called _amps:

  double un_normalised_noPs(IDalitzEvent& evt){
    return  _amps->RealVal(evt);
  }
  
 */

namespace MINT{
  class Minimiser;
}

class DalitzPdfBaseFastInteg 
: public MINT::PdfBase<IDalitzEvent>
, virtual public IDalitzPdf
{
 protected:
  MINT::MinuitParameterSet* _mps;
  DalitzEventPattern _pat;
  double _norm;
  double _precision;
#ifdef CHECK_INTEGRATOR
  DalitzMCIntegrator _mcint;
#endif
  FastAmplitudeIntegrator _faint;
  IFastAmplitudeIntegrable* _amps;
  MINT::counted_ptr<IFastAmplitudeIntegrable> _countedAmps;
  double _val;  
  Double_t* _gradNorm; 
  bool _redoGradNorm;  
  //  MINT::counted_ptr<IGetDalitzEvent> _efficiency;

  MINT::IEventGenerator<IDalitzEvent>* _generator;
  bool _integrating;
  MINT::counted_ptr<MINT::IEventGenerator<IDalitzEvent> > _defaultGenerator;

  std::string _commaSepList_of_SavedIntegrators;

  void setup();
  bool makeAmps();
    
  virtual double un_normalised_noPs(IDalitzEvent& evt)=0;
  // This replaces un_normalised. Make sure it 
  // does not contain the phase space component.
  // ... turned out that it's important in several
  // places to separate this out.

  virtual double phaseSpace(IDalitzEvent& evt);
  virtual bool getNorm();
  virtual bool integrating();

  IFastAmplitudeIntegrable* getAmps(){ return _amps; }


  MINT::IEventGenerator<IDalitzEvent>* makeDefaultGenerator();

 public:
  DalitzPdfBaseFastInteg(const DalitzEventPattern& pat
			 , MINT::IEventGenerator<IDalitzEvent>* generator
			 , IFastAmplitudeIntegrable* amps
			 , double precision = 1.e-3
			 );

  DalitzPdfBaseFastInteg( const DalitzEventPattern& pat
			  , MINT::IEventGenerator<IDalitzEvent>* generator=0
			  //, MINT::counted_ptr<IGetDalitzEvent> eff=0
			  , MINT::MinuitParameterSet* mps=0
			  , double precision = 1.e-3
			 );

  DalitzPdfBaseFastInteg(const  DalitzPdfBaseFastInteg& other);

  virtual ~DalitzPdfBaseFastInteg();

  void setIntegrationPrecision(double prec);
  void setEventGenerator(MINT::IEventGenerator<IDalitzEvent>* g){_generator=g;}
  MINT::IEventGenerator<IDalitzEvent>* getEventGenerator();
  const MINT::IEventGenerator<IDalitzEvent>* getEventGenerator() const;

  void parametersChanged();
  virtual double getVal(IDalitzEvent& evt);
  virtual double getVal_withPs(IDalitzEvent& evt);
  virtual double getVal_noPs(IDalitzEvent& evt);

  virtual double getNewVal(IDalitzEvent& evt){
    this->parametersChanged();
    return getVal(evt);
  };

  // next 4 for backward compability (will be removed one day):
  virtual double getVal(IDalitzEvent* evt){
    if(0 == evt) return 0;
    return getVal(*evt);
  }
  virtual double getVal_withPs(IDalitzEvent* evt){
    if(0 == evt) return 0;
    return getVal_withPs(*evt);
  }
  virtual double getVal_noPs(IDalitzEvent* evt){
    if(0 == evt) return 0;
    return getVal_noPs(*evt);
  }
  virtual double getNewVal(IDalitzEvent* evt){
    this->parametersChanged();
    if(0 == evt) return 0;
    return getNewVal(*evt);
  }
    
  virtual void Gradient(IDalitzEvent& evt,Double_t* grad, MINT::MinuitParameterSet* mps); 
  virtual void GradientForLasso(Double_t* grad);  
  bool useAnalyticGradient() {return getAmps()->useAnalyticGradient();}

  virtual double RealVal(IDalitzEvent& evt){return getVal(evt);}
  // w/o phase space - more robust if your events leak out of
  // the kinematically allowed region.

  MINT::MinuitParameterSet* getMPS();
  const MINT::MinuitParameterSet* getMPS() const;

  bool saveIntegrator(const std::string& fname)const;

  bool makePlots(const std::string& filename) const;

  virtual DalitzHistoSet histoSet() const;
  virtual DalitzHistoSet histoSet();
  void saveEachAmpsHistograms(const std::string& prefix) const;
  std::vector<DalitzHistoSet> GetEachAmpsHistograms();

  virtual DalitzHistoSet interferenceHistoSet() const;
  virtual DalitzHistoSet interferenceHistoSet();
  void saveInterferenceHistograms(const std::string& prefix) const;
  std::vector<DalitzHistoSet> GetInterferenceHistograms();  

  IFastAmplitudeIntegrable* getFitAmpSum(){ return _amps;}
  //double efficiency(){return _efficiency->RealVal();}
  virtual void endFit();

  void setIntegratorFileName(const std::string& commaSeparatedList);

  double getIntegralValue() const{
    return _norm;
  }
  double getIntegralValue(){
    if(_norm < 0) getNorm(); return _norm;
  }
  double redoIntegrator(){
    _norm = -1;
    getNorm();
    return _norm;
  }
    
  double sumOfSqrtFitFractions() {
      if(!_faint.initialised())getNorm();
      return _faint.sumOfSqrtFitFractions();
  }   
    
  int numberOfFitFractionsLargerThanThreshold(double threshold){
        return _faint.numberOfFitFractionsLargerThanThreshold(threshold);
  }
    
  void doFinalStats(MINT::Minimiser* mini=0);
};

#endif
//
