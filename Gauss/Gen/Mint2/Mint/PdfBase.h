#ifndef PDF_BASE_HH
#define PDF_BASE_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:56 GMT

#include "Mint/IPdf.h"
#include "Mint/IEventList.h"
#include "Mint/IReturnRealForEvent.h"

namespace MINT{

  template<typename EVENT_TYPE>
    class PdfBase : virtual public IPdf<EVENT_TYPE>, virtual public IReturnRealForEvent<EVENT_TYPE>{
  public:
    PdfBase(){};
    PdfBase(const PdfBase<EVENT_TYPE>& )
      : IReturnRealForEvent<EVENT_TYPE>()
      , IPdf<EVENT_TYPE>()
      {};
 
   IPdf<EVENT_TYPE>* Clone() const{
     return new PdfBase(*this);     
   } 
    
    virtual double getVal(EVENT_TYPE & evt)=0;
    virtual double RealVal(EVENT_TYPE & evt){
      return getVal(evt);
    }
    
    virtual void beginFit(){};
    virtual void parametersChanged(){};
    virtual void endFit(){};

    virtual double getNewVal(EVENT_TYPE & evt){
      parametersChanged();
      return getVal(evt);
    }

    virtual ~PdfBase(){};
  };
  
}//namespace MINT
#endif
//
