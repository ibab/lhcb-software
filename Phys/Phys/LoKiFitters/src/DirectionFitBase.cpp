// $Id: DirectionFitBase.cpp,v 1.1.1.1 2008-02-20 15:48:44 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// local
// ============================================================================
#include "DirectionFitBase.h"
// ============================================================================
/** @file
 *  The implementation file for class LoKi::DirectionFitBase
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2008-02-1
 */
// ============================================================================
/* constructor 
 *  @param type the actual tool type (??)
 *  @param name the tool instance name 
 *  @param parent the parent of the tool
     */
// ============================================================================
LoKi::DirectionFitBase::DirectionFitBase
( const std::string& type   , 
  const std::string& name   , 
  const IInterface*  parent ) 
  : GaudiTool ( type , name , parent ) 
    //
  , m_max_iter   ( 10   ) 
  , m_delta_chi2 ( 0.01 ) 
  , m_delta_ctau ( 0.1 * Gaudi::Units::micrometer )
    //
{
  //
  declareProperty 
    ( "MaxIterations" , m_max_iter  , "Maximal number of iterations "       ) ;
  declareProperty 
    ( "DeltaChi2"    , m_delta_chi2 , "Delta chi2  (convergency criterion)" ) ;
  declareProperty 
    ( "DeltaCTau"    , m_delta_ctau , "Delta c*tau (convergency criterion)" ) ; 
} 
// ============================================================================
// virtual & protected destructor
// ============================================================================
LoKi::DirectionFitBase::~DirectionFitBase () {}
// ============================================================================
// make the first fast evaluation of c*tau  
//                    (no iterations and no uncertanties in momentum)
// ============================================================================
double LoKi::DirectionFitBase::ctau0 
( const LHCb::VertexBase* primary  , 
  const LHCb::Particle*   particle , 
  const LHCb::VertexBase* decay    ) const 
{
  // neglect here the errors in momentum ( essentially it merans that we have set  W to ZERO )
  m_vd = primary -> covMatrix () + decay  -> covMatrix() ;
  if ( !m_vd.Invert() ) 
  {
    Warning ( "Matrix inversion failure for fast c*tau0 evaluation" , MatrixInversionFailure ) ;
    return 0.0 ;
  }
  
  const Gaudi::LorentzVector& mom = particle->momentum() ;
  
  // E = d(H)/d(ctau) = vec(p)/m 
  Gaudi::Math::geo2LA ( mom , m_e ) ; ///< copy 3 spatial components into LA-vector
  m_e /= mom.M() ;
  
  const Gaudi::XYZPoint& pd = decay   -> position () ;
  const Gaudi::XYZPoint& pp = primary -> position () ;
  
  // d = H(0,0) 
  Gaudi::Math::geo2LA ( pp - pd , m_d ) ;
  
  // Lam0 = V_D( D*delta(alpha) + d ) 
  m_l0 = m_vd * m_d ;
  
  // V_ct = (E^T*V_D*E)^{-1}
  const double V_ct = 1.0 / ROOT::Math::Similarity ( m_vd , m_e ) ;
  
  // delta( ctau )  =  - V_ct * E^T * Lam0 
  return - V_ct * ROOT::Math::Dot ( m_e , m_l0 ) ;
}
// ============================================================================
// make one fit iteration 
// ============================================================================
StatusCode LoKi::DirectionFitBase::step 
( LHCb::VertexBase* primary   , 
  LHCb::Particle*   particle  , 
  LHCb::VertexBase* decay     , 
  double&           ctau      , 
  double&           error     , 
  double&           chi2      ) const 
{
  // introduce the helper operators into the game  
  using namespace Gaudi::Math::Operators ;  
  
  const Gaudi::XYZPoint&      pp  = primary  -> position () ;
  const Gaudi::XYZPoint&      pd  = decay    -> position () ;
  const Gaudi::LorentzVector& mom = particle -> momentum () ;
  
  const double mass2 = mom.M2()    ;
  const double mass  = ::sqrt ( mass2 ) ;
  
  // E = dH/d(ctau)= vec(p)/m 
  Gaudi::Math::geo2LA ( mom , m_e ) ; ///< get the spatial components 
  m_e     /= mass      ;
  
  // d = H(alpha_0, ctau_0)
  Gaudi::Math::geo2LA ( pp - pd , m_d ) ;
  m_d += ctau * m_e ; 
  
  // W = d(H)/d(p_mu)
  wmatrix ( mom , ctau , m_w ) ;
  
  // V_D =  ... 
  m_vd 
    = ROOT::Math::Similarity ( m_w , particle -> momCovMatrix () ) 
    + decay    -> covMatrix () 
    + primary  -> covMatrix () ;
  Gaudi::Math::update ( m_vd , m_w * particle -> posMomCovMatrix () , -1.0 ) ;
  if ( !m_vd.Invert() ) 
  { return Error ( "The matrix inversion failure" , MatrixInversionFailure ) ; }
  
  m_l0  = m_vd * m_d ;
  
  const double v_ct = 1.0 / ROOT::Math::Similarity ( m_vd , m_e ) ;
  
  // evaluate new time :  delta(ctau) = -V_ct * E^T * Lam0 
  const double dctau = - v_ct * ROOT::Math::Dot ( m_e , m_l0 ) ;
  
  // auxillary matrices 
  m_d1 = m_w * particle->momCovMatrix() - 
    ROOT::Math::Transpose ( particle->posMomCovMatrix() ) ;
  m_d2 = m_w * particle -> posMomCovMatrix () - decay -> covMatrix () ;

  const Gaudi::SymMatrix3x3& m_d3 = primary->covMatrix () ;
  
  // -lambda = -1*(lambda0 + V_D * E * delta(ctau) )  
  m_l   = m_l0 + m_vd * m_e * dctau ;
  
  // delta(alpha) = -V*D^T*Lambda
  //m_dp  = m_l * m_d1 ; // ?
  //m_dxd = m_l * m_d2 ; // ? 
  //m_dxp = m_l * m_d3 ; // ? 
  
  // new chi2:  chi2 = lambda^T( D delta(alpha0) + d ) 
  const double chi2_new = ROOT::Math::Dot ( m_l , m_d ) ;
  
  m_vd1 = -m_vd ;
  Gaudi::Math::update ( m_vd1 , m_e * m_vd , v_ct ) ;
  
  particle     -> setMomentum ( particle -> momentum() + m_dp  ) ;
  
  //particle     -> setRefPoint ( pd                     + m_dxd ) ;
  //decay        -> setPosition ( pd                     + m_dxd ) ;
  //primary      -> setPosition ( pp                     + m_dxp ) ;\
  // particle     -> setMomCovMatrix 
  //  ( particle -> momCovMatrix () + ROOT::Math::Similarity ( m_d1 , m_vd1 ) ) ;
  //  decay        -> setCovMatrix 
  //  ( decay    -> covMatrix    () + ROOT::Math::Similarity ( m_d2 , m_vd1 ) ) ;
  //primary      -> setCovMatrix 
  //  ( primary  -> covMatrix    () + ROOT::Math::Similarity ( m_d3 , m_vd1 ) ) ;
  
  // some trick (dirty? hmm. lets compiler to decide... ) 

  // update the decay vertex 
  const_cast<Gaudi::SymMatrix3x3&> ( decay   -> covMatrix () ) 
    += ROOT::Math::Similarity ( m_d2 , m_vd1 ) ;
  
  // update the primary vertex 
  const_cast<Gaudi::SymMatrix3x3&> ( primary -> covMatrix () ) 
    += ROOT::Math::Similarity ( m_d3 , m_vd1 ) ;

  return StatusCode::SUCCESS ;
}
// ============================================================================
// make the real fit 
// ============================================================================
StatusCode LoKi::DirectionFitBase::fit_
( LHCb::VertexBase* primary   , 
  LHCb::Particle*   particle  , 
  LHCb::VertexBase* decay     , 
  double&           ctau      , 
  double&           error     , 
  double&           chi2      ) const 
{
  // make the fast evaluation of the particle lifetime 
  ctau = ctau0 ( primary  , particle , decay ) ; ///< the fast evaluation of lifetime 
  
  // reset the initial values 
  error = -1.e+10 * Gaudi::Units::nanosecond ;
  chi2  = -1.e+10                            ;
  
  // make the actual iterations:
  for ( unsigned int iIter = 0 ; iIter < m_max_iter ; ++iIter ) 
  {
    const double o_chi2 = chi2 ;
    const double o_ctau = ctau ;
    
    // make one iteration step 
    StatusCode sc = step ( primary  , particle , decay , ctau , error , chi2 ) ;
    if ( sc.isFailure() ) { return sc ; }                            // RETURN
    
    // check for the convergency: by the change in chi2 or by in the proper lifetime  
    if ( ::fabs ( chi2 - o_chi2 ) < m_delta_chi2 
         ||
         ::fabs ( ctau - o_ctau ) < m_delta_ctau )
    { 
      counter ( "#iterations" ) += ( iIter + 1 ) ;
      return StatusCode::SUCCESS ; 
    }
  }
  return Error ( "There is no convergency" , NoConvergency ) ;
}
// ========================================================================
/* calculate w-matrix: W = d(H)/d(p_mu)
 *
 *  It is the only non-trivial component of the general matrix D:
 *   D = d(H)/d(alpha) = ( W , -1 , 1 ) 
 *
 *  @param v    the lorents-vector     (input) 
 *  @param ctau the evaluation of c*tau (input) 
 *  @param w    the w-matrix            (output) 
 */
