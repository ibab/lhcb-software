//
#include "Mint/SumPdf.h"
#include "Mint/IDalitzEvent.h"
#include "Mint/PdfBase.h"

#include "Mint/IDalitzPdf.h"
#include "Mint/FitParameter.h"
#include "Mint/IPdf.h"
#include "Mint/IEventList.h"

#include "Mint/IDalitzEvent.h"

#include "Mint/DalitzSumPdf.h"

#include <iostream>

using namespace std;
using namespace MINT;

DalitzSumPdf::DalitzSumPdf(const DalitzSumPdf& other)
  : IReturnRealForEvent<IDalitzEvent>()
  , IPdf<IDalitzEvent>()
  , IDalitzPdf()
  , SumPdf<IDalitzEvent>(other)
  , _dalitz_pdf_1(other._dalitz_pdf_1), _dalitz_pdf_2(other._dalitz_pdf_2)
{
  cout << "DON'T USE copy-constructor WITH DalitzSumPdf!!" << endl;
}

DalitzSumPdf::DalitzSumPdf(MINT::FitParameter& f1
			   , IDalitzPdf& pdf_1
			   , IDalitzPdf& pdf_2)
  : SumPdf<IDalitzEvent>( f1
			 , pdf_1
			 , pdf_2)
  , _dalitz_pdf_1(pdf_1)
  , _dalitz_pdf_2(pdf_2)
{
}

double DalitzSumPdf::phaseSpace(IDalitzEvent& evt){
  return evt.phaseSpace();
}
double DalitzSumPdf::getVal(IDalitzEvent& evt){
  bool dbThis=false;
  if(dbThis) cout << "DalitzSumPdf::getVal(): you called? " << endl;
  return getVal_noPs(evt);
}
double DalitzSumPdf::getVal_noPs(IDalitzEvent& evt){
  bool dbThis=false;
  if(dbThis) cout << "DalitzSumPdf::getVal_noPs(): you called? " << endl;
  double returnVal = 
    _f1         * _dalitz_pdf_1.getVal_noPs(evt) +
    (1.0 - _f1) * _dalitz_pdf_2.getVal_noPs(evt);
  if(dbThis) cout << "DalitzSumPdf::getVal_noPs(): returning "
		  << returnVal << endl;
  return returnVal;
}
double DalitzSumPdf::getVal_withPs(IDalitzEvent& evt){
  return getVal_noPs(evt)*phaseSpace(evt);
}

//
