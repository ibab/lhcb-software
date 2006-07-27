
#ifndef LESTER_VECTORSPACEOBJECT_H
#define LESTER_VECTORSPACEOBJECT_H

// fwd dec
#include "VectorSpaceObject.fwd" 

// includes
#include <iostream>

// declaration
template<class TypeToBecomeVectorified>
class VectorSpaceObject {
// VectorSpaceObject() {};
// VectorSpaceObject(const VectorSpaceObject & other) {};
// VectorSpaceObject & operator=(const VectorSpaceObject & other) { return *this; };
// virtual ~VectorSpaceObject() {};

public:

  // The bare minimum you must implement:
  virtual TypeToBecomeVectorified operator*(const double d) const = 0;
  virtual TypeToBecomeVectorified operator+(const TypeToBecomeVectorified & other) const = 0;

  // Default implementations of operators which *can* be derived from the above,
  // but which you would probably
  // do better to re-implement yourself in faster ways:
  virtual TypeToBecomeVectorified & operator*=(const double d) {
    *this = ((*this)*d);
    return dynamic_cast<TypeToBecomeVectorified &>(*this);
  };
  virtual TypeToBecomeVectorified operator-(const TypeToBecomeVectorified & other) const {
    return (*this)+(other*(-1.0));
  };
  virtual TypeToBecomeVectorified & operator+=(const TypeToBecomeVectorified & other) {
    *this = ((*this) + other);
    return dynamic_cast<TypeToBecomeVectorified &>(*this);
  };
  virtual TypeToBecomeVectorified & operator-=(const TypeToBecomeVectorified & other) {
    *this = ((*this) - other);
    return dynamic_cast<TypeToBecomeVectorified &>(*this);
  };
  // As a mathmo I can't allow the following one to be included! ;)
  //virtual TypeToBecomeVectorified & operator/=(const double d) {
  //  *this = ((*this)*(1.0/d));
  //  return *this;
  //};
  
};

#endif

