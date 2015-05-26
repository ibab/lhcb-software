#ifndef CACHED_BY_EVENT_HH
#define CACHED_BY_EVENT_HH

#include "Mint/FitParDependent.h"
#include "Mint/IDalitzEvent.h"
#include "Mint/DalitzEvent.h"

template <typename T>
class CachedByEvent : virtual public MINT::IFitParDependent{
  
 public:
  virtual T getNewVal(IDalitzEvent& evt)=0; // <<< user needs to implement this

 protected:
  mutable long int _rememberNumber; // for caching
  long int rememberNumber() const{
    if(_rememberNumber < 0){
      _rememberNumber = DalitzEvent::assignUniqueRememberNumber();
    }
    return _rememberNumber;
  }

  virtual T recalculate(IDalitzEvent& evt){
    T result(getNewVal(evt));
    evt.setValue(rememberNumber(), result);
    return result;
  }
  
 public:
  CachedByEvent() : _rememberNumber(-9999){}
  CachedByEvent(const CachedByEvent& other) : _rememberNumber(-9999) {}

  T getValWithCaching(IDalitzEvent& evt){
    if(changedSinceLastCall()) return recalculate(evt);
    T result;
    if(! evt.retrieveValue(rememberNumber(), result)) return recalculate(evt);
    return result;
  }

};

#endif
//
