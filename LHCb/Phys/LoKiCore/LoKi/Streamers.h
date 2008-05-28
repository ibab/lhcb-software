// $Id: Streamers.h,v 1.2 2008-05-28 13:40:29 cattanem Exp $
// ============================================================================
#ifndef LOKI_STREAMERS_H 
#define LOKI_STREAMERS_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Primitives.h"
#include "LoKi/Cast.h"
#include "LoKi/apply.h"
#include "LoKi/compose.h"
// ============================================================================
/** compose/chain the function 
 *
 *  @code 
 *
 *  typedef Functor<Type1,Type2>             Func1 ;
 *  typedef Functor<Type2,Type3>             Func2 ;
 *  typedef Functor<Type3,Type4>             Func3 ;
 *  typedef Functor<Type4,Type5>             Func4 ;
 *
 *  typedef FunctorFromFunctor<Type1,Type5>  Fun   ;
 *
 *  void func ( const Func1& A , 
 *              const Func2& B , 
 *              const Func3& C , 
 *              const Func4& D )
 *  {
 *
 *    Fun  fun = A >> B >> C >> D  ; 
 *
 *  }
 *
 *  @endcode  
 *
 *  @see LoKi::Compose
 *
 *  @param fun1 the first functor to be chained 
 *  @param fun2 the second functor to be chained 
 *  @return the chained functor
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23  
 */
template <class TYPE,class TYPE1,class TYPE2>
inline LoKi::Compose<TYPE,TYPE1,TYPE2>
operator >> ( const LoKi::Functor<TYPE,TYPE1>&  fun1 ,
              const LoKi::Functor<TYPE1,TYPE2>& fun2 ) 
{ return LoKi::compose ( fun1 , fun2 ) ; }
// ============================================================================
/** the streamer for two predicates is "logical AND" for the predicates 
 *
 *  @code
 * 
 *   const Functor<TYPE,bool>& pred1 ;
 *   const Functor<TYPE,bool>& pred2 ;
 *
 *   SEQUENCE input = ... ;
 *
 *   SEQUENCE output1 = input >>   pred1 >> pred2   ;
 *
 *   /// the same, but a bit "old-fashined":
 *   SEQUENCE output2 = input >> ( pred1 && pred2 ) ;
 *
 *  @endcode  
 *  
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23   
 */    
template <class TYPE> 
inline LoKi::And<TYPE,bool>
operator >> 
( const LoKi::Functor<TYPE,bool>& cut1 , 
  const LoKi::Functor<TYPE,bool>& cut2 ) 
{ return LoKi::And<TYPE> ( cut1 , cut2 ) ; }
// ============================================================================
/** evaluate the function throgh stremer operation:
 *  
 *  @code 
 *
 *  typedef Functor<Type1,Type2>             Func ;
 *
 *  const Func&  fun   = ... ;
 *  const Type1& input = ... ;
 *
 *  Type2 output = input >> fun ;
 *
 *  @endcode  
 *
 *  @see LoKi::apply 
 *
 *  @param a argument 
 *  @param o the functor 
 *  @return the result of the functor evaluation 
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23   
 */
template <class TYPE,class TYPE2>
inline typename LoKi::Functor<TYPE,TYPE2>::result_type 
operator >> ( typename LoKi::Functor<TYPE,TYPE2>::argument a ,
              const LoKi::Functor<TYPE,TYPE2>&             o )
{ return LoKi::apply ( o , a ) ; }
// ============================================================================
/** evaluate the vector function  ("map" or "yield")
 *
 *  @code 
 *
 *  typedef Functor<Type1,Type2>             Func ;
 *
 *  const std::vector<Type1> input = ...
 *
 *  std::vector<Type2> output = input >> fun ;
 *
 *  @endcode  
 *
 *  @see LoKi::apply 
 *
 *  @param input vector in input arguments 
 *  @param funct the functor
 *  @return the vector of results 
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23   
 */  
template <class TYPEI, class TYPE,class TYPE2>
inline std::vector<typename LoKi::Functor<TYPE,TYPE2>::result_type>
operator >> ( const std::vector<TYPEI>&        input ,  
              const LoKi::Functor<TYPE,TYPE2>& funct )
{
  typedef std::vector<typename LoKi::Functor<TYPE,TYPE2>::result_type> OUTPUT ;
  OUTPUT out ;
  out.reserve ( input.size() ) ;
  //
  LoKi::apply 
    ( input.begin () , input.end () , funct , std::back_inserter ( out ) ) ;
  // 
  return out ;
}
// ============================================================================
/** evaluate the vector function with vector argument 
 *
 *  @code 
 *
 *  typedef Functor<std::vector<Type1>,Type2>             Func ;
 *
 *  const std::vector<Type3> input = ...
 *
 *  Type2 output = input >> fun ;
 *
 *  @endcode  
 *
 *  @see LoKi::apply 
 *
 *  @param input vector in input arguments 
 *  @param funct the functor
 *  @return the vector of results 
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23   
 */  
template <class TYPEI, class TYPE,class TYPE2>
inline typename LoKi::Functor<std::vector<TYPE>,TYPE2>::result_type
operator >> ( const std::vector<TYPEI>&                     input ,  
              const LoKi::Functor<std::vector<TYPE>,TYPE2>& funct )
{
  typedef std::vector<TYPE> INPUT ;
  return LoKi::apply ( funct , INPUT( input.begin() , input.end() ) ) ;
}
// ============================================================================
/** evaluate/filter the vector function/predicate 
 *
 *  @code 
 *
 *  typedef Functor<Type1,bool>            Func ;
 *
 *  const std::vector<Type1> input = ...
 *
 *  std::vector<Type1> output = input >> fun ;
 *
 *  @endcode  
 *
 *  @see LoKi::apply 
 *
 *  @param input vector in input arguments 
 *  @param pred the predicate to be evaluated 
 *  @return the filtered inputs 
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23   
 */  
