// $Id:$ 
// ============================================================================
// Include files 
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/LHCbMath.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Kinematics.h"
#include "LoKi/MissingParticle.h"
// ============================================================================
/** @file 
 *  Implementation file for class LoKi::Kinematics::MissingParticle
 *  @see LoKi::Kinematics::MissingParticle
 *  @date 2015-02-04 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *
 *                    $Revision:$
 *  Last modification $Date:$
 *                 by $Author:$
 */
// ============================================================================
namespace 
{
  // ==========================================================================
  /// is the value close to zero ? 
  const LHCb::Math::Zero<double> s_zero{} ;
  // ==========================================================================
}
// ============================================================================
/*  constructor from all arguments 
 *  @param p4  four momenutm of recotructed system 
 *  @param dir the flight direction of mother particle
 *  @param mother the mass of mother particle 
 *  @param missing the mass of missing particle 
 */
// ============================================================================
LoKi::Kinematics::MissingParticle::MissingParticle
( const LoKi::LorentzVector& p4        , 
  const LoKi::ThreeVector &  direction ,
  const double               mother    , 
  const double               missing   ) 
  //
  : m_p4       ( p4                 ) 
  , m_dir      ( direction.Unit()   ) 
  , m_mother2  ( mother   * mother  ) 
  , m_missing2 ( missing  * missing ) 
  , m_v1  ( )  
  , m_v2  ( )  
  , m_det (0) 
{ solve() ; }
// ============================================================================
/*  constructor from all arguments 
 *  @param mother the mass of mother particle 
 *  @param missing the mass of missing particle 
 *  @param p4  four momenutm of recotructed system 
 *  @param dir the flight direction of mother particle
 */
