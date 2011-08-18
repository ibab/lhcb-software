#ifndef DALITZ_SUM_PDF_HH
#define DALITZ_SUM_PDF_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:02 GMT

#include "Mint/Mint/PdfAndLogL/SumPdf.h"
#include "Mint/Mojito/DalitzEvents/IDalitzEvent.h"
#include "Mint/Mint/PdfAndLogL/PdfBase.h"

#include "Mint/Mojito/DalitzFits/IDalitzPdf.h"
#include "Mint/Mint/FitParameter/FitParameter.h"
#include "Mint/Mint/PdfAndLogL/IPdf.h"
#include "Mint/Mint/Events/IEventList.h"

#include "Mint/Mojito/DalitzEvents/IDalitzEvent.h"
#include "Mint/Mojito/DalitzEvents/IDalitzEventAccess.h"
#include "Mint/Mint/Events/IGetRealEvent.h"


class DalitzSumPdf 
: public MINT::SumPdf<IDalitzEvent>
,  virtual public IDalitzPdf{
 protected:
  IDalitzPdf& _dalitz_pdf_1;
  IDalitzPdf& _dalitz_pdf_2;

 private: // no copying
  DalitzSumPdf(const DalitzSumPdf& other);

 public:
  DalitzSumPdf(MINT::IEventList<IDalitzEvent>* events
	       , MINT::FitParameter& f1
	       , IDalitzPdf& pdf_1
	       , IDalitzPdf& pdf_2);
  
  DalitzSumPdf(MINT::IEventAccess<IDalitzEvent>* events
	       , MINT::FitParameter& f1
	       , IDalitzPdf& pdf_1
	       , IDalitzPdf& pdf_2);
  
  virtual double phaseSpace();
  virtual double getVal();
  virtual double getVal_noPs();
  virtual double getVal_withPs();
  virtual double RealVal(){return getVal();}

  virtual double getVal(IDalitzEvent* evt);
  virtual double getVal_noPs(IDalitzEvent* evt);
  virtual double getVal_withPs(IDalitzEvent* evt);

};

#endif
//
