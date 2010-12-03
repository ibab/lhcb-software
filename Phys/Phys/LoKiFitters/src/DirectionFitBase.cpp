// $Id$
// ============================================================================
// Include files 
// ============================================================================
// local
// ============================================================================
#include "DirectionFitBase.h"
#include "FitterUtils.h"
// ============================================================================
/** @file
 *  The implementation file for class LoKi::DirectionFitBase
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
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2008-02-1
 *  
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 * 
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
  // iterations&convergency:
  , m_max_iter   ( 20    ) 
  , m_delta_chi2 ( 0.001 ) 
  , m_delta_ctau ( 0.01 * Gaudi::Units::micrometer )
  // The name of particle transpoter tool 
  , m_transporterName ( "ParticleTransporter:PUBLIC" )
  // The tarnsported tool itself 
  , m_transporter ( 0 ) // The transporter tool itself 
                       
{
  //
  declareProperty 
    ( "MaxIterations" , m_max_iter  , "Maximal number of iterations "       ) ;
  declareProperty 
    ( "DeltaChi2"    , m_delta_chi2 , "Delta chi2  (convergency criterion)" ) ;
  declareProperty 
    ( "DeltaCTau"    , m_delta_ctau , "Delta c*tau (convergency criterion)" ) ; 
  // 
  declareProperty 
    ( "Transporter"      , m_transporterName     , 
      "The Particle Transporter tool to be used" );
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
( const LHCb::VertexBase& primary  , 
  const LHCb::Particle&   particle , 
  const LHCb::VertexBase& decay    ) const 
{
  double ctau ;
  // (re)use the existing function:
  StatusCode sc = LoKi::Fitters::ctau0 ( primary  , particle , decay , ctau ) ;
  if ( sc.isFailure() ) 
  {
    Warning ( "The error from LoKi::Fitters::ctau0, return 0", sc ) ;
    return 0 ;
  }
  return ctau ;
}
// ============================================================================
// make the iterations 
// ============================================================================
StatusCode LoKi::DirectionFitBase::iterate 
( const LHCb::VertexBase* primary    , 
  const LHCb::Particle*   particle   ,
  const LHCb::VertexBase* decay      , 
  Gaudi::LorentzVector&   momentum   ,
  Gaudi::XYZPoint&        decvertex  , 
  Gaudi::XYZPoint&        primvertex ,
  double&                 ctau       , 
  double&                 error      , 
  double&                 chi2       ) const 
{ 
  // propagate the particle into its own decay vertex 
  StatusCode sc = transport ( particle , decvertex , m_particle ) ;
  if ( sc.isFailure() ) 
  { return Error ( "The error from IParticleTransporter" , sc ) ; }
  
  const LHCb::Particle* good = &m_particle ;  // the properly transported particle

  // make the fast evaluation of the particle lifetime 
  ctau = ctau0 ( *primary  , *good , *decay ) ; 
  m_fitter.m_var = ctau ;
  
  // reset the initial values 
  error = -1.e+10 * Gaudi::Units::mm ;
  chi2  = -1.e+10                    ;
  
  // get the initial expansion point alpha_A = alpha_0 
  momentum   = good     -> momentum       () ;
  decvertex  = good     -> referencePoint () ;
  primvertex = primary  -> position       () ;
  
  // make the actual iterations:
  for ( unsigned int iIter = 1 ; iIter <= m_max_iter ; ++iIter ) 
  {
    const double o_chi2 = chi2 ;
    const double o_ctau = ctau ;
    
    // make one step of the fit:
    sc = LoKi::Fitters::ctau_step 
      ( *primary , *good , momentum , decvertex , primvertex , m_fitter ) ;
    if ( sc.isFailure() ) 
    {
      Warning ( "Error from LoKi::Fitters::ctau_step,reset" , sc ) ;
      counter ( "#reset" ) += 1 ;
      // reset  to the initial expansion point and reiterate 
      momentum   = good     -> momentum       () ;
      decvertex  = good     -> referencePoint () ;
      primvertex = primary  -> position       () ;
      m_fitter.m_var = 0  ; 
      continue  ;                                                 // CONTINUE
    }
    
    // get the parameters from the fitter
    ctau  = m_fitter.m_var  ;
    chi2  = m_fitter.m_chi2 ;
    error = ::sqrt ( m_fitter.m_Vvar ) ;
    
    // check for the convergency: 
    //    - by the change in either chi2 *OR* in the proper lifetime  
    if ( ::fabs ( chi2  - o_chi2 ) < m_delta_chi2 
         || 
         ::fabs ( ctau  - o_ctau ) < m_delta_ctau ) 
    {
        //
      counter ( "#iterations" ) += iIter ;
      return StatusCode::SUCCESS ;                         // RETURN 
    }
  }
  //
  counter ( "chi2" ) += chi2 ;
  counter ( "ctau" ) += ctau ;
  return Error ( "There is no convergency" , NoConvergency ) ;
}
// ============================================================================
// make the real fit 
// ============================================================================
StatusCode LoKi::DirectionFitBase::fitConst_
( const LHCb::VertexBase* primary   , 
  const LHCb::Particle*   particle  , 
  const LHCb::VertexBase* decay     , 
  double&                 ctau      , 
  double&                 error     , 
  double&                 chi2      ) const 
{ 
  
  Gaudi::LorentzVector momentum   = particle -> momentum () ;
  Gaudi::XYZPoint      decvertex  = decay    -> position () ;
  Gaudi::XYZPoint      primvertex = primary  -> position () ;
  
  // make the iterations 
  StatusCode sc = iterate 
    ( primary    , 
      particle   , 
      decay      ,  
      momentum   , // the updated value (output)
      decvertex  , // the updated value (output)
      primvertex , // the updated value (output)
      ctau       , // the updated value (output) 
      error      , 
      chi2       ) ;
  
  if ( sc.isFailure () ) { return Error ("fit_: the error from iterate" , sc ) ; }
  
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
  
  Gaudi::LorentzVector momentum   = particle -> momentum () ;
  Gaudi::XYZPoint      decvertex  = decay    -> position () ;
  Gaudi::XYZPoint      primvertex = primary  -> position () ;
  
  // make the iterations 
  StatusCode sc = iterate 
    ( primary    , 
      particle   , 
      decay      ,  
      momentum   , // the updated value (output)
      decvertex  , // the updated value (output)
      primvertex , // the updated value (output)
      ctau       , // the updated value (output) 
      error      , 
      chi2       ) ;
  
  if ( sc.isFailure () ) 
  { return Error ( "fit_: Input data are not modified" , sc ) ; }
  
  // start to modify the input values 
  
  // particle momentum 
  
  particle -> setMomentum ( momentum  ) ;
  
  // decay vertex position :
  
  particle -> setReferencePoint ( decvertex )  ;
  decay    -> setPosition       ( decvertex )  ;
  
  // primary vertex position:
  
  primary -> setPosition ( primvertex ) ;
  
  // evaluate the corresponding covariance matrices 
  
  m_vd1 = - m_fitter.m_VD ;
  Gaudi::Math::update ( m_vd1 , m_fitter.m_E * m_fitter.m_VD , m_fitter.m_Vvar ) ;
  
  // some dirty trick (hmm. lets compiler to decide... ) 
  
  // update the momentum covariance matrix 
  const_cast<Gaudi::SymMatrix4x4&> ( particle -> momCovMatrix() )
    += ROOT::Math::Similarity ( m_fitter.m_D1 , m_vd1 )  ;
  
  // update the momentum-position covariance matrix 
  const_cast<Gaudi::Matrix4x3&>    ( particle -> posMomCovMatrix() ) 
    += m_fitter.m_D1 * m_vd1 * ROOT::Math::Transpose ( m_fitter.m_D2 ) ;
  
  // update the position covariance matrix for the particle 
  const_cast<Gaudi::SymMatrix3x3&> ( particle -> posCovMatrix () ) 
    += ROOT::Math::Similarity ( m_fitter.m_D2 , m_vd1 ) ;
  
  // update the position covariance matrix for the particle 
  decay -> setCovMatrix ( particle->posCovMatrix () ) ;
  
  const Gaudi::SymMatrix3x3& m_d3 = primary->covMatrix () ;
  
  // update the primary vertex covariance matrix 
  const_cast<Gaudi::SymMatrix3x3&> ( primary -> covMatrix () ) 
    += ROOT::Math::Similarity ( m_d3 , m_vd1 ) ;
  
  return StatusCode::SUCCESS ;
}
// ========================================================================

// ============================================================================
// The END 
// ============================================================================

