#ifndef _VPDataFunctor_H_
#define _VPDataFunctor_H_
#include <functional>
#include <numeric>
#include "Kernel/VPChannelID.h"

// This File contains the declaration of VPDataFunctor namespace
// C++ code for 'LHCb Tracking package(s)'
//
//   Author: M. Kucharczyk
//   Created: 12.10.2009

namespace VPDataFunctor {


// Functors
template <class TYPE1, class TYPE2 = TYPE1>
  class Less_by_Channel
    : public std::binary_function<TYPE1,TYPE2,bool>
  {
  public:
    /** Compare the channel of one object with the
     *  channel of another object
     *  @param obj1   first  object
     *  @param obj2   second object
     *  @return  result of the comparision
     */
    inline bool operator() (TYPE1 obj1, TYPE2 obj2) const
    {
      return
        (!obj1) ? true  :
        (!obj2) ? false : obj1->channelID() < obj2->channelID();
    }
  };


template <class TYPE1, class TYPE2 = TYPE1>
  class Less_by_Key
    : public std::binary_function<TYPE1,TYPE2,bool>
  {
  public:
    /** Compare the channel of one object with the
     *  channel of another object
     *  @param obj1   first  object
     *  @param obj2   second object
     *  @return  result of the comparision
     */
    inline bool operator() (TYPE1 obj1, TYPE2 obj2) const
    {
      return
        (!obj1) ? true  :
        (!obj2) ? false : obj1->key() < obj2->key();
    }
  };


 template <class TYPE1, class TYPE2 = TYPE1 >
  class Greater_by_totValue
   : public std::binary_function<TYPE1,TYPE2,bool>
   {
     public:
     /** Compare the adc value of one object with the
      *  adc Value  of another object
      *  @param obj1   first  object
      *  @param obj2   second object
      *  @return  result of the comparision
      */
     inline bool operator() (TYPE1 obj1, TYPE2 obj2) const
     {
      return
      (!obj1) ? true  :
      (!obj2) ? false : obj1->ToTValue() > obj2->ToTValue();
     }
   };


// Class for accumulating energy
template <class TYPE>
  class Accumulate_Charge
  : public std::binary_function<double,TYPE,double> {
  public:
  inline double operator() (double& charge, TYPE obj) const {
    return (!obj) ? charge : charge+= obj->depositedCharge(); };

};
}

#endif // _VPDataFunctor_H
