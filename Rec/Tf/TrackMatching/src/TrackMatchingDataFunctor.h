// $Id: TrackMatchingDataFunctor.h,v 1.1.1.1 2007-10-09 18:11:39 smenzeme Exp $
#ifndef TRACKMATCHING_TRACKMATCHINGDATAFUNCTOR_H
#define TRACKMATCHING_TRACKMATCHINGDATAFUNCTOR_H 1

#include <functional>

#include "Kernel/STChannelID.h"

/** namespace TrackMatchingDataFunctor
 *
 * This file contains the declaration of TrackMatchingDataFunctor 
 * namespace.  It contains a functor used for sorting.
 *
 *  @author  Jeroen van Tilburg jtilburg@nikhef.nl
 *  @date    24-06-2002
 */

namespace TrackMatchingDataFunctor {

  template <class TYPE1, class TYPE2 = TYPE1 >
  class Less_by_Chi2
    : public std::binary_function<TYPE1,TYPE2,bool> 
  {
  public:
    
    /** compare the chi2 of one object with the 
     *  channel of another object
     *  @param obj1   first  object 
     *  @param obj2   second object
     *  @return  result of the comparision
     */
    inline bool operator() ( TYPE1 obj1 , TYPE2 obj2 ) const
    {
      return
        ( !obj1 ) ? true  : 
        ( !obj2 ) ? false : obj1->chi2() < obj2->chi2() ;
    } 
    ///
  };

  template <class TYPE>
  class compByLayer_LB
    : public std::binary_function<const TYPE, const LHCb::STChannelID, bool>
  {
    LHCb::STChannelID testID;
  public:
    inline bool operator() ( const TYPE& obj,
                             const LHCb::STChannelID& testID ) const {
      return ((!obj) ? false : 
              testID.uniqueLayer() > obj->channelID().uniqueLayer());
    }
  };
  
  template <class TYPE>
  class compBySector_LB
    : public std::binary_function<const TYPE, const LHCb::STChannelID, bool>
  {
    LHCb::STChannelID testID;
  public:
    inline bool operator() ( const TYPE& obj,
                             const LHCb::STChannelID& testID ) const {
      return ((!obj) ? false : 
              testID.uniqueSector() > obj->channelID().uniqueSector());
    }
  };
  
}


#endif // TRACKMATCHING_TRACKMATCHINGDATAFUNCTOR_H
