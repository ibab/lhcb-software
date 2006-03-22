// $Id: OTDataFunctor.h,v 1.1 2006-03-22 16:59:59 cattanem Exp $
#ifndef OTSIMULATION_OTDATAFUNCTOR_H
#define OTSIMULATION_OTDATAFUNCTOR_H 1

#include <functional>
#include "Kernel/OTChannelID.h"

/** namespace OTDataFunctor
 *
 * This file contains the declaration of OTDataFunctor namespace. 
 * It contains functors used for sorting.
 *
 *  @author  M. Needham
 *  @date    May 2002
 */

namespace OTDataFunctor {

// functors 
template <class TYPE1, class TYPE2 = TYPE1 >
  class Less_by_Channel
    : public std::binary_function<TYPE1,TYPE2,bool> 
  {
  public:

    /** compare the channel of one object wOTh the 
     *  channel of another object
     *  @param obj1   first  object 
     *  @param obj2   second object
     *  @return  result of the comparision
     */
    inline bool operator() ( TYPE1 obj1 , TYPE2 obj2 ) const 
    { 
      return 
        ( !obj1 ) ? true  : 
        ( !obj2 ) ? false : obj1->channel() < obj2->channel() ; 
    }
    ///
  };

template <class TYPE1, class TYPE2 = TYPE1 >
  class Less_by_Key
    : public std::binary_function<TYPE1,TYPE2,bool> 
  {
  public:

    /** compare the channel of one object wOTh the 
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
  class Less_by_tdcTime
    : public std::binary_function<TYPE1,TYPE2,bool> 
  {
  public:

    /** compare the dep charge of one object wOTh the 
     *  dep Charge  of another object
     *  @param obj1   first  object 
     *  @param obj2   second object
     *  @return  result of the comparision
     */
    inline bool operator() ( TYPE1 obj1 , TYPE2 obj2 ) const 
    { 
      return 
        ( !obj1 ) ? true  : 
        ( !obj2 ) ? false : obj1->tdcTime() < obj2->tdcTime() ; 
    }
    ///
  };


template <class TYPE1, class TYPE2 = TYPE1 >
  class Less_by_ChannelAndTime
    : public std::binary_function<TYPE1,TYPE2,bool> 
  {
  public:

    /** compare the dep charge of one object wOTh the 
     *  dep Charge  of another object
     *  @param obj1   first  object 
     *  @param obj2   second object
     *  @return  result of the comparision
     */
    inline bool operator() ( TYPE1 obj1 , TYPE2 obj2 ) const 
    { 
      if ( obj1->channel() != obj2->channel() ) {
        return 
          ( !obj1 ) ? true  : 
          ( !obj2 ) ? false : obj1->channel() < obj2->channel() ;
      } else {
        return 
          ( !obj1 ) ? true  : 
          ( !obj2 ) ? false : obj1->tdcTime() < obj2->tdcTime() ;
      }

    }
    ///
  };


template <class TYPE>
class station_eq: public std::unary_function<TYPE,bool>{
   LHCb::OTChannelID aChan;
public:
  explicit station_eq(const LHCb::OTChannelID& testChan) : aChan(testChan){}
  inline bool operator() (TYPE obj) const{
  return obj->channel().station()==aChan.station();}
};

template <class TYPE>
class layer_eq: public std::unary_function<TYPE,bool>{
   LHCb::OTChannelID aChan;
public:
  explicit layer_eq(const LHCb::OTChannelID& testChan) : aChan(testChan){}
  inline bool operator() (TYPE obj) const{
  return obj->channel().uniqueLayer()==aChan.uniqueLayer();}
};

template <class TYPE>
class module_eq: public std::unary_function<TYPE,bool>{
   LHCb::OTChannelID aChan;
public:
  explicit module_eq(const LHCb::OTChannelID& testChan) : aChan(testChan){}
  inline bool operator() (TYPE obj) const{
  return obj->channelID().uniqueModule()==aChan.uniqueModule();}
};

template <class TYPE>
class compByStation_LB: public std::binary_function<const TYPE,
  const LHCb::OTChannelID, bool>{
public:
  inline bool operator() (const TYPE& obj, const LHCb::OTChannelID& testID) const{
    return ((!obj) ? false : testID.station() >obj->channel().station());
  }
};

template <class TYPE>
class compByStation_UB: public std::binary_function<const LHCb::OTChannelID,
  const TYPE ,bool>{
public:
  inline bool operator() (const LHCb::OTChannelID& testID, const TYPE& obj) const{
    return ((!obj) ? false : testID.station() >obj->channel().station());
  }
};

template <class TYPE>
class compByLayer_LB: public std::binary_function<const TYPE, 
  const LHCb::OTChannelID, bool>{
   LHCb::OTChannelID testID;
public:
  inline bool operator() (const TYPE& obj,const LHCb::OTChannelID& testID) const{
    return 
      ((!obj) ? false : testID.uniqueLayer() >obj->channel().uniqueLayer());
  }
};

template <class TYPE>
class compByLayer_UB: public std::binary_function<const LHCb::OTChannelID,
  const TYPE ,bool>{
   LHCb::OTChannelID testID;
public:
  inline bool operator()(const LHCb::OTChannelID& testID, const TYPE& obj) const{
    return 
      ((!obj) ? false : testID.uniqueLayer() >obj->channel().uniqueLayer());
  }
};

template <class TYPE>
class compByModule_LB: public std::binary_function<const TYPE, 
  const LHCb::OTChannelID, bool>{
   LHCb::OTChannelID testID;
public:
  inline bool operator() (const TYPE& obj,const LHCb::OTChannelID& testID) const{
    return 
      ((!obj) ? false : testID.uniqueModule() >obj->channel().uniqueModule());
  }
};

template <class TYPE>
class compBModule_UB: public std::binary_function<const LHCb::OTChannelID, 
  const TYPE ,bool>{
   LHCb::OTChannelID testID;
public:
  inline bool operator() (const LHCb::OTChannelID& testID, const TYPE& obj) const{
    return 
      ((!obj) ? false : testID.uniqueModule() >obj->channel().uniqueModule());
  }
};

};


#endif // OTSIMULATION_OTDATAFUNCTOR_H












