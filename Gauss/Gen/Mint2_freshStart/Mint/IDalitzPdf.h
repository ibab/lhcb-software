#ifndef MINTDALITZ_IDALITZ_PDF_HH
#define MINTDALITZ_IDALITZ_PDF_HH

#include "Mint/IReturnRealForEvent.h"
#include "Mint/IDalitzEvent.h"
#include "Mint/IPdf.h"
#include "Mint/DalitzHistoSet.h"

class IDalitzPdf 
: virtual public MINT::IReturnRealForEvent<IDalitzEvent>
, virtual public MINT::IPdf<IDalitzEvent>
{
 public:
  virtual double getVal(IDalitzEvent& evt)=0;
  virtual double getVal_noPs(IDalitzEvent& evt)=0;
  virtual double getVal_withPs(IDalitzEvent& evt)=0;

  // the following three are for backward compatiblity
  // and will disappear soon:
  virtual double getVal(IDalitzEvent* evt)=0;
  virtual double getVal_noPs(IDalitzEvent* evt)=0;
  virtual double getVal_withPs(IDalitzEvent* evt)=0;

  virtual DalitzHistoSet histoSet()=0;
  //virtual double RealVal(IDalitzEvent& evgt)=0;
};

#endif
//
