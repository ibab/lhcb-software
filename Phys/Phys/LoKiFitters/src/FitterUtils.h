// $Id: FitterUtils.h,v 1.1.1.1 2008-02-20 15:48:44 ibelyaev Exp $
// ============================================================================
#ifndef FITTERUTILS_H 
#define FITTERUTILS_H 1
// ============================================================================
// Include files
// ============================================================================
// Event 
// ============================================================================
#include  "Event/Particle.h"
#include  "Event/Vertex.h"
// ============================================================================

/** @namespace FitterUtils FitterUtils.h
 *  Helper namespace with various utility classes, useful for kinematical fits 
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2006-04-12
 */
namespace FitterUtils 
{ 
  /** @class Entry
   *  Helper class useful for kinematical fits 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-04-12
   */
  class Entry 
  {
  public:
    // the original particle 
    const LHCb::Particle* m_p0  ;
    // the properly extrapolated particle 
    LHCb::Particle        m_p   ;
    //     "Measurements"
    // parameter vector (x,y,z,px,py,pz,e)
    Gaudi::Vector3        m_parx ;
    Gaudi::Vector4        m_parq ;
    // inverse of the (modified?) covariance submatrix for (x,y,z)
    Gaudi::SymMatrix3x3   m_vxi ;
    //      Projectors are trivial and omitted 
    //      Kalman filter values:
    double                            m_chi2 ;
    // vertex position  (x,y,z)
    Gaudi::Vector3                    m_x    ;
    // position errors  
    Gaudi::SymMatrix3x3               m_c    ;
    // position errors  (inverse) 
    Gaudi::SymMatrix3x3               m_ci   ;
    // parameter vector (px,py,pz,e)
    Gaudi::Vector4                    m_q    ;
    // covariance matrix for q-parameters
    Gaudi::SymMatrix4x4               m_d    ;
    // covariance matrix for (x,q)
    Gaudi::Matrix4x3                  m_e    ;
    Gaudi::Matrix4x3                  m_f    ;
  } ;
  
} ;

// ============================================================================
// The END 
// ============================================================================
#endif // FITTERUTILS_H
// ============================================================================
