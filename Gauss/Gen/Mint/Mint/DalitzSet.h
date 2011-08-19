#ifndef DALITZ_SOMETHING_SET_HH
#define DALITZ_SOMETHING_SET_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:58 GMT

#include <map>
#include <string>
#include "Mint/DalitzEventPattern.h"
#include "Mint/DalitzCoordSet.h"
#include "Mint/AllPossibleSij.h"
#include "Mint/counted_ptr.h"
#include <map>

template<typename T>
class DalitzSet : public std::map< DalitzCoordSet, MINT::counted_ptr<T> >{
  std::string _name;
  void makeEmptyContainer(const DalitzEventPattern& pat){
    int nDaughters = pat.numDaughters();
    AllPossibleSij sijList(nDaughters);
    
    for(namedVMap::const_iterator it = sijList.begin();
	it != sijList.end(); it++){
      (*this)[it->second] = MINT::counted_ptr<T>(0);
    }
    
    return;
  }
  
  const std::string& makeName(){
    _name = "DalitzSetOfSomething";
    return _name;
  }
  
 public:
  DalitzSet(){ makeName();}
  DalitzSet(const DalitzSet<T>& other)
    :  std::map< DalitzCoordSet, MINT::counted_ptr<T> >( other)
    {
      makeName();
    }

  /*
  virtual void add(const DalitzCoordinate& coord
		   , const MINT::counted_ptr<T>& thing){
    if(0 == (*this)[coord]) (*this)[coord] = thing;
    else ((*this)[coord])->Add(*thing);
  }
  
  virtual void add(const DalitzSet<T>& hL){
    for(typename std::map< DalitzCoordSet, MINT::counted_ptr<T> >::const_iterator it = hL.begin();
	it != hL.end();
	it++){
      DalitzCoordinate c(it->first);
      this->add(c, it->second);
    }
    return;
  }
  */
  const std::string& name() const{return _name;}

  virtual ~DalitzSet(){}

};


#endif
//