// ============================================================================
LoKi::Kinematics::MissingParticle::MissingParticle
( const double               mother    , 
  const double               missing   ,
  const LoKi::LorentzVector& p4        , 
  const LoKi::ThreeVector&   direction )
  //
  : m_p4       ( p4                 )  
  , m_dir      ( direction.Unit()   ) 
  , m_mother2  ( mother   * mother  ) 
  , m_missing2 ( missing  * missing ) 
  , m_v1  ( )  
  , m_v2  ( )  
  , m_det (0) 
{ solve() ; }
// ============================================================================
// get number of solutions (0,1 or 2) 
// ============================================================================
unsigned short 
LoKi::Kinematics::MissingParticle::nSolutions () const 
{
  return 
    s_zero ( m_det ) ? 1 :
    m_det > 0        ? 2 : 0 ;
}
// ============================================================================
// solve the system 
// ============================================================================
double LoKi::Kinematics::MissingParticle::solve () // solve the system 
{
  //
  // useful constants:
  //
  const double m2_B  = m_mother2  ;
  const double m2_nu = m_missing2 ;
  //
  // reconstructed system: 
  //
  const double m2_R  = m_p4.M2    () ;  // mass sqyared 
  const double e_R   = m_p4.E     () ;  // energy 
  const double e2_R  = e_R * e_R     ;  // energy squared 
  //
  // 3-vector of recontructed system: 
  //
  const LoKi::ThreeVector p3 ( m_p4.Px() , m_p4.Py() , m_p4.Pz() ) ;
  //
  // decompose recontructed system into parallel and perpendicular parts 
  // 
  const LoKi::ThreeVector p_para = LoKi::Kinematics::parallel   ( p3 , m_dir ) ;
  const LoKi::ThreeVector p_perp = LoKi::Kinematics::transverse ( p3 , m_dir ) ;
  //
  std::cout << "compopnents :"
            << " PARA: " << p_para 
            << " PEPP: " << p_perp << std::endl ;
  
  const double   p2_para = p_para.Mag2() ; // squared magnitude 
  const double   p2_perp = p_perp.Mag2() ; // squared magnitude  
  //
  // Missing system "nu" ( "alpha" is unknown parameter to be solved) 
  //
  //    p_nu_para = alpha * p_para 
  //    p_nu_perp =       - p_perp
  //
  //    e_nu^2    = p_nu_para^2 + p_nu_perp^2 + m_nu^2 
  //
  //    e_nu^2    = alpha^2 * p_para^2 + p_perp^2 + m_nu^2 
  //
  // The total system  ( R + nu ) : 
  //
  //    p_tot   = p_para + p_nu_para = p_para * ( 1 + alpha ) 
  //    e_tot   = e_R    + e_nu 
  //    m_tot   = m_B    
  //
  // The equation for alpha is :
  //  
  //  e_tot^2                     = p_tot^2                + m_tot^2 
  // (e_R + e_nu)^2               = p_tot^2                + m_B^2 
  // (e_R + e_nu)^2               = p_para^2*(1+alpha)^2   + m_B^2 
  //  e_R^2 + 2e_R*e_nu + e_nu^2) = p_para^2*(1+alpha)^2   + m_B^2
  //   2*e_R*e_nu = 2*alpha*p_para^2 + ( m_B^2 - m_nu^2 - m_R^2 - 2*p_perp^2 )
  //   2*e_R*e_nu = 2*alpha*p_para^2 + ( m_B^2 - m_nu^2 - m_R^2 - 2*p_perp^2 )
  //   2*e_R*e_nu = 2*alpha*p_para^2 + C0 
  const double C0 = m2_B - m2_nu - m2_R - 2*p2_perp ;
  //    e_nu/e_R  =  alpha*p_para^2/e_R^2  + 0.5 * ( C0 ) / e_R^2
  //    e_nu/e_R  =  alpha*C1 + C2  
  const double C1 = p2_para  / e2_R ;
  const double C2 = 0.5 * C0 / e2_R ;
  //
  // square the   equation 
  // (alpha^2*p_para^2+p_perp^2+m_nu^2)/e_R^2 = ( alpha*C1 + C2 )^2 
  //
  // and the final form of quadratic equation:
  // alpha^2*( p_para^2/e_R^2-C1*C1) - 2 * alpha *  C1 * C2 + (p_perp^2+p_nu^2)/e_R2 - C2^2 = 0 
  // 
  const double A = p2_para/e2_R - C1 * C1 ;
  const double B = -2 * C1 * C2 ;
  const double C = ( p2_perp + m2_nu ) / e2_R - C2*C2 ;
  //
  std::cout 
    << " p2_para:"      << p2_para 
    << " e2_R:"         << e2_R   
    << " p2_para/e2_R:" << p2_para/e2_R << std::endl ;
  
  // valid LINEAR equation:
  std::cout 
    << " A=" << A 
    << " B=" << B
    << " C=" << C << std::endl ;
  //
  if ( s_zero ( A ) ) 
  {
    m_det = -1 ;
    if ( !s_zero ( B ) ) { return m_det ; }  // RETURN 
    //
    m_det = 0 ;
    const double a1    = C / B ;
    const double p2_nu = p2_perp + a1*a1*p2_para ;
    const double e2_nu = p2_nu   + m2_nu ;
    m_v1 = LoKi::Kinematics::fourMomentum ( p_para * a1 - p_perp , std::sqrt ( e2_nu ) ) ;
    m_v2 = m_v1 ;
    //
    return m_det ;
  }
  //
  // determinant: 
  //
  m_det = B * B - 4 * A * C ;
  //
  std::cout 
    << " D=" << m_det << std::endl ;
  // no solutions or degenerated solutions:
  //
  if ( m_det < 0 || s_zero ( m_det ) ) 
  {
    const double a1    = -0.5 * B / A ;
    const double p2_nu = p2_perp + a1*a1*p2_para ;
    const double e2_nu = p2_nu   + m2_nu ;
    m_v1 = LoKi::Kinematics::fourMomentum ( p_para * a1 - p_perp , std::sqrt ( e2_nu ) ) ;
    m_v2 = m_v1 ;
    //
    return m_det ;
  }
  //
  // regular case: 2 solution 
  //
  const double d  = std::sqrt ( m_det )  ;
  // 1st solution 
  const double a1 = 0.5 * ( -B + d ) / A ;
  const double p2_nu_1 = p2_perp + a1 * a1 * p2_para ;
  const double e2_nu_1 = p2_nu_1 + m2_nu ;
  m_v1 = LoKi::Kinematics::fourMomentum ( p_para * a1 - p_perp , std::sqrt ( e2_nu_1 ) ) ;
  // 2nd solution 
  const double a2 = 0.5 * ( -B - d ) / A ;
  const double p2_nu_2 = p2_perp + a2 * a2 * p2_para ;
  const double e2_nu_2 = p2_nu_2 + m2_nu ;
  m_v2 = LoKi::Kinematics::fourMomentum ( p_para * a2 - p_perp , std::sqrt ( e2_nu_2 ) ) ;
  //
  return m_det ;
}
// ============================================================================
// The END 
// ============================================================================
