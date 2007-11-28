// $Id: apply.h,v 1.1 2007-11-28 13:56:33 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_APPLY_H 
#define LOKI_APPLY_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Functor.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /// the direct use the function 
  template <class TYPE,class TYPE2> 
  inline typename LoKi::Functor<TYPE,TYPE2>::result_type 
  apply  ( const LoKi::Functor<TYPE,TYPE2>&             f ,  
           typename LoKi::Functor<TYPE,TYPE2>::argument a ) 
  { return f ( a ) ; }
  // ==========================================================================
  /// the direct use the function (save one virtual call)
  template <class TYPE,class TYPE2> 
  inline typename LoKi::Functor<TYPE,TYPE2>::result_type 
  apply  ( const LoKi::FunctorFromFunctor<TYPE,TYPE2>&  f ,  
           typename LoKi::Functor<TYPE,TYPE2>::argument a ) 
  { return apply ( f.fun() , a ) ; }
  // ==========================================================================
  /// use through dereferencing
  template <class TYPE,class TYPE2> 
  inline typename LoKi::Functor<TYPE,TYPE2>::result_type 
  apply  ( const LoKi::Functor<TYPE,TYPE2>&              f ,  
           typename LoKi::Functor<TYPE*,TYPE2>::argument a ) 
  { return apply ( f , *a ) ; }
  // ==========================================================================
  /// use through dereferencing
  template <class TYPE,class TYPE2> 
  inline typename LoKi::Functor<TYPE,TYPE2>::result_type 
  apply  ( const LoKi::Functor<TYPE,TYPE2>&                    f ,  
           typename LoKi::Functor<const TYPE*,TYPE2>::argument a ) 
  { return apply ( f , *a ) ; }
  // ==========================================================================
  /// use through address 
  template <class TYPE,class TYPE2> 
  inline typename LoKi::Functor<TYPE*,TYPE2>::result_type 
  apply  ( const LoKi::Functor<TYPE*,TYPE2>&            f ,  
           typename LoKi::Functor<TYPE,TYPE2>::argument a ) 
  { return apply ( f , &a ) ; }
  // ========================================================================== 
  /// use through address 
  template <class TYPE,class TYPE2> 
  inline typename LoKi::Functor<const TYPE*,TYPE2>::result_type 
  apply  ( const LoKi::Functor<const TYPE*,TYPE2>&      f ,  
           typename LoKi::Functor<TYPE,TYPE2>::argument a ) 
  { return apply ( f , &a ) ; }
  // ==========================================================================
  /// "vector-use" of the function 
  template <class INPUT, class TYPE, class TYPE2, class OUTPUT>
  inline OUTPUT 
  apply ( INPUT                            first , 
          INPUT                            last  , 
          const LoKi::Functor<TYPE,TYPE2>& fun   , 
          OUTPUT                           out   ) 
  {
    for ( ; first != last ; ++first ) 
    { *out = LoKi::apply ( fun , *first ) ; ++out ; }
    return out ;
  }
  // ==========================================================================
  /// "vector-use" of the function 
  template <class INPUT, class TYPE, class TYPE2>
  inline void  
  apply ( INPUT                            first , 
          INPUT                            last  , 
          const LoKi::Functor<TYPE,TYPE2>& fun   )
  {
    for ( ; first != last ; ++first ) { LoKi::apply ( fun , *first ) ; }
  }
  // ==========================================================================
  /// "vector-use" of the filter 
  template <class INPUT, class TYPE, class OUTPUT>
  inline OUTPUT 
  apply_filter ( INPUT                            first , 
                 INPUT                            last  , 
                 const LoKi::Functor<TYPE,bool>&  fun   , 
                 OUTPUT                           out   ) 
  {
    for ( ; first != last ; ++first ) 
    { if ( LoKi::apply ( fun , *first ) ) { *out = *first ; ++out ; } }
    return out ;
  }
  // ==========================================================================
  /** @struct Apply 
   *  the helper structure to allow "the polimorphic arguments"
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-11-02 
   */
  template <class TYPE,class TYPE2>
  struct Apply 
  {
  public:
    /// nedded for the proper template instantiation 
    typedef typename LoKi::Functor<TYPE,TYPE2>::result_type result_type ;
  public:
    /// the constructor 
    Apply ( const LoKi::Functor<TYPE,TYPE2>* fun ) : m_functor ( fun ) {}
  public:
    /// the only one essential method 
    template <class ARGUMENT>
    inline typename LoKi::Functor<TYPE,TYPE2>::result_type 
    operator() ( ARGUMENT& a ) const { return LoKi::apply ( *m_functor , a ) ; }
  private:
    // the default constructor is disabled 
    Apply() ; ///< the default constructor is disabled
  private:
    // the functor itself 
    const LoKi::Functor<TYPE,TYPE2>* m_functor ; ///< the functor itself 
  } ;
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_APPLY_H
// ============================================================================
