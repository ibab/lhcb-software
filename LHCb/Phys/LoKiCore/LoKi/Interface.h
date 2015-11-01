// ============================================================================
#ifndef LOKI_INTERFACE_H
#define LOKI_INTERFACE_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKiCore
// ============================================================================
#include "LoKi/AddRef.h"
#include "LoKi/Release.h"
// ============================================================================
/** @file
 *
 *  This file is a part of LoKi project -
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas,
 *  contributions and advices from G.Raven, J.van Tilburg,
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-01-23
 */
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  /** @class Interface Interface.h LoKi/Interface.h
   *
   *  Helper class for manipulations with interfaces
   *  It looks like reduced smart *INTRUSIVE* pointer
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-03-10
   */
  template <class TYPE>
  class Interface
  {
  public:
    // ========================================================================
    /** Standard/default constructor from the interface
     *  @param obj object to be used
     */
    Interface ( const TYPE* obj = 0 )
      : m_object ( const_cast<TYPE*>( obj )  ) { LoKi::addRef ( m_object ) ; }
    /// copy constructor
    Interface ( const Interface<TYPE>& right )
      : m_object( right.m_object )             { LoKi::addRef ( m_object ) ; }
    /// templated "copy" constructor
    template <class OTHER>
    Interface ( const Interface<OTHER>& right )
      : m_object ( right.m_object )            { LoKi::addRef ( m_object ) ; }
    /// virtual destructor
    virtual ~Interface() { LoKi::release ( m_object ) ; } // virtual Destructor
    // ========================================================================
  public:
    // ========================================================================
    /// assignement from the raw pointer
    Interface& operator= ( const TYPE* obj )
    {
      if ( m_object == obj ) { return *this ; }
      //
      TYPE* tmp = m_object ;
      m_object = const_cast<TYPE*> ( obj ) ;
      //                                              the order *DOES* matter
      LoKi::addRef  ( m_object ) ;                 // 1) increment the counter
      LoKi::release ( tmp      ) ;                 // 2) decrement the counter
      //
      return *this ;
    }
    /// the regular assignement
    Interface& operator= ( const Interface<TYPE>& right )
    {
      if ( &right == this ) { return *this ; }
      //
      TYPE* tmp = m_object ;
      m_object  = right.getObject () ;
      //                                              the order *DOES* matter
      LoKi::addRef  ( m_object ) ;                 // 1) increment the counter
      LoKi::release ( tmp      ) ;                 // 2) decrement the counter
      //
      return *this ;
    }
    /// the templated assignement
    template <class OTHER>
    Interface& operator= ( const Interface<OTHER>& right )
    {
      TYPE* tmp = m_object ;
      m_object  = right.getObject () ;
      //                                              the order *DOES* matter!
      LoKi::addRef  ( m_object ) ;                 // 1) increment the counter
      LoKi::release ( tmp      ) ;                 // 2) decrement the counter
      //
      return *this ;                                  // RETURN
    }
    // ========================================================================
  public:
    // ========================================================================
    /// comparison with raw pointer
    bool operator== ( const TYPE* other ) const
    { return m_object == other ; }
    /// comparison with same type pointer
    bool operator== ( const Interface<TYPE>& other ) const
    { return m_object == other.m_object ; }
    /// comparison with other type of pointer
    template <class OTHER>
    bool operator== ( const Interface<OTHER>& other ) const
    { return m_object == other.m_object ; }
    // ========================================================================
  public:
    // ========================================================================
    bool same ( const TYPE* other ) const { return m_object == other ; }
    // ========================================================================
  public:
    // ========================================================================
    /// valid pointer?
    bool validPointer         () const { return 0 != m_object   ; }
    /// invalid ?
    bool operator!            () const { return !validPointer() ; }
    /// valid?
    explicit operator bool    () const { return validPointer()  ; }
    /// conversion to underlying type
    inline TYPE* operator->   () const { return getObject()     ; }
    /// automatic conversion
    inline operator TYPE*     () const { return getObject()     ; }
    /// access to the object
    inline TYPE* get          () const { return m_object        ; }
    /// access to the object
    inline TYPE* getObject    () const { return m_object        ; }
    // ========================================================================
  public :
    // ========================================================================
    void reset () {  m_object = 0 ; }
    // ========================================================================
  public :
    // ========================================================================
    /// release the underlying pointer & invalidate the object
    void release ()
    {
      LoKi::release ( m_object ) ;
      m_object = 0 ;
    }
    // ========================================================================
  private:
    // ========================================================================
    /// the object to be stored
    TYPE* m_object ;                                 // the object to be stored
    // ========================================================================
  } ;
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
// The END
// ============================================================================
#endif // LOKI_INTERFACE_H
// ============================================================================
