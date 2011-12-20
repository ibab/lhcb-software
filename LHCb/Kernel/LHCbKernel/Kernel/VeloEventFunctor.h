#ifndef _VeloEventFunctor_H_
#define _VeloEventFunctor_H_

#include <functional>
#include "Kernel/VeloChannelID.h"

//
// This File contains the declaration of VeloEventFunctor namespace 
// functions for sorting elements of Velo Event classes and 
// other utility functions
//   Author: Chris Parkes
//   Created: 25/05/02 

namespace VeloEventFunctor {

// functors 

template <class TYPE1, class TYPE2 = TYPE1 >
  class Less_by_key
    : public std::binary_function<TYPE1,TYPE2,bool> 
  {
  public:

    /** compare the channel of one object with the 
     *  channel of another object
     *  @param obj1   first  object 
     *  @param obj2   second object
     *  @return  result of the comparision
     */
    inline bool operator() ( TYPE1 obj1 , TYPE2 obj2 ) const 
    { 
      return 
        ( !obj1 ) ? true  : 
        ( !obj2 ) ? false : obj1->key() < obj2->key() ; 
    }
    ///
  };

template <class TYPE1, class TYPE2 = TYPE1 >
  class Less_by_sensor
    : public std::binary_function<TYPE1,TYPE2,bool> 
  {
  public:

    /** compare the channel of one object with the 
     *  channel of another object
     *  @param obj1   first  object 
     *  @param obj2   second object
     *  @return  result of the comparision
     */
    inline bool operator() ( TYPE1 obj1 , TYPE2 obj2 ) const 
    { 
      return 
        ( !obj1 ) ? true  : 
        ( !obj2 ) ? false : obj1->sensor() < obj2->sensor() ; 
    }
    ///
  };

template <class TYPE1, class TYPE2 = TYPE1 >
  class Less_by_charge
    : public std::binary_function<TYPE1,TYPE2,bool> 
  {
  public:

    /** compare the dep charge of one object with the 
     *  adc Value  of another object
     *  @param obj1   first  object 
     *  @param obj2   second object
     *  @return  result of the comparision
     */
    inline bool operator() ( TYPE1 obj1 , TYPE2 obj2 ) const 
    { 
      return 
        ( !obj1 ) ? true  : 
        ( !obj2 ) ? false : obj1->charge() < obj2->charge() ; 
    }
    ///
  };

template <class TYPE1, class TYPE2 = TYPE1 >
  class Less_by_adcValue
    : public std::binary_function<TYPE1,TYPE2,bool> 
  {
  public:

    /** compare the adc value of one object with the 
     *  adc Value  of another object
     *  @param obj1   first  object 
     *  @param obj2   second object
     *  @return  result of the comparision
     */
    inline bool operator() ( TYPE1 obj1 , TYPE2 obj2 ) const 
    { 
      return
      ( !obj1 )?true: 
      (!obj2)?false:obj1->adcValue()<obj2->adcValue();
    }
    ///
  };

template <class TYPE>
class key_eq: public std::unary_function<TYPE,bool>{

   /** compare the key of an object 
    */

   LHCb::VeloChannelID aChan;
public:
  explicit key_eq(const LHCb::VeloChannelID& testChan) : aChan(testChan){}
  inline bool operator() (TYPE obj) const{
  return (obj->channelID()).key()==aChan.channelID();}
};

template <class TYPE>
class sensor_eq: public std::unary_function<TYPE,bool>{

   /** compare the sensor number of an object
    */

   long m_sensor;
public:
  explicit sensor_eq(const LHCb::VeloChannelID& testChan) :
    m_sensor(testChan.sensor())
   { }
  explicit sensor_eq(const int testChan) : 
    m_sensor(testChan)
   { }
  inline bool operator() (TYPE obj) const{
  return obj->channelID().sensor()==m_sensor;}
};

template <class TYPE>
class compBySensor_LB:
public std::binary_function<const TYPE, const LHCb::VeloChannelID, bool>{

   /** compare the sensor number of an object for lower bound
    */
public:
  inline bool operator() (const TYPE& obj, 
                          const LHCb::VeloChannelID& testID) const{
    return ((!obj) ? false : testID.sensor() >obj->channelID().sensor());
  }
};

template <class TYPE>
class compBySensor_UB:
public std::binary_function<const LHCb::VeloChannelID,const TYPE ,bool>{

  /** compare the sensor number of an object for upper bound
    */

public:
  inline bool operator() (const LHCb::VeloChannelID& testID, 
                          const TYPE& obj) const{
    return ((!obj) ? false : testID.sensor() >obj->channelID().sensor());
  }
};

}
#endif // _VeloEventFunctor_H_
