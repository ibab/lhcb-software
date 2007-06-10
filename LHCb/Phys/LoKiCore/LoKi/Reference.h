// $Id: Reference.h,v 1.1 2007-06-10 19:54:06 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_REFERENCE_H 
#define LOKI_REFERENCE_H 1 
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
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
    inline          TYPE& get () const 
    { 
      assert ( 0 != m_data && "LoKi::Reference:get(): invalid pointer" );
      return *m_data ; 
    } ;
    inline operator TYPE&     () const { return   get() ; }
    inline TYPE& operator*    () const { return   get() ; }
  public:
    inline Reference& operator=( const TYPE& value ) 
    { m_data = const_cast<TYPE*>( &value ) ; return *this ; } ;
  public:
    bool operator==( const Reference& right ) const 
    { 
      return &right == this || right.m_data == m_data ;
    }
    bool operator==( const TYPE& right ) const 
    { 
      return &right == m_data ;
    }
  private:
    TYPE* m_data ;    
  };
} //end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_REFERENCE_H
// ============================================================================
