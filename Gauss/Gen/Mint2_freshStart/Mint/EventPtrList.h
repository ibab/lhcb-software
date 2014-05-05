#ifndef EVENT_PTR_LIST_HH
#define EVENT_PTR_LIST_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:54 GMT

#include <iostream>
#include <vector>
#include <ctime>
#include <cmath>

#include "TRandom.h"

#include "Mint/IEventList.h"
//#include "Mint/EventAccess.h"
#include "Mint/IReturnReal.h"
#include "Mint/IReturnRealForEvent.h"

#include "Mint/GeneralisedPareto.h"

#include "Mint/counted_ptr.h"

namespace MINT{

template<typename EVENT_TYPE>
class EventPtrList 
  : virtual public IEventList<EVENT_TYPE>
    , public std::vector<MINT::counted_ptr<EVENT_TYPE> > 
    {
  
  public:
  EventPtrList()
  {
  }

  EventPtrList(const EVENT_TYPE& evt)
  {
    Add(evt);
  }

  counted_ptr<EVENT_TYPE> getPtr(unsigned int i) const{
    return std::vector<MINT::counted_ptr<EVENT_TYPE> >::operator[](i);
  }

  EVENT_TYPE& operator[](unsigned int i){
    // a bit awkward, but makes it compatible with
    // a MINT::EventList<EVENT_TYPE>
    
    return *(getPtr(i));
  }

  const EVENT_TYPE& operator[](unsigned int i) const{
    return *(getPtr(i));
  }

  EVENT_TYPE getEvent(unsigned int i) const{
    return *(getPtr(i));
  }
  EventPtrList(const EventPtrList<EVENT_TYPE>& other)
    : IEventList<EVENT_TYPE>()
    , std::vector<MINT::counted_ptr<EVENT_TYPE> >(
    (std::vector<MINT::counted_ptr<EVENT_TYPE> >)other)
  {
  }
  EventPtrList<EVENT_TYPE>& 
  operator=(const EventPtrList<EVENT_TYPE>& other){
    if(this == &other) return *this;
    this->clear();
    for(unsigned int i=0; i<other.size(); i++){
      this->push_back(other.getPtr(i));
    }
    /*
    this->_recordCollection = other._recordCollection;
    this->_recordCollection.pop_front();
    this->_recordCollection.push_front((IEventList< RETURN_TYPE >*) this);
    */
    return *this;
  }

  virtual unsigned int size() const{
    return std::vector<counted_ptr<EVENT_TYPE> >::size();
  }

  virtual MINT::counted_ptr<EVENT_TYPE> popLastEventPtr(){
    if(this->empty()) return MINT::counted_ptr<EVENT_TYPE>(0);
    MINT::counted_ptr<EVENT_TYPE> evtPtr(getPtr(this->size()-1));
    this->resize(this->size() - 1);
    return evtPtr;
  }

 
  virtual bool Add(const EVENT_TYPE& evt){
    counted_ptr<EVENT_TYPE> copy (new EVENT_TYPE (evt));
    this->push_back(copy);
    return true;
  }

 
  virtual bool Add(const MINT::counted_ptr<EVENT_TYPE>& evt){
    this->push_back(evt);
    return true;
  }

  virtual bool Add(const EventPtrList<EVENT_TYPE>& otherList){
    if(otherList.empty()) return false;
    for(unsigned int i=0; i < otherList.size(); i++){
      this->push_back(otherList.getPtr(i));
    }
    return true;
  }

  double getMax(IReturnReal* amps){
    double max=-1;
    int counter=0;
    for(unsigned int i=0; i < this->size(); i++){
      double d=amps->RealVal((*this)[i]);
      if(d > max || 0 == counter) max=d;
      counter++;
    };
    return max;
  }
 
  double getMin(IReturnReal* amps){
    double min = +9999;
    int counter=0;
    for(unsigned int i=0; i < this->size(); i++){
      double d=amps->RealVal((*this)[i]);
      if(d < min || 0 == counter) min=d;
      counter++;
    }
    return min;
  }
};

}//namespace MINT

#endif
//
