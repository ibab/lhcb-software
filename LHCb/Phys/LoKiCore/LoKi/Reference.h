// $Id: Reference.h,v 1.2 2007-06-27 14:09:39 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_REFERENCE_H 
#define LOKI_REFERENCE_H 1 
// ============================================================================
// Include files
// ============================================================================
// LoKi 
// ============================================================================
#include "LoKi/Exception.h"
// ============================================================================
namespace LoKi 
{
  /** @class Reference Reference.h LoKi/Reference.h
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2007-06-08
   */
  template <class TYPE> 
  class Reference 
  {
  public: 
    /// Standard constructor
    Reference( TYPE& data ) : m_data( &data ) {} ;
    /// destructor
    ~Reference(){};
    /// just to please the compiler: NO USE IS ALLOWED 
    Reference() : m_data ( 0 ) {}
  public:
    /// get the content: conversion to the reference 
    inline          TYPE& get () const 
    {
      // check the validity of the data:
      if ( 0 == m_data ) { throw LoKi::Exception ( "LoKi::Reference::get(): Invalid pointer!" ) ; }
      return *m_data ; 
    } ;
    /// THE MAIN METHOD: conversion to reference  
    inline operator TYPE&     () const { return   get() ; }
    /// dereferencing: conversion  to reference 
    inline TYPE& operator*    () const { return   get() ; }
  public:
    /// assignement from the value 
    inline Reference& operator=( const TYPE& value ) 
    { m_data = const_cast<TYPE*>( &value ) ; return *this ; } ;
  public:
    /// comparison
    bool operator==( const Reference& right ) const 
    { 
      return &right == this || right.m_data == m_data ;
    }
    /// comparison
    bool operator==( const TYPE& right ) const 
    { 
      return &right == m_data ;
    }
  private:
    // the underlying pointer 
    TYPE* m_data ; ///< the underlying pointer 
  };
} //end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_REFERENCE_H
// ============================================================================