template <class TYPEI, class TYPE>
inline std::vector<TYPEI>
operator >> 
( const std::vector<TYPEI>&       input ,  
  const LoKi::Functor<TYPE,bool>& pred  )
{
  typedef std::vector<TYPEI> OUTPUT ;
  //
  OUTPUT out ;
  out.reserve ( input.size() ) ;
  //
  LoKi::apply_filter 
    ( input.begin() , input.end() , pred , std::back_inserter ( out ) ) ;
  // 
  return out ;
}
// ============================================================================
/** evaluate the vector fucntion with scalar argument through 
 *  the construction of the intermediate vector of length 1
 *  
 *  @code 
 *
 *  const Functor<std::vector<TYPE>, TYPE2> & functor = ... ;
 *
 *  const TYPE& object = ... ;
 * 
 *  TYPE2 result = object >> functor ;
 *
 *  @endcode  
 *
 *  @see LoKi::apply 
 *
 *  @param a vector in input arguments 
 *  @param f the predicate to be evaluated 
 *  @return the filtered inputs 
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23   
 */  
template <class TYPE,class TYPE2> 
inline 
typename LoKi::Functor<std::vector<TYPE>,TYPE2>::result_type 
operator>>  ( typename LoKi::Functor<TYPE,TYPE2>::argument  a , 
              const LoKi::Functor<std::vector<TYPE>,TYPE2>& f )
{
  return LoKi::apply ( f , a ) ;
}        
// ============================================================================
/** "source" : produce the sequence from nothing 
 *
 *   @code
 *  
 *    const LoKi::Functor<void, std::vector<TYPE> > source ;
 *
 *    std::vector<TYPE> dest ;
 *
 *    source >> dest ;
 *
 *   @endcode
 *
 *  The concept belongs to the Gerhard "The Great" Raven.
 *
 *   @param  source the source 
 *   @param  dest   the destination 
 *   @return the destination 
 *
 *   @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *   @date 2001-01-23   
 */  
template <class TYPE>
inline std::vector<TYPE> 
operator >> 
( const LoKi::Functor<void,std::vector<TYPE> >& source , 
  std::vector<TYPE>&                            dest   ) 
{
  dest = source.evaluate() ;
  return dest ;
}
// ============================================================================
/** "source": produce the sequnce from nothing 
 *
 *   @code
 *  
 *    const LoKi::Functor<void, std::vector<TYPE*> > source ;
 *
 *    std::vector<const TYPE*> dest ;
 *
 *    source >> dest ;
 *
 *   @endcode
 *
 *  The concept belongs to the Gerhard "The Great" Raven.
 *
 *   @param  source the source 
 *   @param  dest   the destination 
 *   @return the destination 
 *
 *   @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *   @date 2001-01-23   
 */  
template <class TYPE>
inline std::vector<const TYPE*> 
operator >> 
( const LoKi::Functor<void,std::vector<TYPE*> >& source , 
  std::vector<const TYPE*>&                      dest   ) 
{
  std::vector<TYPE*>& aux = source.evaluate() ;
  dest = std::vector<const TYPE*>( aux.begin() , aux.end() ) ;
  return dest ;  
}
// ============================================================================
/** "source" : produce the sequence from nothing 
 *
 *   @code
 *  
 *    const LoKi::Functor<void, std::vector<const TYPE*> > source ;
 *
 *    std::vector<TYPE*> dest ;
 *
 *    source >> dest ;
 *
 *   @endcode
 *
 *  The concept belongs to the Gerhard "The Great" Raven.
 *
 *   @param  source the source 
 *   @param  dest   the destination 
 *   @return the destination 
 *
 *   @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *   @date 2001-01-23   
 */  
template <class TYPE>
inline std::vector<TYPE*> 
operator >> 
( const LoKi::Functor<void,std::vector<const TYPE*> >& source , 
  std::vector<TYPE*>&                                  dest   ) 
{
  /// evaluate the functor 
  std::vector<const TYPE*>& aux = source.evaluate() ;
  dest.erase() ;
  dest.reserve( aux.size() ) ;
  std::transform 
    ( aux.begin ()                  , 
      aux.end   ()                  , 
      std::back_inserter ( dest )   , 
      LoKi::Cast::ConstAway<TYPE>() ) ;
  /// 
  return dest ;                   
}
// ============================================================================
/** chaining/composition of the vector-function and the scalar one 
 * 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2007-11-29
 */
//template <class TYPE,class TYPE1, class TYPE2>
//inline
//inline LoKi::Compose<TYPE,std::vector<TYPE1>,std::vector<TYPE2> >
//operator>> ( const LoKi::Functor<TYPE,std::vector<TYPE1> >& fun1 , 
//             const LoKi::Functor<TYPE1,TYPE2>&              fun2 ) 
//{ 
//  return LoKi::Compose<TYPE,std::vector<TYPE1>,std::vector<TYPE2> > 
//    ( fun1 , LoKi::Yields<TYPE1,TYPE2,TYPE1> ( fun2 ) ) ;
//}

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_STREAMERS_H
// ============================================================================
