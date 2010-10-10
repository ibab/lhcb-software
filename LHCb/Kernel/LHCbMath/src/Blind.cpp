// $Id: $
// ============================================================================
// Include files 
// ============================================================================
// STD& STL
// ============================================================================
#include <string>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Hash.h"
// ============================================================================
// Boots 
// ============================================================================
#include "boost/integer_traits.hpp"
#include "boost/static_assert.hpp"
// ============================================================================
// Kernel/LHcbMath
// ============================================================================
#include "LHCbMath/Blind.h"
// ============================================================================
/** @file
 *  Implementation file for function Gaudi::Math::blind 
 *  @see Gaudi::Math::blind 
 *  @author Vanya Belyaev Ivan.Belyaev@nikhef.nl
 *  @date 2010-10-10
 */
// ============================================================================
/// local anonymous namespace to hide all implementation details 
// ============================================================================
namespace 
{
  // ===========================================================================
  // prerequisites for "correct" 
  BOOST_STATIC_ASSERT( boost::integer_traits<unsigned int> :: is_specialized && 
                       boost::integer_traits<unsigned int> :: is_integral    && 
                       boost::integer_traits<unsigned int> :: const_min == 0 && 
                       boost::integer_traits<unsigned int> :: const_max  > 0 ) ;
  // ==========================================================================
  // define proper double 
  // ==========================================================================
  /// get the hash of the value 
  unsigned int _hash_ ( const std::string& name ) 
  { 
    // VB. It is not so uniform for 'short' strings
    // GaudiUtils::Hash<std::string> hash ;
    // return hash( name ) ; 
    // this one it better... 
    return InterfaceID::hash32 ( name.c_str() ) ; 
  }
  /// normalize the hash [0,1]
  double       _norm_ ( const unsigned int hashv ) 
  {
    //
    static const double ui_max = boost::integer_traits<unsigned int> :: const_max ;
    //
    return double ( std::min ( hashv , boost::integer_traits<unsigned int> :: const_max - 1 ) ) / ui_max ;
  }
  // ==========================================================================
  /// reuturn the normalized value [-1,1] of hash 
  double _blind_ ( const std::string& name ) 
  { 
    return 2 * _norm_ ( _hash_ ( name ) ) - 1 ;
  }  
  // ==========================================================================
} //                                           end of local anonymous namespace 
// ============================================================================
/*  get the 'blind'-number form the string 
 *  @param name the name to be used for construction of the coefficient
 *  @param minv the minimal value 
 *  @param minv the maximal value 
 *  @return the value that can be used for blinding 
 *  @author Vanya Belyaev Ivan.Belyaev@nikhef.nl
 *  @date 2010-10-10
 */
// ============================================================================
double Gaudi::Math::blind 
( const std::string& name , 
  const double       minv , 
  const double       maxv ) 
{
  //
  if      ( minv == maxv ) { return minv ; }
  else if ( minv  > maxv ) { return blind ( name , maxv , minv ) ; }
  //
  return 0.5 * ( minv + maxv ) + 0.5 * ( maxv - minv ) *  _blind_ ( name ) ;
}



// ============================================================================
// The END 
// ============================================================================
