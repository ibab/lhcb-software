#ifndef _VPDataFunctor_H_
#define _VPDataFunctor_H_
#include <functional>
#include <numeric>
#include "Kernel/VPChannelID.h"

// Functors used in VP classes
//
//   Author: M. Kucharczyk
//   Created: 12.10.2009

namespace VPDataFunctor {

template <class TYPE1, class TYPE2 = TYPE1>
class Less_by_Channel : public std::binary_function<TYPE1, TYPE2, bool> {
public:
  /** Compare the channel of one object with the channel of another object
   *  @param obj1   first object
   *  @param obj2   second object
   *  @return  result of the comparison
   */
  inline bool operator()(TYPE1 obj1, TYPE2 obj2) const {
    return (!obj1) ? true : (!obj2) ? false
                                    : obj1->channelID() < obj2->channelID();
  }
};

template <class TYPE1, class TYPE2 = TYPE1>
class Less_by_Key : public std::binary_function<TYPE1, TYPE2, bool> {
public:
  /** Compare the key of one object with the key of another object
   *  @param obj1   first object
   *  @param obj2   second object
   *  @return  result of the comparison
   */
  inline bool operator()(TYPE1 obj1, TYPE2 obj2) const {
    return (!obj1) ? true : (!obj2) ? false : obj1->key() < obj2->key();
  }
};

}

#endif // _VPDataFunctor_H
