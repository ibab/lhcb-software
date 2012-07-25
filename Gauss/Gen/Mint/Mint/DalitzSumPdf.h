#ifndef DALITZ_SUM_PDF_HH
#define DALITZ_SUM_PDF_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:02 GMT

#include "Mint/SumPdf.h"
#include "Mint/IDalitzEvent.h"
#include "Mint/PdfBase.h"

#include "Mint/IDalitzPdf.h"
#include "Mint/FitParameter.h"
#include "Mint/IPdf.h"
#include "Mint/IEventList.h"

#include "Mint/IDalitzEvent.h"
#include "Mint/IDalitzEventAccess.h"
#include "Mint/IGetRealEvent.h"


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
    
    virtual DalitzHistoSet histoSet(){
        DalitzHistoSet hset = _f1 * _dalitz_pdf_1.histoSet();
        hset += (1.0-_f1)*_dalitz_pdf_2.histoSet();
        return hset;
    }

};

#endif
//
