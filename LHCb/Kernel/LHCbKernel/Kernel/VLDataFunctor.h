#ifndef VL_DATA_FUNCTOR_H
#define VL_DATA_FUNCTOR_H 1

#include <functional>
#include "Kernel/VLChannelID.h"

namespace VLDataFunctor {

  template<class TYPE1, class TYPE2 = TYPE1> 
  class LessByKey : public std::binary_function<TYPE1, TYPE2, bool> {
    public:
      /** compare the channel number of two objects
       *  @param obj1   first  object 
       *  @param obj2   second object
       *  @return  result of the comparison
       */
      bool operator()(TYPE1 obj1, TYPE2 obj2) const { 
        return (!obj1) ? true : (!obj2) ? false : obj1->key() < obj2->key(); 
      }
  };

  template<class TYPE1, class TYPE2 = TYPE1>
  class LessBySensor : public std::binary_function<TYPE1, TYPE2, bool>  {
    public:
      /** compare the sensor number of two objects
       *  @param obj1   first  object 
       *  @param obj2   second object
       *  @return  result of the comparison
       */
      bool operator()(TYPE1 obj1, TYPE2 obj2) const { 
        return (!obj1) ? true : (!obj2) ? false : obj1->sensor() < obj2->sensor(); 
      }
  };

  template<class TYPE1, class TYPE2 = TYPE1>
  class LessByStrip : public std::binary_function<TYPE1, TYPE2, bool>  {
    public:
      /** compare the sensor number and centroid strip number of two objects
       *  @param obj1   first  object 
       *  @param obj2   second object
       *  @return  result of the comparison
       */
      bool operator()(TYPE1 obj1, TYPE2 obj2) const { 
        return (!obj1) ? true : (!obj2) ? false : obj1->channelID().sensor() == obj2->channelID().sensor() ? 
          obj1->strip(0) < obj2->strip(0) : obj1->channelID().sensor() < obj2->channelID().sensor(); 
      }
  };

  template<class TYPE1, class TYPE2 = TYPE1>
  class LessByCharge : public std::binary_function<TYPE1, TYPE2, bool> {
    public:
      /** compare the deposited charge of two objects
       *  @param obj1   first  object 
       *  @param obj2   second object
       *  @return  result of the comparison
       */
      bool operator()(TYPE1 obj1, TYPE2 obj2) const { 
        return (!obj1) ? true : (!obj2) ? false : obj1->charge() < obj2->charge(); 
      }
  };

  template<class TYPE1, class TYPE2 = TYPE1>
  class LessByAdc : public std::binary_function<TYPE1, TYPE2, bool> {
    public:
      /** compare the adc values of two objects
       *  @param obj1   first  object 
       *  @param obj2   second object
       *  @return result of the comparison
     */
    bool operator()(TYPE1 obj1, TYPE2 obj2) const { 
      return (!obj1) ? true : (!obj2) ? false : obj1->adc() < obj2->adc();
    }
  };

  template<class TYPE>
  class key_eq : public std::unary_function<TYPE, bool> {
    LHCb::VLChannelID m_channel;
    public:
      explicit key_eq(const LHCb::VLChannelID& test) : m_channel(test) {}
      bool operator()(TYPE obj) const {
        return (obj->channelID()).key() == m_channel.channelID();
      }
  };

  template<class TYPE>
  class sensor_eq : public std::unary_function<TYPE, bool> {
    long m_sensor;
    public:
      explicit sensor_eq(const LHCb::VLChannelID& test) : m_sensor(test.sensor()) {}
      explicit sensor_eq(const int test) : m_sensor(test) {}
      bool operator()(TYPE obj) const {
        return obj->channelID().sensor() == m_sensor;
      }
  };

  template <class TYPE1, class TYPE2 = TYPE1>
  class LessByChannel : public std::binary_function<TYPE1, TYPE2, bool> {
    public:
      bool operator() (TYPE1 obj1, TYPE2 obj2) const {
      return obj1.channelID() < obj2.channelID();
    }
  };


}
#endif 
