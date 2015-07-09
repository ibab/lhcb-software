// $Id$
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IAlgorithm.h"
#include "GaudiKernel/SmartIF.h"
// ============================================================================
// PartProp
// ============================================================================
#include "Kernel/ParticleProperty.h"
#include "Kernel/ParticleID.h"
#include "Kernel/IParticlePropertySvc.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/IDecay.h"
#include "LoKi/Trees.h"
// ============================================================================
// Local
// ============================================================================
#include "ParticleClassificator.h"
// ============================================================================
/** @file
 *  Implementation file for class  LoKi::ParticleClassificator
 *  @see LoKi::ParticleClassificator
 *
 *  This file is a part of
 *  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
 *  ``C++ ToolKit for Smart and Friendly Physics Analysis''
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas,
 *  contributions and advices from G.Raven, J.van Tilburg,
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  By usage of this code one clearly states the disagreement
 *    with the campain of Dr.O.Callot et al.:
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *
 *  @author Vanya Belyaev Ivan.Belyaev@cern.ch
 *  @date   2010-11-11
 *
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
// ============================================================================
// standard constructor
// ============================================================================
LoKi::ParticleClassificator::ParticleClassificator
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : base_class ( type , name , parent )
    /// particle classification:
  , m_ppSvc               (  0      )
  , m_longLived           (         )
  , m_shortLived          (         )
  , m_gammaLike           ( "gamma" )
  , m_gammaCLike          ( Decays::Trees::Invalid_<const LHCb::Particle*>() )
  , m_digammaLike         ( Decays::Trees::Invalid_<const LHCb::Particle*>() )
  , m_mergedPi0Like       ( "pi0"   )
  , m_jetsLike            ( Decays::Nodes::Invalid() ) 
    //
  , m_dd_gammaC  ( " gamma -> e+ e-    "  )
  , m_dd_digamma ( " [ ( pi0 -> <gamma> <gamma> ) , ( eta -> <gamma> <gamma> ) , <pi0> ] ")
  , m_dd_jets    ( " CELLjet | CLUSjet "  ) 
    //
  , m_unclassified   ()
  , m_gamma_like     ()
  , m_gammaC_like    ()
  , m_digamma_like   ()
  , m_mergedPi0_like ()
  , m_jets_like      ()
    //
{
  // ==========================================================================
  declareProperty
    ( "GammaCDecays"      ,
      m_dd_gammaC         ,
      "The gammaC-decays"         ) ;
  declareProperty
    ( "DiGammaDecays"     ,
      m_dd_digamma        ,
      "The di-gamma-decays"       ) ;
  // ==========================================================================
}
// ============================================================================
// virtual & protected destrcutor
// ============================================================================
LoKi::ParticleClassificator::~ParticleClassificator(){}
// ============================================================================
// initialize the tool
// ============================================================================
StatusCode LoKi::ParticleClassificator::initialize ()
{
  /// initialize the base
  StatusCode sc = MessagingBase::initialize () ;
  if ( sc.isFailure() ) { return sc ; }                              // RETURN
  //
  svc<IService>( "LoKiSvc" , true ) ;
  //
  // get particle property service
  m_ppSvc = svc<LHCb::IParticlePropertySvc> ( "LHCb::ParticlePropertySvc" , true );
  // validate
  sc = m_longLived.validate      ( m_ppSvc ) ;
  if ( sc.isFailure() )
  { return Error ( "Unable to validate Long-Lived       particles" , sc ) ; }
  sc = m_shortLived.validate     ( m_ppSvc ) ;
  if ( sc.isFailure() )
  { return Error ( "Unable to validate Short-Lived      particles" , sc ) ; }
  sc = m_gammaLike.validate      ( m_ppSvc ) ;
  if ( sc.isFailure() )
  { return Error ( "Unable to validate Gamma-Like       particles" , sc ) ; }
  sc = m_mergedPi0Like.validate  ( m_ppSvc ) ;
  if ( sc.isFailure() )
  { return Error ( "Unable to validate Merged-pi0-Like  particles" , sc ) ; }
  //
  const StatusCode ok = StatusCode( StatusCode::SUCCESS , true ) ;
  //
  // GammaC ?
  //
  if ( !m_dd_gammaC.empty() )
  {
    // construct gamma_c descriptor
    Decays::IDecay* decay = tool<Decays::IDecay> ( "LoKi::Decay/Decays" ) ;
    //
    m_gammaCLike =  decay->tree ( m_dd_gammaC ) ;
    if ( !m_gammaCLike.valid() )
    { return Error ( "Unable to decode Gamma_c: '" + m_dd_gammaC  + "'" ) ; }
    //
    debug () << " Gamma_c  descriptor : " << m_gammaCLike << endmsg ;
    //
    release ( decay ) ;
  }
  else
  {
    // disable gamma_c treatment
    m_gammaCLike = Decays::Trees::Any_<const LHCb::Particle*>() ;
    m_gammaCLike = Decays::Trees::Not_<const LHCb::Particle*>( m_gammaCLike ) ;
    Warning ( "The special treatment of Gamma_c is disabled" , ok ) ;
  }
  //
  // DiGamma  ?
  //
  if ( !m_dd_digamma.empty() )
  { // construct digamma descriptors
    Decays::IDecay* decay = tool<Decays::IDecay> ( "LoKi::Decay/Decays" ) ;
    //
    m_digammaLike =  decay->tree ( m_dd_digamma ) ;
    if ( !m_digammaLike.valid() )
    { return Error ( "Unable to decode DiGamma: '" + m_dd_digamma + "'" ) ; }
    //
    debug () << " Di-Gamma descriptor : " << m_digammaLike << endmsg ;
    //
    release ( decay ) ;
  }
  else
  {
    // disable di-gamma treatment
    m_digammaLike = Decays::Trees::Any_<const LHCb::Particle*>() ;
    m_digammaLike = Decays::Trees::Not_<const LHCb::Particle*>( m_digammaLike ) ;
    Warning ( "The special treatment of DiGamma is disabled" , ok ) ;
  }
  //
  // Jets 
  //
  if ( !m_dd_jets.empty() )
  { // construct digamma descriptors
    Decays::IDecay* decay = tool<Decays::IDecay> ( "LoKi::Decay/Decays" ) ;
    //
    m_jetsLike =  decay->node ( m_dd_jets ) ;
    if ( !m_jetsLike.valid() )
    { return Error ( "Unable to decode JetLike: '" + m_dd_jets + "'" ) ; }
    //
    debug () << " Jets-like descriptor : " << m_jetsLike << endmsg ;
    //
    release ( decay ) ;
  }
  else
  {
    // disable jets-like treatment
    m_jetsLike = Decays::Nodes::Not ( Decays::Nodes::Any() ) ;
    Warning ( "The special treatment of Jets-like objects is disabled" , ok ) ;
  }
  //
  return StatusCode::SUCCESS ;
}

// ========================================================================
// the standard finalization of the tool
// ========================================================================
StatusCode LoKi::ParticleClassificator::finalize()
{
  // statistics:
  if ( msgLevel ( MSG::DEBUG ) || propsPrint () )
  {
    MsgStream& log = propsPrint () ? info() : debug () ;
    if ( !m_shortLived.accepted().empty() )
    {
      log << "Short-Lived      particles : " << std::endl ;
      LHCb::ParticleProperties::printAsTable ( m_shortLived.accepted () , log , m_ppSvc ) ;
      log << endmsg ;
    }
    if ( !m_longLived.accepted().empty() )
    {
      log << "Long-Lived       particles : " << std::endl ;
      LHCb::ParticleProperties::printAsTable ( m_longLived .accepted () , log , m_ppSvc ) ;
      log << endmsg ;
    }
    //
    // Gamma
    if ( !m_gamma_like.empty() )
    {
      log << "Gamma-like       particles : " << m_gammaLike << std::endl ;
      LHCb::ParticleProperties::printAsTable ( m_gamma_like , log , m_ppSvc ) ;
      log << endmsg ;
    }
    // GammaC
    if ( !m_gammaC_like.empty() )
    {
      log << "Gamma-conv-like  particles : " << m_gammaCLike << std::endl ;
      LHCb::ParticleProperties::printAsTable ( m_gammaC_like , log , m_ppSvc ) ;
      log << endmsg ;
    }
    // DiGamma
    if ( !m_digamma_like.empty() )
    {
      log << "DiGamma-like     particles : " << m_digammaLike << std::endl ;
      LHCb::ParticleProperties::printAsTable ( m_digamma_like , log , m_ppSvc ) ;
      log << endmsg ;
    }
    //
    // Merged-pi0
    if ( !m_mergedPi0_like.empty() )
    {
      log << "Merged-pi0-like  particles : " << m_mergedPi0Like << std::endl ;
      LHCb::ParticleProperties::printAsTable ( m_mergedPi0_like , log , m_ppSvc ) ;
      log << endmsg ;
    }
    //
    // jets-like 
    if ( !m_jets_like.empty() )
    {
      log << "Jets-like  particles : " << m_jetsLike << std::endl ;
      LHCb::ParticleProperties::printAsTable ( m_jets_like , log , m_ppSvc ) ;
      log << endmsg ;
    }
    //
  }
  //
  if ( !m_unclassified.empty() )
  {
    MsgStream& log = warning() ;
    log << "Unclassified particles : " << std::endl ;
    LHCb::ParticleProperties::printAsTable ( m_unclassified , log , m_ppSvc ) ;
    log << endmsg ;
  }
  //
  // reset particle property service & functors
  m_ppSvc = 0 ;
  m_shortLived . setService ( m_ppSvc ) ;
  m_longLived  . setService ( m_ppSvc ) ;
  m_gammaCLike   = Decays::Trees::Invalid_<const LHCb::Particle*>() ;
  m_digammaLike  = Decays::Trees::Invalid_<const LHCb::Particle*>() ;
  //
  return MessagingBase::finalize () ;
}
// ============================================================================
// get the particle type
// ============================================================================
LoKi::KalmanFilter::ParticleType
LoKi::ParticleClassificator::particleType_ ( const LHCb::Particle& p ) const
{
  //
  if ( m_jetsLike( p.particleID()  ) )
  {
    m_jets_like.insert   ( p.particleID () ) ;
    return LoKi::KalmanFilter::JetLikeParticle     ;    // RETURN
  }
  else if ( m_gammaCLike  ( &p ) )
  {
    m_gammaC_like.insert ( p.particleID () ) ;
    // ATTENTION! GammaC is *LONG_LIVED_PARTICLE*
    return LoKi::KalmanFilter::LongLivedParticle   ;    // RETURN
  }
  else if ( m_digammaLike ( &p ) )
  {
    m_digamma_like.insert ( p.particleID () ) ;
    return LoKi::KalmanFilter::DiGammaLikeParticle ;    // RETURN
  }
  else if ( m_gammaLike   ( &p ) )
  {
    m_gamma_like.insert   ( p.particleID () ) ;
    return LoKi::KalmanFilter::GammaLikeParticle   ;    // RETURN
  }
  else if ( m_mergedPi0Like( &p ) )
  {
    m_mergedPi0_like.insert   ( p.particleID () ) ;
    return LoKi::KalmanFilter::MergedPi0LikeParticle   ;    // RETURN
  }
  else if ( m_longLived  ( p.particleID () ) )
  { return LoKi::KalmanFilter::LongLivedParticle   ; }  // RETURN
  else if ( m_shortLived ( p.particleID () ) )
  { return LoKi::KalmanFilter::ShortLivedParticle  ; }  // RETURN
  //
  m_unclassified.insert  ( p.particleID() ) ;
  //
  return LoKi::KalmanFilter::UnspecifiedParticle ;
}
// ============================================================================
/// the factory needed for instantiation
DECLARE_NAMESPACE_TOOL_FACTORY ( LoKi , ParticleClassificator )
// ============================================================================
// The END
// ============================================================================
