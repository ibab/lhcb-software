#ifndef DALITZ_PDF_BASE_MC_INTEG_HH
#define DALITZ_PDF_BASE_MC_INTEG_HH
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

#include "Mint/DalitzMCIntegrator.h"

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

class DalitzPdfBaseMCInteg 
: public MINT::PdfBase<IDalitzEvent>
, virtual public IDalitzPdf
{
 protected:
  MINT::MinuitParameterSet* _mps;
  DalitzEventPattern _pat;
  double _norm;
  double _precision;
  DalitzMCIntegrator _mcint;
  IFastAmplitudeIntegrable* _amps;
  double _val;   
    
  MINT::IEventGenerator<IDalitzEvent>* _generator;
  bool _integrating;
  MINT::counted_ptr<MINT::IEventGenerator<IDalitzEvent> > _defaultGenerator;

  virtual double un_normalised_noPs(IDalitzEvent& evt)=0;
  // This replaces un_normalised. Make sure it 
  // does not contain the phase space component.
  // ... turned out that it's important in several
  // places to separate this out.

  virtual double phaseSpace(IDalitzEvent& evt);
  virtual bool getNorm();
  virtual bool integrating();

  IFastAmplitudeIntegrable* getAmps();
  
  MINT::IEventGenerator<IDalitzEvent>* makeDefaultGenerator();

 public:
  DalitzPdfBaseMCInteg(const DalitzEventPattern& pat, IFastAmplitudeIntegrable* amps
			 , double precision = 1.e-3
			 , MINT::IEventGenerator<IDalitzEvent>* generator=0, MINT::MinuitParameterSet* mps=0
			 );


  DalitzPdfBaseMCInteg(const  DalitzPdfBaseMCInteg& other);

  virtual ~DalitzPdfBaseMCInteg();

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

  virtual double RealVal(IDalitzEvent& evt){return getVal(evt);}
  // w/o phase space - more robust if your events leak out of
  // the kinematically allowed region.

  MINT::MinuitParameterSet* getMPS();
  const MINT::MinuitParameterSet* getMPS() const;
  
  virtual DalitzHistoSet histoSet();
  /*
  bool makePlots(const std::string& filename) const;
  virtual DalitzHistoSet histoSet() const;
  void saveEachAmpsHistograms(const std::string& prefix) const;
  std::vector<DalitzHistoSet> GetEachAmpsHistograms();

  virtual DalitzHistoSet interferenceHistoSet() const;
  virtual DalitzHistoSet interferenceHistoSet();
  void saveInterferenceHistograms(const std::string& prefix) const;
  std::vector<DalitzHistoSet> GetInterferenceHistograms();  
  */
  IFastAmplitudeIntegrable* getFitAmpSum(){ return _amps;}
  virtual void endFit();

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

  void doFinalStats(MINT::Minimiser* mini=0);
};

#endif
//
