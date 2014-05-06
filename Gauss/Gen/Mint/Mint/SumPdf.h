#ifndef SUM_PDF_sums_two_normalised_pdfs_HH
#define SUM_PDF_sums_two_normalised_pdfs_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:56 GMT

#include "Mint/IPdf.h"
#include "Mint/PdfBase.h"
#include "Mint/FitParameter.h"

#include <iostream>

namespace MINT{

template<typename EVENT_TYPE>
class SumPdf : public PdfBase<EVENT_TYPE>{
 protected:
  FitParameter& _f1;
  IPdf<EVENT_TYPE>&  _pdf_1;
  IPdf<EVENT_TYPE>&  _pdf_2;

 // no copying, therefore protected
  SumPdf(const SumPdf& other)
    : IBasicEventAccess<EVENT_TYPE>()
    , IEventAccess<EVENT_TYPE>()
    , IReturnReal()
    , IGetRealEvent<EVENT_TYPE>()
    , IPdf<EVENT_TYPE>()
    , PdfBase<EVENT_TYPE>()
    , _f1(other._f1), _pdf_1(other._pdf_1), _pdf_2(other._pdf_2)
  {
    std::cout << "DON'T USE copy-constructor WITH SUMPDF!!" << std::endl;
    //    _pdf_1.setDaddy(this);
    //    _pdf_2.setDaddy(this);
  }

 public:
  SumPdf(IEventList<EVENT_TYPE>* events
	 , FitParameter& f1
	 , IPdf<EVENT_TYPE>& pdf_1
	 , IPdf<EVENT_TYPE>& pdf_2)
    : PdfBase<EVENT_TYPE>(events)
    , _f1(f1), _pdf_1(pdf_1), _pdf_2(pdf_2)
  {
    //    _pdf_1.setDaddy(this);
    //    _pdf_2.setDaddy(this);
  }
 SumPdf(IEventAccess<EVENT_TYPE>* events
	, FitParameter& f1
	, IPdf<EVENT_TYPE>& pdf_1
	, IPdf<EVENT_TYPE>& pdf_2)
    : PdfBase<EVENT_TYPE>(events)
    , _f1(f1), _pdf_1(pdf_1), _pdf_2(pdf_2)
  {
    //    _pdf_1.setDaddy(this);
    //    _pdf_2.setDaddy(this);
  }

  virtual void setPdfsEventRecords(){
    bool dbThis=false;
    if(dbThis) std::cout << "SumPdf::setPdfsEventRecords()" << std::endl;
    _pdf_1.setEventRecord(this->getEventRecord());
    if(dbThis) std::cout << ".... set 1 " << std::endl;
    _pdf_2.setEventRecord(this->getEventRecord());
    if(dbThis) std::cout << "... and the other - returning." << std::endl;
  }
  virtual void resetPdfsEventRecords(){
    _pdf_1.resetEventRecord();
    _pdf_2.resetEventRecord();
  }
  virtual double getVal(){
    bool dbThis=false;
    if(dbThis) std::cout << "SumPdf::getVal(): you called?" << std::endl;
    double f = (double) _f1;
    setPdfsEventRecords();
    double returnVal = f * _pdf_1.getVal() + (1.0 - f) * _pdf_2.getVal();
    resetPdfsEventRecords();
    if(dbThis) std::cout << "SumPdf::getVal(): returning " << returnVal
			 << std::endl;
    return returnVal;
  }

  virtual void beginFit(){
    bool dbThis=false;
    if(dbThis) std::cout << "SumPdf::beginFit(): you called?" << std::endl;
    setPdfsEventRecords();
    if(dbThis) std::cout << "... calling beginFit PDF 1" << std::endl;
    _pdf_1.beginFit();
    if(dbThis) std::cout << "... calling beginFit PDF 2" << std::endl;
    _pdf_2.beginFit();
    if(dbThis) std::cout << "... resetting event records" << std::endl;
    resetPdfsEventRecords();
    if(dbThis) std::cout << "SumPdf::beginFit(): returning" << std::endl;
  }
  virtual void parametersChanged(){
    bool dbThis=false;
    if(dbThis) std::cout << "SumPdf::parametersChanged you called?" 
			 << std::endl;
    setPdfsEventRecords();
    if(dbThis) std::cout << "... parametersChanged pdf 1" << std::endl;
    _pdf_1.parametersChanged();
    if(dbThis) std::cout << "... parametersChanged pdf 2" << std::endl;
    _pdf_2.parametersChanged();
    if(dbThis) std::cout << "... resetting event records" << std::endl;
    resetPdfsEventRecords();
    if(dbThis) std::cout << "SumPdf::parametersChanged: done." 
			 << std::endl;
  }
  virtual void endFit(){
    setPdfsEventRecords();
    _pdf_1.endFit();
    _pdf_2.endFit();
    resetPdfsEventRecords();
  }

  virtual ~SumPdf(){}
};

}//namespace MINT
#endif
//
