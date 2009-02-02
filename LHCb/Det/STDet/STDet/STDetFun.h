// $Id: STDetFun.h,v 1.2 2009-02-02 15:36:48 mneedham Exp $
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


  /// equal by nickname
  template <class TYPE>
  class equal_by_name: public std::unary_function<TYPE, bool>{
    std::string name;
  public:
    explicit equal_by_name(const std::string& testname): name(testname){}
    inline bool operator() (TYPE obj) const{
      return obj->nickname() == name;
    }  
  };

}

#endif // _STDataFunctor_H_
