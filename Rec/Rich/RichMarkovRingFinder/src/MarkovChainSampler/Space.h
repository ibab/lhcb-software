
#ifndef RICHMARKOV_SPACE_H
#define RICHMARKOV_SPACE_H

namespace RichMarkov {

  template <typename ThePointType>
  class Space {
  public:
    typedef ThePointType PointType;
    virtual bool contains(const PointType & point) const { 
      return true; // Default implementation! 
    };
  };
  
}

#endif
