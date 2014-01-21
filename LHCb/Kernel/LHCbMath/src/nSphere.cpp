// $Id:$
// ============================================================================
// Include files 
// ============================================================================
// STD & STL
// ============================================================================
#include <vector>
#include <cmath>
// ============================================================================
// local
// ============================================================================
#include "LHCbMath/NSphere.h"
#include "LHCbMath/LHCbMath.h"
// ============================================================================
/** @file 
 *  Implementation of class Gaudi::Math::NSphere 
 *  @see Gaudi::Math::NSPhere 
 *  @author Vanya BELYAEV Ivan/Belyaev@itep.ru
 *  @date 2014-01-21 
 *  
 *  Version           $Revision:$
 *  Last modification $Date:$
 *                 by $Author:$
 */
// ============================================================================
namespace 
{
  // ==========================================================================
  /// equality criteria for doubles
  LHCb::Math::Equal_To<double> s_equal ;       // equality criteria for doubles
  // ==========================================================================
}
// ============================================================================
/*  Standard constructor
 *  @param N     dimensionality of N-sphere (number of phases)  
 *  @param bias  use the bias in phases? 
 */
// ============================================================================
Gaudi::Math::NSphere::NSphere 
( const unsigned short N       ,
  const bool           rotated ) 
  : m_rotated ( rotated ) 
  , m_delta   (  N ,  0 ) 
  , m_sin_phi (  N ,  0 ) 
  , m_cos_phi (  N ,  1 ) 
{ 
  // ==============================
  // calculate the bias (if needed) 
  if  ( m_rotated )                       // ROTATE SPHERE 
  {
    for ( unsigned short  i = 0 ; i < N ; ++i )
    {
      const double ni = N - i ;
      m_delta   [i] = std::atan2 ( std::sqrt ( ni ) , 1.0L ) ;
      m_sin_phi [i] = std::sin   ( m_delta[i] ) ;
      m_cos_phi [i] = std::cos   ( m_delta[i] ) ;      
    }
  }
  //
}
// ============================================================================
// destructor 
// ============================================================================
Gaudi::Math::NSphere::~NSphere() {}
// ============================================================================
// set new value for phi(i)      0 <= i < nPhi
// ============================================================================
bool Gaudi::Math::NSphere::setPhi 
( const unsigned short index , 
  const double         value ) 
{
  // 
  if ( nPhi() <= index ) { return false ; } // no change in unphysical phases 
  //
  const double di   = m_rotated ? delta ( index ) : 0.0 ;
  //
  const double sinv = std::sin ( value + di ) ;
  const double cosv = std::cos ( value + di ) ;
  //
  // 
  if ( s_equal ( sinv , m_sin_phi [ index ] ) && 
       s_equal ( cosv , m_cos_phi [ index ] ) ) { return false ; } // nothing to change
  //
  m_sin_phi [ index ] = sinv ;
  m_cos_phi [ index ] = cosv ;
  //
  return true ;
}


// ============================================================================
// The END 
// ============================================================================
