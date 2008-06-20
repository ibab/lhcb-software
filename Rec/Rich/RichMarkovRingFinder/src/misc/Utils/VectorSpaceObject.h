
#ifndef LESTER_VECTORSPACEOBJECT_H
#define LESTER_VECTORSPACEOBJECT_H

// fwd dec
#include "VectorSpaceObject.fwd"

// includes
#include <iostream>

namespace Lester
{

  // declaration
  template<class TypeToBecomeVectorified>
  class VectorSpaceObject
  {

  public:

    // The bare minimum you must implement:
    virtual TypeToBecomeVectorified operator*(const double d) const = 0;
    virtual TypeToBecomeVectorified operator+(const TypeToBecomeVectorified & other) const = 0;

    // Default implementations of operators which *can* be derived from the above,
    // but which you would probably
    // do better to re-implement yourself in faster ways:
    virtual TypeToBecomeVectorified & operator*=(const double d) 
    {
      *this = ((*this)*d);
      return dynamic_cast<TypeToBecomeVectorified &>(*this);
    };
    virtual TypeToBecomeVectorified operator-(const TypeToBecomeVectorified & other) const 
    {
      return (*this)+(other*(-1.0));
    };
    virtual TypeToBecomeVectorified & operator+=(const TypeToBecomeVectorified & other) 
    {
      *this = ((*this) + other);
      return dynamic_cast<TypeToBecomeVectorified &>(*this);
    };
    virtual TypeToBecomeVectorified & operator-=(const TypeToBecomeVectorified & other) 
    {
      *this = ((*this) - other);
      return dynamic_cast<TypeToBecomeVectorified &>(*this);
    };

  };

}

#endif

