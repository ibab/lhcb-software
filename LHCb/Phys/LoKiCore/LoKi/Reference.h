// $Id$
// ============================================================================
#ifndef LOKI_REFERENCE_H 
#define LOKI_REFERENCE_H 1 
// ============================================================================
// Include files
// ============================================================================
// LoKi 
// ============================================================================
#include "LoKi/Assert.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  template <class TYPE> class ConstReference ;
  // ==========================================================================
  /** @class Reference Reference.h LoKi/Reference.h
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2007-06-08
   */
  template <class TYPE> 
  class Reference 
  {
  public: 
    // ========================================================================
    /// Standard constructor
    Reference( TYPE& data ) : m_data( &data ) {} 
    /// destructor
    ~Reference(){}
    /// just to please the compiler: NO USE IS ALLOWED 
    Reference() : m_data ( 0 ) {}
    // ========================================================================
  public:
    // ========================================================================
    /// get the content: conversion to the reference 
    inline          TYPE& get () const 
    {
      // check the validity of the data:
      LoKi::Assert ( 0 != m_data , "LoKi::Reference::get(): Invalid pointer!" ) ; 
      return *m_data ; 
    } 
    /// THE MAIN METHOD: conversion to reference  
    inline operator TYPE&     () const { return   get() ; }
    /// dereferencing: conversion  to reference 
    inline TYPE& operator*    () const { return   get() ; }
    // ========================================================================
  public:
    // ========================================================================
    /// get the data, technical method, please do not use it!!!
    TYPE* i_data() const { return m_data ; }
    // ========================================================================
  public:
    // ========================================================================
    /// assignement from the value 
    Reference& operator=( TYPE&            value ) 
    { m_data = &value        ; return *this ; } 
    /// assignement from the value 
    Reference& operator=( const Reference& value ) 
    { m_data =  value.m_data ; return *this ; } 
    // ========================================================================
  public:
    // ========================================================================
    /// comparison
    bool operator==( const Reference& right ) const 
    { return &right == this || right.m_data == m_data ; }
    /// comparison
    bool operator==( const ConstReference<TYPE>& right ) const ;
    /// comparison
    bool operator==( const TYPE& right ) const { return &right == m_data ; }
    // ========================================================================
  private:
    // ========================================================================
    /// the underlying pointer 
    TYPE* m_data ;                                    // the underlying pointer 
    // ========================================================================
  };
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
#include "LoKi/ConstReference.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /// comparison
  template <class TYPE>
  bool Reference<TYPE>::operator==( const ConstReference<TYPE>& right ) const 
  { return right.i_data() == m_data ; }
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_REFERENCE_H
// ============================================================================
