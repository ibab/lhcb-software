// $Id: FitterUtils.h,v 1.2 2008-02-24 19:48:19 ibelyaev Exp $
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
  // ==========================================================================
  /** @class Entry
   *  Helper class useful for kinematical fits.
   *  It stores in th euseful format the most important 
   *  values neede for vertex Kalman fit procedure 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-04-12
   */
  class Entry 
  {
  public:
    // the original particle 
    const LHCb::Particle* m_p0   ; ///< pointer to the original particle 
    // the properly extrapolated particle 
    LHCb::Particle        m_p    ; ///< properly extrapolated (local) particle 
    // "Measurements": The parameter vector alpha=(x,y,z,px,py,pz,e)
    Gaudi::Vector3        m_parx ;  ///< \f$\vec{x}=(x,y,z)\f$ 
    Gaudi::Vector4        m_parq ;  ///< \f$\vec{q}=(p_x,p_y,p_z,E)\f$ 
    // inverse of the (modified?) covariance submatrix for (x,y,z)
    Gaudi::SymMatrix3x3   m_vxi  ;  ///< \f$C_{\vec{x}}^{-1}\f$
    //      Projectors are trivial and omitted 
    //      Kalman filter values:
    double                            m_chi2 ; ///< \f$\chi^2\f$
    // vertex position  (x,y,z)
    Gaudi::Vector3                    m_x    ; ///< \f$\vec{x}=(x,y,z)\f$ 
    // position errors  
    Gaudi::SymMatrix3x3               m_c    ; ///< \f$C_{\vec{x}}=cov(\vec{x})\f$
    // position errors  (inverse) 
    Gaudi::SymMatrix3x3               m_ci   ; ///< \f$C_{\vec{x}}^{-1}\f$
    // parameter vector (px,py,pz,e)
    Gaudi::Vector4                    m_q    ; ///< \f$\vec{q}=(p_x,p_y,p_z,E)\f$ 
    // covariance matrix for q-parameters
    Gaudi::SymMatrix4x4               m_d    ; ///< \f$C_q=cov(\vec{q})\f$
    // covariance matrix for (x,q)
    Gaudi::Matrix4x3                  m_e    ; ///< \f$E=cov(\vec{x},vec{q})\f$
    // auxillary matrix F 
    Gaudi::Matrix4x3                  m_f    ; ///< \f$F=WB^{T}GA\f$
  } ;
  // ==========================================================================  
} // end of namespace Fitter Utils
// ============================================================================
// The END 
// ============================================================================
#endif // FITTERUTILS_H
// ============================================================================
