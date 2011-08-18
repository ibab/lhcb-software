#ifndef MINTDALITZ_IDALITZ_PDF_HH
#define MINTDALITZ_IDALITZ_PDF_HH

#include "Mint/Mint/Events/IGetRealEvent.h"
#include "Mint/Mojito/DalitzEvents/IDalitzEvent.h"
#include "Mint/Mint/PdfAndLogL/IPdf.h"

class IDalitzPdf 
: virtual public MINT::IGetRealEvent<IDalitzEvent>
, virtual public MINT::IPdf<IDalitzEvent>
{
 public:
  virtual double getVal()=0;
  virtual double getVal_noPs()=0;
  virtual double getVal_withPs()=0;

  virtual double getVal(IDalitzEvent* evt)=0;
  virtual double getVal_noPs(IDalitzEvent* evt)=0;
  virtual double getVal_withPs(IDalitzEvent* evt)=0;
  //virtual double RealVal()=0;
};

#endif
//
