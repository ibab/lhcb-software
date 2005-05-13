// $Id: Pointer.h,v 1.1 2005-05-13 11:22:29 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef RELATIONS_POINTER_H 
#define RELATIONS_POINTER_H 1
// ============================================================================
// Include files
// ============================================================================

namespace Relations
{
  /** @class Pointer Pointer.h Relations/Pointer.h
   *  
   *  simple wrapper class for pointer ( similar to boost::ptr) 
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@lapp.in2p3.fr
   *  @date   2005-05-12
   */
  template <class TYPE>
  class Pointer 
  {
  public:
    Pointer ( const TYPE* data = 0 ) 
      : m_data ( const_cast<TYPE*>( data ) ) 
    {} ;
    /// destructor 
    ~Pointer(){}
  public:
    inline          TYPE* get () const { return m_data ; }
    inline operator TYPE*     () const { return get() ; }
    inline TYPE* operator->   () const { return get() ; }
  private:
    TYPE* m_data ;
  };
  // "const-away"
  template <class TYPE>
  class Pointer<const TYPE>
  {
  public:
    Pointer ( const TYPE* data = 0 ) 
      : m_data ( const_cast<TYPE*>( data ) ) 
    {} ;
    /// destructor 
    ~Pointer(){}
  public:
    inline          TYPE* get () const { return m_data ; }
    inline operator TYPE*     () const { return get() ; }
    inline TYPE* operator->   () const { return get() ; }
  private:
    TYPE* m_data ;
  };

}; // end of namespace Relations 


// ============================================================================
//  The END 
// ============================================================================
#endif // RELATIONS_POINTER_H
// ============================================================================
