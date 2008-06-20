
#ifndef LESTER_SPACE_H
#define LESTER_SPACE_H

namespace Lester 
{

  template <typename ThePointType>
  class Space 
  {
  public:
    typedef ThePointType PointType;
    virtual bool contains(const PointType & point) const 
    { 
      return true; // Default implementation! 
    };
  };
  
};

#endif
