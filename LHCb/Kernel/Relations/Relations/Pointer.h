// $Id: Pointer.h,v 1.3 2006-06-11 17:46:05 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.3 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2006/06/11 15:23:45  ibelyaev
//  The major  upgrade: see doc/release.notes
//
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
    Pointer ( const TYPE* data = 0 ) : m_data ( const_cast<TYPE*>( data ) ) {} ;
    /// destructor 
    ~Pointer(){}
  public:
    inline          TYPE* get () const { return m_data ; }
    inline operator TYPE*     () const { return  get() ; }
    inline TYPE* operator->   () const { return  get() ; }
  public:
    inline Pointer& operator=( const TYPE* value ) 
    { m_data = const_cast<TYPE*> ( value ) ; return *this ; } ;
  public:
    bool operator<  ( const Pointer& right ) const
    { return m_data < right.m_data ; }
    bool operator== ( const Pointer& right ) const
    { return &right == this || m_data == right.m_data ; }
  private:
    TYPE* m_data ;
  };
  // "const-away"
  template <class TYPE>
  class Pointer<const TYPE> : public Pointer<TYPE> 
  {
  public:
    /// constructor 
    Pointer ( const TYPE* data = 0 ) : Pointer<TYPE>( data ) {}
    /// destructor 
    ~Pointer() {}
  } ;

}; // end of namespace Relations 


// ============================================================================
//  The END 
// ============================================================================
#endif // RELATIONS_POINTER_H
// ============================================================================
