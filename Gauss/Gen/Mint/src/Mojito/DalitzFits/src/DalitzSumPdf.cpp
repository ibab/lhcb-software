//
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

#include "Mint/DalitzSumPdf.h"

#include <iostream>

using namespace std;
using namespace MINT;

DalitzSumPdf::DalitzSumPdf(const DalitzSumPdf& other)
  : IBasicEventAccess<IDalitzEvent>()
  , IEventAccess<IDalitzEvent>()
  , IReturnReal()
  , IGetRealEvent<IDalitzEvent>()
  , IPdf<IDalitzEvent>()
  , IDalitzPdf()
  , SumPdf<IDalitzEvent>(other)
  , _dalitz_pdf_1(other._dalitz_pdf_1), _dalitz_pdf_2(other._dalitz_pdf_2)
{
  cout << "DON'T USE copy-constructor WITH DalitzSumPdf!!" << endl;
}

DalitzSumPdf::DalitzSumPdf(MINT::IEventList<IDalitzEvent>* events
			   , MINT::FitParameter& f1
			   , IDalitzPdf& pdf_1
			   , IDalitzPdf& pdf_2)
  : SumPdf<IDalitzEvent>(events
			 , f1
			 , pdf_1
			 , pdf_2)
  , _dalitz_pdf_1(pdf_1)
  , _dalitz_pdf_2(pdf_2)
{
}
DalitzSumPdf::DalitzSumPdf(MINT::IEventAccess<IDalitzEvent>* events
			   , MINT::FitParameter& f1
			   , IDalitzPdf& pdf_1
			   , IDalitzPdf& pdf_2)
  : SumPdf<IDalitzEvent>(events
			 , f1
			 , pdf_1
			 , pdf_2
			 )
  , _dalitz_pdf_1(pdf_1)
  , _dalitz_pdf_2(pdf_2)
{
}

double DalitzSumPdf::phaseSpace(){
  IDalitzEvent* evt = getEvent();
  if(0 == evt) return 0;
  return evt->phaseSpace();
}
double DalitzSumPdf::getVal(){
  bool dbThis=false;
  if(dbThis) cout << "DalitzSumPdf::getVal(): you called? " << endl;
  return getVal_noPs();
}
double DalitzSumPdf::getVal_noPs(){
  bool dbThis=false;
  if(dbThis) cout << "DalitzSumPdf::getVal_noPs(): you called? " << endl;
  setPdfsEventRecords();
  double returnVal = 
    _f1         * _dalitz_pdf_1.getVal_noPs() +
    (1.0 - _f1) * _dalitz_pdf_2.getVal_noPs();
  resetPdfsEventRecords();
  if(dbThis) cout << "DalitzSumPdf::getVal_noPs(): returning "
		  << returnVal << endl;
  return returnVal;
}
double DalitzSumPdf::getVal_withPs(){
  return getVal_noPs()*phaseSpace();
}

double DalitzSumPdf::getVal(IDalitzEvent* evt){
  this->setEvent(evt);
  double result = this->getVal();
  this->resetEventRecord();
  return result;
}

double DalitzSumPdf::getVal_noPs(IDalitzEvent* evt){
  this->setEvent(evt);
  double result = this->getVal_noPs();
  this->resetEventRecord();
  return result;
}

double DalitzSumPdf::getVal_withPs(IDalitzEvent* evt){
  this->setEvent(evt);
  double result = this->getVal_withPs();
  this->resetEventRecord();
  return result;
}

//
