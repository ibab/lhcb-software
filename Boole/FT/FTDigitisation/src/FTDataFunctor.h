// $Id$
#ifndef FTDATAFUNCTOR_H 
#define FTDATAFUNCTOR_H 1


 // from Event
#include "Event/MCFTDigit.h"


/** @class FTDataFunctor FTDataFunctor.h
 *  
 *
 *  @author Eric Cogneras
 *  @date   2012-05-22
 */

namespace FTDataFunctor {
 
//  // functors
  template <class TYPE1, class TYPE2 = TYPE1 >
    class Less_by_Channel
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


struct less_Chan : public std::binary_function<LHCb::MCFTDigit*, LHCb::MCFTDigit*, bool>{
 bool operator()(LHCb::MCFTDigit* x, LHCb::MCFTDigit* y)const {return x->channelID() < y->channelID();}
};
#endif // FTDATAFUNCTOR_H
