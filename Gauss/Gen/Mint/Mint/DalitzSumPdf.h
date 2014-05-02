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
#include "Mint/IReturnRealForEvent.h"


class DalitzSumPdf 
: public MINT::SumPdf<IDalitzEvent>
,  virtual public IDalitzPdf{
 protected:
  IDalitzPdf& _dalitz_pdf_1;
  IDalitzPdf& _dalitz_pdf_2;

 private: // no copying
  DalitzSumPdf(const DalitzSumPdf& other);

 public:
  DalitzSumPdf(	MINT::FitParameter& f1
	       , IDalitzPdf& pdf_1
	       , IDalitzPdf& pdf_2);
  
  virtual double phaseSpace(IDalitzEvent& evt);
  virtual double getVal(IDalitzEvent& evt);
  virtual double getVal_noPs(IDalitzEvent& evt);
  virtual double getVal_withPs(IDalitzEvent& evt);
  virtual double RealVal(IDalitzEvent& evt){return getVal(evt);}

  // the next three are for backward compatibility only
  // and will disappear in the near future.
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
    
  virtual DalitzHistoSet histoSet(){
    DalitzHistoSet hset = _f1 * _dalitz_pdf_1.histoSet();
    hset += (1.0-_f1)*_dalitz_pdf_2.histoSet();
    return hset;
  }

};

#endif
//
