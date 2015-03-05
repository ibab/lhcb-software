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
// VDT: https://svnweb.cern.ch/trac/vdt
// ============================================================================
#include "vdt/sincos.h"
#include "vdt/atan2.h"
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
  inline 
  std::pair<double,double> _sincos_ ( const double phase ) 
  {
    // return std::make_pair ( std::sin( phase ) , std::cos( phase ) ) ;
    double sinv = 0 ;
    double cosv = 1 ;
    vdt::fast_sincos ( phase , sinv , cosv ) ;
    return std::make_pair ( sinv , cosv ) ;
  }
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
      //
      m_delta   [i]      = std::atan2 ( std::sqrt ( ni ) , 1.0L ) ;      
      const double phase = m_phases [i] + m_delta[i] ;
      //
      const std::pair<double,double> sincos = _sincos_ ( phase );
      m_sin_phi [i] = sincos.first  ;
      m_cos_phi [i] = sincos.second ;
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
      //
      m_delta   [i]      = std::atan2 ( std::sqrt ( ni ) , 1.0L ) ;      
      const double phase = m_phases [i] + m_delta[i] ;
      //
      const std::pair<double,double> sincos = _sincos_ ( phase );
      m_sin_phi [i] = sincos.first  ;
      m_cos_phi [i] = sincos.second ;
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
  //
  const std::pair<double,double> sincos = _sincos_ ( phase );
  m_sin_phi [ index ] = sincos.first  ;
  m_cos_phi [ index ] = sincos.second ;
  m_phases  [ index ] = value ;  // attention!! original values!! 
  //
  return true ;
}


// ============================================================================
// The END 
// ============================================================================
