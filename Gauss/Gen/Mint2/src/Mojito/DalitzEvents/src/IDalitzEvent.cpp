// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:00 GMT
#include "Mint/IDalitzEvent.h"

bool EqualEvent(const IDalitzEvent* a, const IDalitzEvent* b){

  if(0 == a && 0 == b) return true;
  if(0 == a && 0 != b) return false;
  if(0 != a && 0 == b) return false;

  if( a->eventPattern() != b->eventPattern()) return false;

  for(unsigned int i=0; i< a->eventPattern().size(); i++){
    if(a->p(i) != b->p(i)) return false;
  }

  return true;
}

std::ostream& operator<<(std::ostream& os, const IDalitzEvent& de){
  de.print(os);
  return os;
}
//
