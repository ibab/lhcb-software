#ifndef DALITZ_PDF_BASE_HH
#define DALITZ_PDF_BASE_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:01 GMT

#include "PdfBase.h"
#include "IEventGenerator.h"
#include "IDalitzEvent.h"
#include "IDalitzEventAccess.h"
#include "IGetRealEvent.h"
#include "DalitzEvent.h"

#include "IDalitzPdf.h"

#include "DalitzMCIntegrator.h"

class DalitzPdfBase : 
public MINT::PdfBase<IDalitzEvent>
, virtual public IDalitzPdf{
  double _norm;
  double _precision;
  DalitzMCIntegrator _mcint;

  MINT::IEventGenerator<IDalitzEvent>* _generator;

  virtual double un_normalised_noPs()=0;
  virtual double phaseSpace();
  bool getNorm();
  bool integrating();

 public:
  bool _integrating;

  DalitzPdfBase(IDalitzEventAccess* events=0
		, MINT::IEventGenerator<IDalitzEvent>* generator=0
		, double precision = 1.e-2
		);
  DalitzPdfBase(IDalitzEventList* events=0
		, MINT::IEventGenerator<IDalitzEvent>* generator=0
		, double precision = 1.e-2
		);


  void setIntegrationPrecision(double prec);
  void setEventGenerator(MINT::IEventGenerator<IDalitzEvent>* g){_generator=g;}
  MINT::IEventGenerator<IDalitzEvent>* getEventGenerator(){return _generator;}
  const MINT::IEventGenerator<IDalitzEvent>* getEventGenerator() const{return _generator;}

  void parametersChanged();
  virtual double getVal();
  virtual double getVal_noPs();
  virtual double getVal_withPs();

  virtual double getVal(IDalitzEvent* evt);
  virtual double getVal_noPs(IDalitzEvent* evt);
  virtual double getVal_withPs(IDalitzEvent* evt);

  double RealVal(){return getVal();}// implements MINT::IGetRealEvent
};

#endif
//
