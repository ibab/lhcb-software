#ifndef DALITZ_PDF_BASE_HH
#define DALITZ_PDF_BASE_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:01 GMT

#include "Mint/PdfBase.h"
#include "Mint/IEventGenerator.h"
#include "Mint/IDalitzEvent.h"
#include "Mint/IReturnRealForEvent.h"
#include "Mint/DalitzEvent.h"

#include "Mint/IDalitzPdf.h"

#include "Mint/DalitzMCIntegrator.h"
#include "Mint/DalitzHistoSet.h"

class DalitzPdfBase : 
public MINT::PdfBase<IDalitzEvent>
, virtual public IDalitzPdf
  , virtual public MINT::IReturnRealForEvent<IDalitzEvent>
{
  DalitzEventPattern _pat;
  double _norm;
  double _precision;
  DalitzMCIntegrator _mcint;

  MINT::IEventGenerator<IDalitzEvent>* _generator;

  virtual double un_normalised_noPs(IDalitzEvent& evt)=0;
  virtual double phaseSpace(IDalitzEvent& evt);
  bool getNorm();
  bool integrating();

 public:
  bool _integrating;

  DalitzPdfBase(MINT::IEventGenerator<IDalitzEvent>* generator=0
		, double precision = 1.e-2
		);

  void setIntegrationPrecision(double prec);
  void setEventGenerator(MINT::IEventGenerator<IDalitzEvent>* g){_generator=g;}
  MINT::IEventGenerator<IDalitzEvent>* getEventGenerator(){return _generator;}
  const MINT::IEventGenerator<IDalitzEvent>* getEventGenerator() const{
    return _generator;}

  virtual void beginFit();
  virtual void parametersChanged();
  virtual void endFit();

  virtual double getVal(IDalitzEvent& evt);
  virtual double getVal_noPs(IDalitzEvent& evt);
  virtual double getVal_withPs(IDalitzEvent& evt);

  // the following three are for backward compatiblity
  // and will disappear soon:
  virtual double getVal(IDalitzEvent* evt){
    if(0 == evt) return 0;
    return getVal(*evt);
  }
  virtual double getVal_noPs(IDalitzEvent* evt){
    if(0 == evt) return 0;
    return getVal_noPs(*evt);
  }
  virtual double getVal_withPs(IDalitzEvent* evt){
    if(0 == evt) return 0;
    return getVal_withPs(*evt);
  }


  double RealVal(IDalitzEvent& evt){return getVal(evt);}
  // implements MINT::IGetRealEvent

  DalitzHistoSet histoSet(){return _mcint.histoSet();}

};

#endif
//
