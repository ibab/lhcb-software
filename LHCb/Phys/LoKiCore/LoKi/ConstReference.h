// $Id: ConstReference.h,v 1.1 2009-06-28 13:26:06 ibelyaev Exp $
#ifndef LOKI_CONSTREFERENCE_H 
#define LOKI_CONSTREFERENCE_H 1
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
  template <class TYPE> class Reference ;
  // ==========================================================================
  /** @class ConstReference LoKi/ConstReference.h
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date   2009-06-26
   */
  template <class TYPE> 
  class ConstReference 
  {
  public: 
    // ========================================================================
    /// Standard constructor
    ConstReference( const TYPE& data ) : m_data( &data ) {} ;
    /// constructor from reference 
    ConstReference( const Reference<TYPE>& data ) ;
    /// destructor
    ~ConstReference(){};
    /// just to please the compiler: NO USE IS ALLOWED 
    ConstReference() : m_data ( 0 ) {}
    // ========================================================================
  public:
    // ========================================================================
    /// get the content: conversion to the reference 
    inline const TYPE& get () const 
    {
      // check the validity of the data:
      LoKi::Assert ( 0 != m_data , "LoKi::Reference::get(): Invalid pointer!" ) ; 
      return *m_data ; 
    } 
    /// THE MAIN METHOD: conversion to reference  
    inline operator const TYPE& () const { return   get() ; }
    /// dereferencing: conversion  to reference 
    inline const TYPE& operator*() const { return   get() ; }
    // ========================================================================    
  public:
    // ========================================================================
    /// get the data, technical method, please do not use it!!!
    const TYPE* i_data() const { return m_data ; }
    // ========================================================================    
  public:
    // ========================================================================
    /// assignement from the value 
    inline ConstReference& operator=( const TYPE& value ) 
    { m_data = &value        ; return *this ; } ;
    /// assignement form the reference 
    inline ConstReference& operator=( const ConstReference& value ) 
    { m_data =  value.m_data ; return *this ; } ;
    /// assignement from the reference 
    inline ConstReference& operator=( const Reference<TYPE>& value ) ;
    // ========================================================================
  public:
    // ========================================================================
    /// comparison
    bool operator==( const ConstReference& right ) const 
    { return &right == this || right.m_data == m_data ; }
    /// comparison
    bool operator==( const Reference<TYPE>& right ) const ;
    /// comparison
    bool operator==( const TYPE& right ) const { return &right == m_data ; }
    // ========================================================================
  private:
    // ========================================================================
    /// the underlying pointer 
    const TYPE* m_data ;                              // the underlying pointer 
    // ========================================================================
  };
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
#include "LoKi/Reference.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /// constructor from reference 
  template <class TYPE>
  ConstReference<TYPE>::ConstReference ( const Reference<TYPE>& data ) 
    : m_data( data.i_data () ) 
  {} 
  // ==========================================================================
  /// comparison
  template <class TYPE>
  bool ConstReference<TYPE>::operator==( const Reference<TYPE>& right ) const 
  { return right.i_data() == m_data ; }
  // ==========================================================================
  /// assignement from the value 
  template <class TYPE>
  ConstReference<TYPE>& 
  ConstReference<TYPE>::operator=( const Reference<TYPE>& value ) 
  { m_data =  value.i_data() ; return *this ; } ;
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_CONSTREFERENCE_H
// ============================================================================
