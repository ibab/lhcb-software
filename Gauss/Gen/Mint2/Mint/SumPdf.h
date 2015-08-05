#ifndef SUM_PDF_sums_two_normalised_pdfs_HH
#define SUM_PDF_sums_two_normalised_pdfs_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:56 GMT

#include "Mint/IPdf.h"
#include "Mint/PdfBase.h"
#include "Mint/FitParameter.h"
#include "Mint/IReturnReal.h"

#include <iostream>

namespace MINT{

template<typename EVENT_TYPE>
class SumPdf : public PdfBase<EVENT_TYPE>{
 protected:
  FitParameter& _f1;
  IPdf<EVENT_TYPE>&  _pdf_1;
  IPdf<EVENT_TYPE>&  _pdf_2;

 public:
  SumPdf( FitParameter& f1
	 , IPdf<EVENT_TYPE>& pdf_1
	 , IPdf<EVENT_TYPE>& pdf_2)
    : PdfBase<EVENT_TYPE>()
    , _f1(f1), _pdf_1(pdf_1), _pdf_2(pdf_2)
  {
  }

  SumPdf(const SumPdf& other)
    : IReturnRealForEvent<EVENT_TYPE>()
    , IPdf<EVENT_TYPE>()
    , PdfBase<EVENT_TYPE>()
    , _f1(other._f1), _pdf_1(other._pdf_1), _pdf_2(other._pdf_2)
  {
  }

  virtual double getVal(EVENT_TYPE & evt){
    bool dbThis=false;
    if(dbThis) std::cout << "SumPdf::getVal(): you called?" << std::endl;
    double f = (double) _f1;
    double returnVal = f * _pdf_1.getVal(evt) + (1.0 - f) * _pdf_2.getVal(evt);
    if(dbThis) std::cout << "SumPdf::getVal(): returning " << returnVal
			 << std::endl;
    return returnVal;
  }

  virtual void beginFit(){
    bool dbThis=false;
    if(dbThis) std::cout << "SumPdf::beginFit(): you called?" << std::endl;
    if(dbThis) std::cout << "... calling beginFit PDF 1" << std::endl;
    _pdf_1.beginFit();
    if(dbThis) std::cout << "... calling beginFit PDF 2" << std::endl;
    _pdf_2.beginFit();
    if(dbThis) std::cout << "SumPdf::beginFit(): returning" << std::endl;
  }
  virtual void parametersChanged(){
    bool dbThis=false;
    if(dbThis) std::cout << "SumPdf::parametersChanged you called?" 
			 << std::endl;
    if(dbThis) std::cout << "... parametersChanged pdf 1" << std::endl;
    _pdf_1.parametersChanged();
    if(dbThis) std::cout << "... parametersChanged pdf 2" << std::endl;
    _pdf_2.parametersChanged();
    if(dbThis) std::cout << "SumPdf::parametersChanged: done." 
			 << std::endl;
  }
  virtual void endFit(){
    _pdf_1.endFit();
    _pdf_2.endFit();
  }
    
  virtual void Gradient(EVENT_TYPE & evt, Double_t* grad, MINT::MinuitParameterSet* mps){
      Double_t grad_1[mps->size()]; 
      Double_t grad_2[mps->size()];  

      for(unsigned int j=0; j < mps->size(); j++){
          grad_1[j]= 0.;   
          grad_2[j]= 0.;
      }
      
      _pdf_1.Gradient(evt, grad_1,mps);
      _pdf_2.Gradient(evt, grad_2,mps);
      
      for(unsigned int j=0; j < mps->size(); j++){
          grad[j]= _f1 * grad_1[j] + (1.0 - _f1) * grad_2[j];   
      }

  }
    
  virtual bool useAnalyticGradient(){
      return _pdf_1.useAnalyticGradient() && _pdf_2.useAnalyticGradient();
  }

  virtual ~SumPdf(){}
};

}//namespace MINT
#endif
//
