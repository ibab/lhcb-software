// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:12 GMT
#include "EventPattern.h"

bool EventPattern::equal(const EventPattern& other) const{
  if(size() != other.size()) return false;
  for(unsigned int i=0; i<size(); i++){
    if((*this)[i] != other[i]) return false;
  }
  return true;
}
bool EventPattern::operator==(const EventPattern& rhs) const{
  return equal(rhs);
}

//
