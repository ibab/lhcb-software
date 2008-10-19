// $Id: apply.h,v 1.4 2008-10-19 16:11:40 ibelyaev Exp $
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
  /** the direct use the functor 
   *  essentially no gain is expected, but needed to terminate the 
   *  template argument resolution 
   *
   *  @code 
   *
   *   const LoKi::Functor<TYPE,TYPE2>& functor = ... ;
   *  
   *   const TYPE& object = ... ;
   * 
   *   TYPE2 result = LoKi::apply ( functor , object ) ;
   *
   *  @endcode 
   * 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2001-01-23   
   */  
  template <class TYPE,class TYPE2> 
  inline typename LoKi::Functor<TYPE,TYPE2>::result_type 
  apply  ( const LoKi::Functor<TYPE,TYPE2>&             f ,  
           typename LoKi::Functor<TYPE,TYPE2>::argument a ) 
  { return f ( a ) ; }
  // ==========================================================================
  /** the direct use the function (save one virtual call)
   *  For some (rare?) cases it could be a bit more efficient.
   *  
   *  @code 
   *
   *   const LoKi::FunctorFrinFunctor<TYPE,TYPE2>& functor = ... ;
   *  
   *   TYPE* object = ... ;
   * 
   *   TYPE2 result = LoKi::apply ( functor , object ) ;
   *
   *  @endcode 
   * 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2001-01-23   
   */   
  template <class TYPE,class TYPE2> 
  inline typename LoKi::Functor<TYPE,TYPE2>::result_type 
  apply  ( const LoKi::FunctorFromFunctor<TYPE,TYPE2>&  f ,  
           typename LoKi::Functor<TYPE,TYPE2>::argument a ) 
  { return apply ( f.fun() , a ) ; }
  // ==========================================================================
  /** Adapter: use the functor through the dereferencing of the argument
   *  
   *  @code 
   *
   *   const LoKi::Functor<TYPE,TYPE2>& functor = ... ;
   *  
   *   TYPE* object = ... ;
   * 
   *   TYPE2 result = LoKi::apply ( functor , object ) ;
   *
   *  @endcode 
   * 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2001-01-23   
   */   
  template <class TYPE,class TYPE2> 
  inline typename LoKi::Functor<TYPE,TYPE2>::result_type 
  apply  ( const LoKi::Functor<TYPE,TYPE2>&              f ,  
           typename LoKi::Functor<TYPE*,TYPE2>::argument a ) 
  { return apply ( f , *a ) ; }
  // ==========================================================================
  /** Adapter: use the functor through dereferencing of the argument
   *  
   *  @code 
   *
   *   const LoKi::Functor<TYPE,TYPE2>& functor = ... ;
   *  
   *   const TYPE* object = ... ;
   * 
   *   TYPE2 result = LoKi::apply ( functor , object ) ;
   *
   *  @endcode 
   * 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2001-01-23   
   */
  template <class TYPE,class TYPE2> 
  inline typename LoKi::Functor<TYPE,TYPE2>::result_type 
  apply  ( const LoKi::Functor<TYPE,TYPE2>&                    f ,  
           typename LoKi::Functor<const TYPE*,TYPE2>::argument a ) 
  { return apply ( f , *a ) ; }
  // ==========================================================================
  /** Adapter: use the functor through adressing of the argument
   *  
   *  @code 
   *
   *   const LoKi::Functor<TYPE*,TYPE2>& functor = ... ;
   *  
   *   TYPE& object = ... ;
   * 
   *   TYPE2 result = LoKi::apply ( functor , object ) ;
   *
   *  @endcode 
   * 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2001-01-23   
   */
  template <class TYPE,class TYPE2> 
  inline typename LoKi::Functor<TYPE*,TYPE2>::result_type 
  apply  ( const LoKi::Functor<TYPE*,TYPE2>&            f ,  
           typename LoKi::Functor<TYPE,TYPE2>::argument a ) 
  { return apply ( f , &a ) ; }
  // ========================================================================== 
  /** Adapter: use the functor through adressing of the argument
   *  
   *  @code 
   *
   *   const LoKi::Functor<const TYPE*,TYPE2>& functor = ... ;
   *  
   *   const TYPE& object = ... ;
   * 
   *   TYPE2 result = LoKi::apply ( functor , object ) ;
   *
   *  @endcode 
   * 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2001-01-23   
   */
  template <class TYPE,class TYPE2> 
  inline typename LoKi::Functor<const TYPE*,TYPE2>::result_type 
  apply  ( const LoKi::Functor<const TYPE*,TYPE2>&      f ,  
           typename LoKi::Functor<TYPE,TYPE2>::argument a ) 
  { return apply ( f , &a ) ; }
  // ==========================================================================
  /** Adapter: use the vector-functor for scalar argument
   *  
   *  @code 
   *
   *   const LoKi::Functor<const TYPE*,TYPE2>& functor = ... ;
   *  
   *   const TYPE& object = ... ;
   * 
   *   TYPE2 result = LoKi::apply ( functor , object ) ;
   *
   *  @endcode 
   * 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2001-01-23   
   */
  template <class TYPE,class TYPE2> 
  inline typename LoKi::Functor<std::vector<TYPE>,TYPE2>::result_type 
  apply  ( const LoKi::Functor<std::vector<TYPE>,TYPE2>& f ,  
           typename LoKi::Functor<TYPE,TYPE2>::argument  a ) 
  { return apply ( f , std::vector<TYPE> ( 1 , a ) ) ; }
  // ==========================================================================
  /** Adapter: "vector-use" of the scalar function 
   *  
   *  @code 
   *
   *   const LoKi::Functor<TYPE,TYPE2>& functor = ... ;
   *  
   *   SEQUENCE objects = ... ;
   *   std::vector<TYPE2> results ;
   *   results.reserve ( objects.end() - objects.begin() ) ;
   * 
   *   LoKi::apply 
   *         ( objects.begin () , 
   *           objects.end   () , 
   *           functor          , 
   *           std::back_inserter ( results ) ) ;
   *
   *  @endcode 
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2001-01-23   
   */  
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
  /** Adapter: "vector-use" of the scalar function and ignore the result
   *  
   *  @code 
   *
   *   const LoKi::Functor<TYPE,TYPE2>& functor = ... ;
   *  
   *   SEQUENCE objects = ... ;
   * 
   *   LoKi::apply 
   *         ( objects.begin () , 
   *           objects.end   () , 
   *           functor          ) ;
   *
   *  @endcode 
   * 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2001-01-23   
   */    
  template <class INPUT, class TYPE, class TYPE2>
  inline void  
  apply ( INPUT                            first , 
          INPUT                            last  , 
          const LoKi::Functor<TYPE,TYPE2>& fun   )
  {
    for ( ; first != last ; ++first ) { LoKi::apply ( fun , *first ) ; }
  }
  // ==========================================================================
  /** Adapter: "vector-use" of the scalar predicate/filter
   *  @code 
   *
   *   const LoKi::Functor<TYPE,bool>& functor = ... ;
   *  
   *   SEQUENCE objects = ... ;
   *   SEQUENCE results ;
   *   results.reserve ( objects.end() - objects.begin() ) ;
   * 
   *   LoKi::apply_filter 
   *         ( objects.begin () , 
   *           objects.end   () , 
   *           functor          , 
   *           std:back_inserter ( results ) ) ;
   *
   *  @endcode 
   * 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2001-01-23   
   */      
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
   *
   *  The helper structure to allow "the polimorphic arguments"
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-11-02 
   */
  template <class TYPE,class TYPE2>
  struct Apply 
  {
  public:
    /// needed for the proper template instantiation 
    typedef typename LoKi::Functor<TYPE,TYPE2>::result_type result_type ;
  public:
    /// the constructor 
    Apply ( const LoKi::Functor<TYPE,TYPE2>* fun ) : m_functor ( fun ) {}
  public:
    /// the only one essential method 
    template <class ARGUMENT>
    inline typename LoKi::Functor<TYPE,TYPE2>::result_type 
    operator() ( ARGUMENT a ) const 
    { return LoKi::apply ( *m_functor , a ) ; }
    /// the only one essential method 
    inline typename LoKi::Functor<TYPE,TYPE2>::result_type 
    operator() ( typename LoKi::Functor<TYPE,TYPE2>::argument a ) const 
    { return LoKi::apply ( *m_functor , a ) ; }
    /// the only one essential method 
    template <class ARGUMENT>
    inline typename LoKi::Functor<TYPE,TYPE2>::result_type 
    eval       ( ARGUMENT a ) const 
    { return LoKi::apply ( *m_functor , a ) ; }
    /// the only one essential method 
    inline typename LoKi::Functor<TYPE,TYPE2>::result_type 
    eval       ( typename LoKi::Functor<TYPE,TYPE2>::argument a ) const 
    { return LoKi::apply ( *m_functor , a ) ; }    
  private:
    // the default constructor is disabled 
    Apply() ; ///< the default constructor is disabled
  private:
    /// the functor itself 
    const LoKi::Functor<TYPE,TYPE2>* m_functor ; // the functor itself 
  } ;
  // ============================================================================
  template <class TYPE2>
  struct Apply<void,TYPE2> 
  {
  private:
    // ========================================================================
    typedef void TYPE ;
    // ========================================================================
  public:
    // ========================================================================
    /// needed for the proper template instantiation 
    typedef typename LoKi::Functor<TYPE,TYPE2>::result_type result_type ;
    // ========================================================================
  public:
    // ========================================================================
    /// the constructor 
    Apply ( const LoKi::Functor<TYPE,TYPE2>* fun ) : m_functor ( fun ) {}
    // ========================================================================
  public:
    // ========================================================================
    /// the only one essential method 
    inline typename LoKi::Functor<TYPE,TYPE2>::result_type 
    operator() () const { return (*m_functor) () ; }
    /// the only one essential method 
    inline typename LoKi::Functor<TYPE,TYPE2>::result_type 
    eval       () const { return (*m_functor) () ; }
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled 
    Apply() ;                           // the default constructor is disabled
    // ========================================================================
  private:
    // ========================================================================
    /// the functor itself 
    const LoKi::Functor<TYPE,TYPE2>* m_functor ;         // the functor itself 
    // ========================================================================
  } ;
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_APPLY_H
// ============================================================================
