// $Id$
#ifndef FTSORTINGFUNCTOR_H 
#define FTSORTINGFUNCTOR_H 1


 // from Event
#include "Event/MCFTDigit.h"


/** @FTSortingFunctor
 *  
 *
 *  @author Eric Cogneras
 *  @date   2012-05-22
 */

namespace LHCb {
 
  namespace FTSortingFunctor {
//  // functors
  template <class TYPE1, class TYPE2 = TYPE1 >
    class LessByChannel
      : public std::binary_function<TYPE1,TYPE2,bool>
    {
    public:
 
      inline bool operator() ( TYPE1 obj1 , TYPE2 obj2 ) const
      {
        return
          ( !obj1 ) ? true  :
          ( !obj2 ) ? false : obj1->channelID() < obj2->channelID() ;
      }
    };
  }
  
}

#endif // FTDATAFUNCTOR_H