// ========================================================================
void LoKi::DirectionFitBase::wmatrix 
( const Gaudi::LorentzVector& v    , 
  const double                ctau , 
  Gaudi::Matrix3x4&           w    ) const 
{
  //
  const double m2 = v.M2() ;
  const double m  = ::sqrt( m2 ) ;
  //
  // W = d(H)/d(p_mu)
  //
  w ( 0 , 0 ) = ( 1.0 + v . Px () * v . Px () / m2 ) ; // dHx/dpx 
  w ( 0 , 1 ) = (       v . Px () * v . Py () / m2 ) ; // dHx/dpy 
  w ( 0 , 2 ) = (       v . Px () * v . Pz () / m2 ) ; // dHx/dpz 
  w ( 0 , 3 ) = (     - v . Px () * v . E  () / m2 ) ; // dHx/de 
  w ( 1 , 0 ) =  m_w(0,1) ;                            // dHy/dpx 
  w ( 1 , 1 ) = ( 1.0 + v . Py () * v . Py () / m2 ) ; // dHy/dpy 
  w ( 1 , 2 ) = (       v . Py () * v . Pz () / m2 ) ; // dHy/dpz 
  w ( 1 , 3 ) = (     - v . Py () * v . E  () / m2 ) ; // dHy/de 
  w ( 2 , 0 ) =  w(0,2) ;                              // dHz/dpx 
  w ( 2 , 1 ) =  w(1,2) ;                              // dHz/dpy 
  w ( 2 , 2 ) = ( 1.0 + v . Pz () * v . Pz () / m2 ) ; // dHz/dpz 
  w ( 2 , 3 ) = (     - v . Pz () * v . E  () / m2 ) ; // dHz/de 
  //
  w *= ctau / m ;
}
// ========================================================================


// ============================================================================
// The END 
// ============================================================================

