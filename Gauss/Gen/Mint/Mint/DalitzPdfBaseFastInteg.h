#ifndef DALITZ_PDF_BASE_FAST_INTEG_HH
#define DALITZ_PDF_BASE_FAST_INTEG_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:01 GMT

#include "Mint/PdfBase.h"
#include "Mint/IGetDalitzEvent.h"

#include "Mint/IEventGenerator.h"
#include "Mint/IDalitzEvent.h"
#include "Mint/IDalitzEventAccess.h"
#include "Mint/IGetRealEvent.h"
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

namespace MINT{
  class Minimiser;
}

class DalitzPdfBaseFastInteg 
: public MINT::PdfBase<IDalitzEvent>
, virtual public IDalitzEventAccess
, virtual public IDalitzPdf
{
 protected:
  double _norm;
  double _precision;
#ifdef CHECK_INTEGRATOR
  DalitzMCIntegrator _mcint;
#endif
  FastAmplitudeIntegrator _faint;
  IFastAmplitudeIntegrable* _amps;
  MINT::counted_ptr<IFastAmplitudeIntegrable> _countedAmps;

  //  MINT::counted_ptr<IGetDalitzEvent> _efficiency;

  MINT::IEventGenerator<IDalitzEvent>* _generator;
  bool _integrating;
  MINT::counted_ptr<MINT::IEventGenerator<IDalitzEvent> > _defaultGenerator;

  std::string _commaSepList_of_SavedIntegrators;

  virtual double un_normalised_noPs()=0;
  // This replaces un_normalised. Make sure it 
  // does not contain the phase space component.
  // ... turned out that it's important in several
  // places to separate this out.

  virtual double phaseSpace();
  virtual bool getNorm();
  virtual bool integrating();

  const IFastAmplitudeIntegrable* getAmps() const;
  IFastAmplitudeIntegrable* getAmps();
  
  MINT::IEventGenerator<IDalitzEvent>* makeDefaultGenerator();

 public:
  DalitzPdfBaseFastInteg(IDalitzEventAccess* events
			 , MINT::IEventGenerator<IDalitzEvent>* generator
			 , IFastAmplitudeIntegrable* amps
			 , double precision = 1.e-3
			 );

  DalitzPdfBaseFastInteg(IDalitzEventList* events
			 , MINT::IEventGenerator<IDalitzEvent>* generator
			 , IFastAmplitudeIntegrable* amps
			 , double precision = 1.e-3
			 );

  /*
  DalitzPdfBaseFastInteg( IDalitzEventAccess* events
			  , MINT::IEventGenerator<IDalitzEvent>* generator
			  , IFastAmplitudeIntegrable* amps
			  , MINT::counted_ptr<IGetDalitzEvent> eff
			  , double precision = 1.e-3
			 );

  DalitzPdfBaseFastInteg(IDalitzEventList* events
			 , MINT::IEventGenerator<IDalitzEvent>* generator
			 , IFastAmplitudeIntegrable* amps
			  , MINT::counted_ptr<IGetDalitzEvent> eff
			 , double precision = 1.e-3
			 );
  */
  DalitzPdfBaseFastInteg( IDalitzEventAccess* events=0
			  , MINT::IEventGenerator<IDalitzEvent>* generator=0
			  //, MINT::counted_ptr<IGetDalitzEvent> eff=0
			  , MINT::MinuitParameterSet* mps=0
			  , double precision = 1.e-3
			 );
  DalitzPdfBaseFastInteg(IDalitzEventList* events=0
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
  virtual double getVal();
  virtual double getVal_withPs();
  virtual double getVal_noPs();

  virtual double getVal(IDalitzEvent* evt);
  virtual double getNewVal(){
    this->parametersChanged();
    return getVal();
  };
  virtual double getNewVal(IDalitzEvent* evt){
    this->parametersChanged();
    return getVal(evt);
  };
  virtual double getVal_withPs(IDalitzEvent* evt);
  virtual double getVal_noPs(IDalitzEvent* evt);

  virtual double RealVal(){return getVal();}
  // w/o phase space - more robust if your events leak out of
  // the kinematically allowed region.

  bool saveIntegrator(const std::string& fname)const;

  bool makePlots(const std::string& filename) const;
  virtual DalitzHistoSet histoSet() const;
  virtual DalitzHistoSet histoSet();
  void saveEachAmpsHistograms(const std::string& prefix) const;
  std::vector<DalitzHistoSet> GetEachAmpsHistograms();

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

  void doFinalStats(MINT::Minimiser* mini=0);
};

#endif
//
