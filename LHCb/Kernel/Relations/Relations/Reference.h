// $Id: Reference.h,v 1.1 2006-06-11 15:23:45 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef RELATIONS_REFERENCE_H 
#define RELATIONS_REFERENCE_H 1
// ============================================================================
// Include files
// ============================================================================
// ST D& STL 
// ============================================================================
#include <cassert>
// ============================================================================

namespace Relations
{
  /** @class Reference Reference.h Relations/Reference.h
   *  
   *
   *  @author Vanya BELYAEV ibelyaev@physcis.syr.edu
   *  @date   2006-06-09
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
    inline          TYPE& get () const 
    { 
      assert ( 0 != m_data && "Relations::Reference:get(): invalid pointer" );
      return *m_data ; 
    } ;
    inline operator TYPE&     () const { return   get() ; }
    inline TYPE& operator*    () const { return   get() ; }
  public:
    inline Reference& operator=( const TYPE& value ) 
    { m_data = const_cast<TYPE*>( &value ) ; return *this ; } ;
  public:
    bool operator<( const Reference&  right ) const 
    {
      static const std::less<TYPE> _less ;
      return 
        &right == this         ? false : 
        m_data == right.m_data ? false : 
        0      == m_data       ? true  : 
        0      == right.m_data ? false : _less ( *m_data , *right.m_data ) ;  
    } ;
    bool operator==( const Reference& right ) const 
    { 
      static const std::equal_to<TYPE> _equal ;
      return
        &right == this           ? true  :
        m_data == right.m_data   ? true  : 
        0      == m_data         ? false : 
        0      == right.m_data   ? false : _equal ( *m_data , *right.m_data ) ;
    }
  private:
    TYPE* m_data ;
  } ;  
} // end of namespace relations 


// ============================================================================
// The END 
// ============================================================================
#endif // RELATIONS_REFERENCE_H
// ============================================================================
