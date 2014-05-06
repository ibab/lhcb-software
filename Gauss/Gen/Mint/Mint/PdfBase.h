#ifndef PDF_BASE_HH
#define PDF_BASE_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:56 GMT

#include "Mint/IPdf.h"
#include "Mint/EventAccess.h"
#include "Mint/IEventList.h"
#include "Mint/IReturnReal.h"

namespace MINT{

  template<typename EVENTS>
    class PdfBase : virtual public IPdf<EVENTS>, public EventAccess<EVENTS>{
  public:
  PdfBase(IEventList<EVENTS>* erptr = 0)
    : EventAccess<EVENTS>(erptr)
      {}
  PdfBase(IEventAccess<EVENTS>* erptr)
    : EventAccess<EVENTS>(erptr)
      {}
  PdfBase(const PdfBase& other)
    :  IBasicEventAccess<EVENTS>()
      , IEventAccess<EVENTS>()
      , IReturnReal()
      , IGetRealEvent<EVENTS>()
      , IPdf<EVENTS>()
      , EventAccess<EVENTS>(other)
      {}
 
   IPdf<EVENTS>* Clone() const{
     return new PdfBase(*this);     
   } 
    
    virtual double getVal()=0;
    virtual double RealVal(){
      return getVal();
    }
    
    virtual void beginFit(){};
    virtual void parametersChanged(){};
    virtual void endFit(){};

    virtual double getNewVal(){
      parametersChanged();
      return getVal();
    }
    
    virtual ~PdfBase(){};
  };
  
}//namespace MINT
#endif
//
