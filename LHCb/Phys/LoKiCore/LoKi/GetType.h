// $Id: GetType.h,v 1.2 2007-07-23 17:07:38 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_GETTYPE_H 
#define LOKI_GETTYPE_H 1
// ============================================================================
// Include files
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
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
  // ==========================================================================  
  template <class TYPE>
  struct GetType<const TYPE> 
  {
    typedef typename GetType<TYPE>::Type Type ;
  } ; 
  // ==========================================================================  
  template <class TYPE>
  struct GetType<TYPE&> 
  {
    typedef typename GetType<TYPE>::Type Type ;
  } ;
  // ==========================================================================  
  template <class TYPE>
  struct GetType<TYPE*> 
  {
    typedef typename GetType<TYPE>::Type Type ;
  } ;
  // ==========================================================================  
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_GETTYPE_H
// ============================================================================
