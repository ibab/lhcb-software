// $Id$
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Vector4DTypes.h"
#include "GaudiKernel/GenericVectorTypes.h"
#include "GaudiKernel/SymmetricMatrixTypes.h"
#include "GaudiKernel/GenericMatrixTypes.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
#include "Event/VertexBase.h"
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/MatrixUtils.h"
#include "LHCbMath/MatrixTransforms.h"
// ============================================================================
// Local
// ============================================================================
#include "FitterUtils.h"
// ============================================================================
/** @file
 *  The implementation file for the functions from namespace  LoKi::Fitters 
 *
 *  This file is a part of 
 *  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
 *  ``C++ ToolKit for Smart and Friendly Physics Analysis''
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *   By usage of this code one clearly states the disagreement 
 *    with the campain of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-03-07
 *  
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 * 
 */
// ============================================================================
// evaluate W-matrix: W=d(H)/d(p_mu) for "ctau"-fit
// ============================================================================
void LoKi::Fitters::w_ctau
( const Gaudi::LorentzVector&  v    , // particle momentum 
  const double                 ctau , // "the variable" c*tau 
  Gaudi::Matrix3x4&            W    ) // W=d(H)/d(p_mu)
{
  //
  const double m  = v.M  () ;
  const double m2 = v.M2 () ;
  //
  const double p_x = v . Px () ;
  const double p_y = v . Py () ;
  const double p_z = v . Pz () ;
  const double e   = v . E  () ;
  //
  W ( 0 , 0 ) = ( 1.0 + p_x * p_x / m2 ) ; // dHx/dpx 
  W ( 0 , 1 ) = (       p_x * p_y / m2 ) ; // dHx/dpy 
  W ( 0 , 2 ) = (       p_x * p_z / m2 ) ; // dHx/dpz 
  W ( 1 , 0 ) =  W ( 0 , 1)              ; // dHy/dpx 
  W ( 1 , 1 ) = ( 1.0 + p_y * p_y / m2 ) ; // dHy/dpy 
  W ( 1 , 2 ) = (       p_y * p_z / m2 ) ; // dHy/dpz 
  W ( 2 , 0 ) =  W ( 0 , 2 )             ; // dHz/dpx 
  W ( 2 , 1 ) =  W ( 1 , 2 )             ; // dHz/dpy 
  W ( 2 , 2 ) = ( 1.0 + p_z * p_z / m2 ) ; // dHz/dpz
  
  W ( 0 , 3 ) = (     - p_x * e   / m2 ) ; // dHx/de 
  W ( 1 , 3 ) = (     - p_y * e   / m2 ) ; // dHy/de 
  W ( 2 , 3 ) = (     - p_z * e   / m2 ) ; // dHz/de 
  //
  W *= ctau / m ;
  //
}
// ============================================================================
// evaluate W-matrix: W=d(H)/d(p_mu) for "path"-fit
// ============================================================================
void LoKi::Fitters::w_path
( const Gaudi::LorentzVector&  v    ,  // particle momentum 
  const double                 path ,  // "the variable": path 
  Gaudi::Matrix3x4&            W    )  // d(H)/d(p_mu)
{
  //
  const double p_x = v . Px () ;
  const double p_y = v . Py () ;
  const double p_z = v . Pz () ;
  const double p2  = v . P2 () ;
  //
  W ( 0 , 0 ) = ( 1.0 - p_x * p_x / p2 ) ; // dHx/dpx 
  W ( 0 , 1 ) = (     - p_x * p_y / p2 ) ; // dHx/dpy 
  W ( 0 , 2 ) = (     - p_x * p_z / p2 ) ; // dHx/dpz 
  W ( 1 , 0 ) =  W ( 0 , 1)              ; // dHy/dpx 
  W ( 1 , 1 ) = ( 1.0 - p_y * p_y / p2 ) ; // dHy/dpy 
  W ( 1 , 2 ) = (     - p_y * p_z / p2 ) ; // dHy/dpz 
  W ( 2 , 0 ) =  W ( 0 , 2 )             ; // dHz/dpx 
  W ( 2 , 1 ) =  W ( 1 , 2 )             ; // dHz/dpy 
  W ( 2 , 2 ) = ( 1.0 - p_z * p_z / p2 ) ; // dHz/dpz
  
  W ( 0 , 3 ) =  0                       ; // dHx/de 
  W ( 1 , 3 ) =  0                       ; // dHy/de 
  W ( 2 , 3 ) =  0                       ; // dHz/de 
  //
  W *= path / v.P()  ;
  //
}
// ========================================================================
// the simple function which evaluates E=d(H)/d(var) in the case var=c*tau:
// ========================================================================
void LoKi::Fitters::e_ctau 
( const Gaudi::LorentzVector& p , Gaudi::Vector3& E ) 
{
  Gaudi::Math::geo2LA ( p , E ) ; // copy the spatial components: 
  E /= p.M () ;                   // rescale them  
}
// ========================================================================
// the simple function which evaluates E=d(H)/d(var) in the case var=path:
// ========================================================================
void LoKi::Fitters::e_path 
( const Gaudi::LorentzVector& p , Gaudi::Vector3& E ) 
{
  Gaudi::Math::geo2LA ( p , E ) ; // copy the spatial components: 
  E /= p.P () ;                   // rescale them  
}
// ========================================================================
//  the simple function which evaluates the first&fast evaluatuon of 
//  the constrained scalar variable \f$v\f$using the decay vertex, 
//  primary vertex and the momentum.
// ========================================================================
StatusCode LoKi::Fitters::var0 
( const LHCb::VertexBase&     primary  , 
  const LHCb::VertexBase&     decay    , 
  const Gaudi::LorentzVector& momentum , 
  double&                     var      , 
  LoKi::Fitters::FunE         funE     ) 
{
  // import the useful operators:
  using namespace Gaudi::Math::Operators ;
  
  // calculate V_D =  (D*V*D^T)^{-1} & neglect here the errors in momentum 
  Gaudi::SymMatrix3x3 V_D ( primary.covMatrix() + decay.covMatrix() ) ;
  if ( !V_D.Invert() ) 
  { return StatusCode ( ErrorInMatrixInversion1 , true ) ; } // REUTRN  
  
  // E = d(H)/d(var) = 
  Gaudi::Vector3 E ;
  funE ( momentum , E ) ;
  
  const Gaudi::XYZPoint& pd = decay   . position () ;
  const Gaudi::XYZPoint& pp = primary . position () ;
  
  // lam0 = V_D( D*delta(alpha) + d )  = V_D ( H(0,0) ) 
  const Gaudi::Vector3 lam0 = V_D * ( pp - pd ) ;
  
  // V_v = (E^T*V_D*E)^{-1}
  const double V_v = 1.0 / ROOT::Math::Similarity ( V_D , E ) ;
  
  // delta( var )  =  - V_v * E^T * Lam0 
  var    = -V_v * ROOT::Math::Dot ( E , lam0 ) ;
  
  return StatusCode::SUCCESS ; 
}
// ============================================================================
// make one step of the "var-fit"
// ============================================================================
StatusCode LoKi::Fitters::var_step 
( const LHCb::VertexBase& primary    , // the primary vertex 
  const LHCb::Particle&   particle   , // the properly propagated particle 
  Gaudi::LorentzVector&   momentum   , // the expansion point/updated particle momentum 
  Gaudi::XYZPoint&        decvertex  , // the expansion point/updated decay vertex position 
  Gaudi::XYZPoint&        primvertex , // the expansion point/updated primary vertex position 
  LoKi::Fitters::Fitter1& fit        , // the full results of the fit 
  LoKi::Fitters::FunW     funW       , // the function to evaluate d(H)/d(p^mu)
  LoKi::Fitters::FunE     funE       ) // the function to evaluate d(H)/d(var)
{
  // introduce the helper operators into the game  
  using namespace Gaudi::Math::Operators ;  
  
  // E = dH/d(var)
  funE ( momentum , fit.m_E ) ;
  
  // d = H(alpha_A, ctau_A)  
  fit.m_d = fit.m_var * fit.m_E + ( primvertex - decvertex ) ;
  
  // W = d(H)/d(p_mu)
  funW ( momentum , fit.m_var , fit.m_W ) ; 
  
  // V_D =  (D*V*D^T)^{-1}
  fit.m_VD
    = ROOT::Math::Similarity ( fit.m_W , particle . momCovMatrix () ) 
    + particle . posCovMatrix () 
    + primary  .    covMatrix () ;
  Gaudi::Math::update ( fit.m_VD , fit.m_W * particle . posMomCovMatrix () , -1.0 ) ;
  if ( !fit.m_VD.Invert() ) { return StatusCode ( ErrorInMatrixInversion2 , true ) ; }
  
  fit.m_Da0 
    = fit.m_W * ( particle . momentum       () - momentum   ) 
    -           ( particle . referencePoint () - decvertex  ) 
    +           ( primary  . position       () - primvertex ) ;
  
  // lambda_0 = V_D*(D*alpha0+d)
  fit.m_l0  = fit.m_VD * ( fit.m_Da0 + fit.m_d ) ; 
  
  // V_var = (E^T*V_D*E)^{-1}
  const double V_var = 1.0 / ROOT::Math::Similarity ( fit.m_VD , fit.m_E ) ; 
  fit.m_Vvar =  V_var ;
  
  if ( 0 >= V_var ) { return StatusCode ( NegativeSigma2 , true ) ; }
  
  // delta(var) = -V_var * E^T * Lam0
  const double dvar = - V_var * ROOT::Math::Dot ( fit.m_E , fit.m_l0 ) ;
  
  // auxillary matrices DV = ( D1 , D2 ,  D_3 = V_{x_p} )  
  
  // D_1 = V_p*W^T-V_px  
  fit.m_D1 = particle . momCovMatrix() * ROOT::Math::Transpose ( fit.m_W )  
    - particle . posMomCovMatrix ()  ;
  
  // D_2 = V_px^T*W^T - V_x 
  fit.m_D2 = ROOT::Math::Transpose ( fit.m_W * particle . posMomCovMatrix () )  
    - particle . posCovMatrix    () ;
  
  // D_3 = V_{pv}
  const Gaudi::SymMatrix3x3& m_d3 = primary . covMatrix () ;
  
  // lambda = (lambda0 + V_D * E * delta(ctau))
  fit.m_l   = fit.m_l0 + fit.m_VD * fit.m_E * dvar ;
  
  // delta(alpha)            = - V*D^T    * Lambda
  const Gaudi::Vector4 m_dp  = - fit.m_D1 * fit.m_l ; // ?
  const Gaudi::Vector3 m_dxd = - fit.m_D2 * fit.m_l ; // ? 
  const Gaudi::Vector3 m_dxp = -     m_d3 * fit.m_l ; // ? 
  
  // new chi2:  chi2 = lambda^T( D delta(alpha0) + d ) 
  const double chi2_new = ROOT::Math::Dot ( fit.m_l , fit.m_Da0 + fit.m_d ) ; 
  
  fit.m_var   += dvar     ; // clear ? 
  fit.m_chi2  = chi2_new  ; // clear ?
  
  // start to modify the input values 
  
  //momentum   = particle . momentum       ()  + m_dp  ; // new expansion point 
  /** @todo CRJ : Understand why this is needed with GCC4 / OSX */
  momentum   = particle . momentum(); Gaudi::Math::add( momentum, m_dp );
  decvertex  = particle . referencePoint ()  + m_dxd ; // new expansion point
  primvertex = primary  . position       ()  + m_dxp ; // new expansion point 
  
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// The END 
// ============================================================================
