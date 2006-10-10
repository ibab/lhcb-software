// $Id: GetType.h,v 1.1 2006-10-10 09:03:21 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef LOKI_GETTYPE_H 
#define LOKI_GETTYPE_H 1
// ============================================================================
// Include files
// ============================================================================


namespace LoKi 
{
  /** @struct GetType GetType.h LoKi/GetType.h
   *  
   *  Helper utility to get "raw" type 
   *
   *  @author Ivan BELYAEV
   *  @date   2006-10-07
   */
  template <class TYPE>
  struct GetType 
  {
    typedef TYPE Type ;
  } ;
  
  template <class TYPE>
  struct GetType<const TYPE> 
  {
    typedef typename GetType<TYPE>::Type Type ;
  } ; 
  
  template <class TYPE>
  struct GetType<TYPE&> 
  {
    typedef typename GetType<TYPE>::Type Type ;
  } ;
  
  template <class TYPE>
  struct GetType<TYPE*> 
  {
    typedef typename GetType<TYPE>::Type Type ;
  } ;
  
} // end of namespace LoKi 

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_GETTYPE_H
// ============================================================================
