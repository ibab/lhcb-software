// $Id: FilterOps.h,v 1.1 2007-11-28 13:56:32 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_FILTEROPS_H 
#define LOKI_FILTEROPS_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Functions.h"
#include "LoKi/Filters.h"
#include "LoKi/select.h"
// ============================================================================
/** The streaming/chaining/filtering  operator for the vector and predicate 
 * 
 *  @code 
 * 
 *  LHCb::Particle::ConstVector input ;
 * 
 *  Cut cut1 = ... ;
 *  Cut cut2 = ... ;
 *  Cut cut3 = ... ;
 *  Cut cut4 = ... ;
 *  
 *  LHCb::Particle::ConstVector output = 
 *     input >> cut1 >> cut2 >> cut3 >> cut4 ;
 *
 *  @endcode 
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2007-10-30
 */  
template <class TYPE1,class ALLOCATOR,class TYPE2>
inline std::vector<TYPE1,ALLOCATOR> 
operator>> 
( const std::vector<TYPE1,ALLOCATOR>& input , 
  const LoKi::Predicate<TYPE2>&       cut   )
{
  typedef std::vector<TYPE1,ALLOCATOR> INPUT ;
  // create the output 
  INPUT output ;
  output.reserve ( input.size() ) ;
  // std::copy_if 
  LoKi::select 
    ( input.begin() , 
      input.end()   , 
      std::back_inserter( output ) , cut ) ;
  //
  return output ;
}
// ============================================================================
/** streaming(chaining) operator for LoKi-predicates 
 *  @code 
 * 
 *  LHCb::Particle::ConstVector input ;
 * 
 *  Cut cut1 = ... ;
 *  Cut cut2 = ... ;
 *  Cut cut3 = ... ;
 *  Cut cut4 = ... ;
 *  
 *  LHCb::Particle::ConstVector output = 
 *     input >> ( cut1 >> cut2 >> cut3 >> cut4 )  ;
 *
 *  @endcode 
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2007-10-30
 */    
template <class TYPE>
inline LoKi::And<TYPE> 
operator>> 
( const LoKi::Predicate<TYPE2>& first  , 
  const LoKi::Predicate<TYPE2>& second ) 
{ return LoKi::And<TYPE>( first , second ) ; }
// ============================================================================
/** The streaming operator for the vector and function 
 * 
 *  @code 
 * 
 *  LHCb::Particle::ConstVector input ;
 * 
 *  Fun fun  = ... ;
 * 
 *  std::vector<double> output = input >> fun ;
 *
 *  @endcode 
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2007-10-30
 */  
template <class TYPE1,class TYPE2>
inline  typename LoKi::Function<TYPE2>::vector_result 
operator>> 
( const std::vector<TYPE1,ALLOCATOR>& input , 
  const LoKi::Function<TYPE2>&        fun   )
{
  typedef std::vector<TYPE1,ALLOCATOR>         INPUT ;
  typedef LoKi::Function<TYPE2>::vector_result OUTPUT ;
  // create the output 
  OUTPUT output ;
  output.reserve ( input.size() ) ;
  // use the functor"  
  fun ( inpit.begin() , input.end() , std::back_inserter ( output ) ) ;
  // 
  return output ;
}

template <class TYPE1,class ALLOCATOR,class TYPE2>
inline std::vector<TYPE1,ALLOCATOR> 
operator>> 
( const std::vector<TYPE1,ALLOCATOR>& input , 
  const LoKi::Filters::Tee<TYPE2>&    t  ) 
{  
  typedef std::vector<TYPE1,ALLOCATOR> INPUT ;
  for ( INPUT::const_iterator it = input.begin() ; 
        input.end() != it ; ++it ) { tee1.cut() ( *it ) ; }
  return input ; ///< return the input 
}

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_FILTEROPS_H
// ============================================================================

