
#ifndef LESTER_INDEX_H
#define LESTER_INDEX_H

// fwd dec
#include "GenericRingFinder/Index.fwd" 

// includes
#include <iostream>

namespace GenRingF {
  
  // declaration
  template<class TYPE,class Dummy>
  struct Index {
  private:
    Index(); // don't want people to use this!
  public:
    Index(const TYPE ii) : m_i(ii) {};
    inline TYPE value() const { return m_i; }
    bool operator< (const Index<TYPE,Dummy> & other) const {
      return value() < other.value();
    }
    virtual void printMeTo(std::ostream & os) const { os << value(); }
  private:
    TYPE m_i;
  };
  
}

template<class TYPE,class Dummy>
std::ostream & operator<< (std::ostream & os, const GenRingF::Index<TYPE,Dummy> & li) {
  li.printMeTo(os);
  return os;
}

#endif

