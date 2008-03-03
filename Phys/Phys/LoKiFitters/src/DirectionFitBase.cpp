// $Id: DirectionFitBase.cpp,v 1.4 2008-03-03 10:39:20 ibelyaev Exp $
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
  , m_max_iter   ( 20     ) 
  , m_delta_chi2 ( 0.01   ) 
  , m_delta_ctau ( 0.001 * Gaudi::Units::micrometer )
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
  // calculate V_D =  (D*V*D^T)^{-1} & neglect here the errors in momentum 
  m_vd = primary -> covMatrix () + decay  -> covMatrix() ;
  if ( !m_vd.Invert() ) 
  {
    Warning ( "Matrix inversion failure for fast c*tau0 evaluation" , 
              StatusCode ( MatrixInversionFailure , true ) ) ;
    return 0.0 ;
  }
  
  const Gaudi::LorentzVector& mom = particle->momentum() ;
  
  // E = d(H)/d(ctau) = vec(p)/m 
  Gaudi::Math::geo2LA ( mom , m_e ) ; // copy 3 spatial components into LA-vector
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
  
  const Gaudi::LorentzVector& mom = particle -> momentum () ;
  
  const double mass  = mom.M () ;
  
  // E = dH/d(ctau)= vec(p)/m 
  Gaudi::Math::geo2LA ( mom , m_e ) ; // get the spatial components 
  m_e     /= mass      ;  
  
  const Gaudi::XYZPoint&      pp  = primary  -> position () ;
  const Gaudi::XYZPoint&      pd  = decay    -> position () ;
  
  // d = H(alpha_0, ctau_0)
  Gaudi::Math::geo2LA ( pp - pd , m_d ) ;
  m_d += ctau * m_e ;  
  
  // W = d(H)/d(p_mu)
  wmatrix ( mom , ctau , m_w ) ; 
  
  // V_D =  (D*V*D^T)^{-1}
  m_vd
    = ROOT::Math::Similarity ( m_w , particle -> momCovMatrix () ) 
    + decay    -> covMatrix () 
    + primary  -> covMatrix () ;
  Gaudi::Math::update ( m_vd , m_w * particle -> posMomCovMatrix () , -1.0 ) ;
  if ( !m_vd.Invert() ) 
  { return Error ( "The matrix inversion failure" , 
                   StatusCode ( MatrixInversionFailure , true ) ) ; }
  
  // lambda_0 = V_D*(D*alpha0+d)
  m_l0  = m_vd * m_d ; 
  
  // V_ct = (E^T*V_D*E)^{-1}
  const double V_ct = 1.0 / ROOT::Math::Similarity ( m_vd , m_e ) ; 
  
  if ( 0 >= V_ct ) 
  { return Error ( "The Illegal value for V_{ctau} is found!") ; }
  
  // delta(ctau) = -V_ct * E^T * Lam0
  const double dctau = - V_ct * ROOT::Math::Dot ( m_e , m_l0 ) ;
  
  // auxillary matrices DV = ( D1 , D2 ,  D_3 = V_{x_p} )  
  
  // D_1 = V_p*W^T-V_px  
  m_d1 = particle->momCovMatrix() * ROOT::Math::Transpose ( m_w )  
    - particle->posMomCovMatrix()  ;
  
  // D_2 = V_px^T*W^T - V_x 
  m_d2 = ROOT::Math::Transpose ( m_w * particle -> posMomCovMatrix () )  
    - decay -> covMatrix () ;
  
  // D_3 = V_{pv}
  const Gaudi::SymMatrix3x3& m_d3 = primary->covMatrix () ;
  
  // lambda = (lambda0 + V_D * E * delta(ctau))
  m_l   = m_l0 + m_vd * m_e * dctau ;
  
  // delta(alpha) = -V*D^T*Lambda
  m_dp  = -m_d1 * m_l ; // ?
  m_dxd = -m_d2 * m_l ; // ? 
  m_dxp = -m_d3 * m_l ; // ? 
  
  // new chi2:  chi2 = lambda^T( D delta(alpha0) + d ) 
  const double chi2_new = ROOT::Math::Dot ( m_l , m_d ) ;

  
  ctau += dctau         ; // clear ? 
  chi2  = chi2_new      ; // clear ?
  error = sqrt ( V_ct ) ; // clear ?

  
  // start to modify the input values 
  
  // new momentum 
  particle     -> setMomentum       ( particle -> momentum() + m_dp  ) ;
  
  // new positionof the decay vertex 
  particle     -> setReferencePoint ( pd                     + m_dxd ) ;
  decay        -> setPosition       ( pd                     + m_dxd ) ;
  
  // new position of the primary vertex 
  primary      -> setPosition       ( pp                     + m_dxp ) ;
  
  // evaluate the correspondiung covariance matrices 
  
  m_vd1 = -m_vd ;
  Gaudi::Math::update ( m_vd1 , m_e * m_vd , V_ct ) ;
  
  // some dirty trick (hmm. lets compiler to decide... ) 
  
  // update the momentum covarinace matrix 
  const_cast<Gaudi::SymMatrix4x4&> ( particle -> momCovMatrix() )
    += ROOT::Math::Similarity ( m_d1 , m_vd1 )  ;
  
  // update the decay vertex covariance matrix 
  const_cast<Gaudi::SymMatrix3x3&> ( decay   -> covMatrix () ) 
    += ROOT::Math::Similarity ( m_d2 , m_vd1 ) ;
  
  // update the momentum-position covariance matrix 
  const_cast<Gaudi::Matrix4x3&>    ( particle -> posMomCovMatrix() ) 
    += m_d1 * m_vd1 * ROOT::Math::Transpose ( m_d2 ) ;
  
  // update the primary vertex covariance matrix 
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
  error = -1.e+10 * Gaudi::Units::mm ;
  chi2  = -1.e+10                    ;
  
  // make the actual iterations:
  for ( unsigned int iIter = 1 ; iIter <= m_max_iter ; ++iIter ) 
  {
    const double o_chi2 = chi2 ;
    const double o_ctau = ctau ;
    
    // make one iteration step 
    StatusCode sc = step ( primary  , particle , decay , ctau , error , chi2 ) ;
    if ( sc.isFailure() ) { ctau = 0  ; continue  ; }               // CONTINUE
    
    // check for the convergency: by the change in chi2 
    //  *OR* by in the proper lifetime  
    if ( ::fabs ( chi2 - o_chi2 ) < m_delta_chi2 
         || 
         ::fabs ( ctau - o_ctau ) < m_delta_ctau 
         )
    { 
      counter ( "#iterations" ) += iIter ;
      return StatusCode::SUCCESS ;                         // RETURN 
    }
  }
  counter ( "chi2" ) += chi2 ;
  counter ( "ctau" ) += ctau ;
  return Error ( "There is no convergency" , NoConvergency ) ;
}
// ========================================================================
/* calculate w-matrix: W = d(H)/d(p_mu)
 *
 *  It is the only non-trivial component of the general matrix D:
 *   \f$ D = d(H)/d(alpha) = ( W , -1 , 1 ) \f$ 
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
  const double m2 = v.M2 () ;
  const double m  = v.M  () ;
  //
  // W = d(H)/d(p_mu)
  //
  const double p_x = v.Px () ;
  const double p_y = v.Py () ;
  const double p_z = v.Pz () ;
  const double e   = v.E  () ;
  //
  w ( 0 , 0 ) = ( 1.0 + p_x * p_x / m2 ) ; // dHx/dpx 
  w ( 0 , 1 ) = (       p_x * p_y / m2 ) ; // dHx/dpy 
  w ( 0 , 2 ) = (       p_x * p_z / m2 ) ; // dHx/dpz 
  w ( 1 , 0 ) =  m_w(0,1) ;                            // dHy/dpx 
  w ( 1 , 1 ) = ( 1.0 + p_y * p_y / m2 ) ; // dHy/dpy 
  w ( 1 , 2 ) = (       p_y * p_z / m2 ) ; // dHy/dpz 
  w ( 2 , 0 ) =  w(0,2) ;                              // dHz/dpx 
  w ( 2 , 1 ) =  w(1,2) ;                              // dHz/dpy 
  w ( 2 , 2 ) = ( 1.0 + p_z * p_z / m2 ) ; // dHz/dpz
  
  w ( 0 , 3 ) = (     - p_x * e   / m2 ) ; // dHx/de 
  w ( 1 , 3 ) = (     - p_y * e   / m2 ) ; // dHy/de 
  w ( 2 , 3 ) = (     - p_z * e   / m2 ) ; // dHz/de 
  //
  w *= ctau / m ;
  //
}
// ========================================================================


// ============================================================================
// The END 
// ============================================================================

