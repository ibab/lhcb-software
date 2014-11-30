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
  , m_phases  (  N ,  0 ) 
  , m_sin_phi (  N ,  0 ) 
  , m_cos_phi (  N ,  1 ) 
{ 
  // ==============================
  // calculate the bias (if needed) 
  if  ( m_rotated )                       // ROTATE SPHERE 
  {
    for ( unsigned short  i = 0 ; i < N ; ++i )
    {
      const double ni    = N - i ;
      m_delta   [i]      = std::atan2 ( std::sqrt ( ni ) , 1.0L ) ;      
      const double phase = m_phases [i] + m_delta[i] ;
      m_sin_phi [i]      = std::sin   ( phase ) ;
      m_cos_phi [i]      = std::cos   ( phase ) ;      
    }
  }
  //
}
// ============================================================================
Gaudi::Math::NSphere::NSphere 
( const std::vector<double>& phases , 
  const bool                 rotated ) 
  : m_rotated ( rotated ) 
  , m_delta   ( phases.size () , 0 ) 
  , m_phases  ( phases ) 
  , m_sin_phi ( phases.size () , 0 ) 
  , m_cos_phi ( phases.size () , 1 ) 
{ 
  // ==============================
  // calculate the bias (if needed) 
  if  ( m_rotated )                       // ROTATE SPHERE 
  {
    for ( unsigned short  i = 0 ; i < m_phases.size() ; ++i )
    {
      const double ni    = m_phases.size () - i ;
      m_delta   [i]      = std::atan2 ( std::sqrt ( ni ) , 1.0L ) ;      
      const double phase = m_phases [i] + m_delta[i] ;
      m_sin_phi [i]      = std::sin   ( phase ) ;
      m_cos_phi [i]      = std::cos   ( phase ) ;      
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
bool Gaudi::Math::NSphere::setPhase 
( const unsigned short index , 
  const double         value ) 
{
  // 
  if ( nPhi() <= index ) { return false ; } // no change in unphysical phases 
  //
  if ( s_equal ( m_phases[index] , value ) ) { return false ; }
  //
  const double di     = m_rotated ? m_delta [ index ] : 0.0 ;
  const double phase  = value + di ;
  const double sinv   = std::sin ( phase ) ; // attention +delta!
  const double cosv   = std::cos ( phase ) ; // attention +delta!
  //
  m_phases  [ index ] = value ;  // attention!! original values!! 
  m_sin_phi [ index ] = sinv  ;
  m_cos_phi [ index ] = cosv  ;
  //
  return true ;
}


// ============================================================================
// The END 
// ============================================================================
