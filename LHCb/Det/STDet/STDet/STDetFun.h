// $Id: STDetFun.h,v 1.1 2008-05-06 09:58:26 mneedham Exp $
#ifndef _STDetFun_H_
#define _STDetFun_H_

#include <functional>
#include <numeric>
#include "STDet/DeSTBaseElement.h"

namespace STDetFun {

// functors 
  class SortByY
    : public std::binary_function<DeSTBaseElement*,DeSTBaseElement*,bool> 
  {
  public:

    inline bool operator() (DeSTBaseElement* obj1 , DeSTBaseElement* obj2 ) const 
    { 
      return 
        ( !obj1 ) ? true  : 
        ( !obj2 ) ? false : obj1->globalCentre().y() < obj2->globalCentre().y() ; 
    }
    ///
  };
}

#endif // _STDataFunctor_H_
