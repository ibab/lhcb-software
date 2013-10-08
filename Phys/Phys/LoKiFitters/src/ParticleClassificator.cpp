// $Id$
// ============================================================================
// Include files 
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
  : GaudiTool ( type , name , parent )
/// particle classification: 
  , m_ppSvc               (  0      ) 
  , m_longLived           (         ) 
  , m_shortLived          (         ) 
  , m_gammaLike           ( "gamma" )
  , m_gammaCLike          ( Decays::Trees::Invalid_<const LHCb::Particle*>() )
  , m_digammaLike         ( Decays::Trees::Invalid_<const LHCb::Particle*>() )
//
  , m_dd_gammaC  (" gamma -> e+ e- ")
  , m_dd_digamma (" [ ( pi0 -> <gamma> <gamma> ) , ( eta -> <gamma> <gamma> ) ] ")
///
  , m_unclassified  ()
  , m_gamma_like    ()
  , m_gammaC_like   ()
  , m_digamma_like  ()
///
{
  // ==========================================================================
  declareProperty 
    ( "GammaCDecays"  , 
      m_dd_gammaC     , 
      "The gammaC-decays" ) ;
  declareProperty 
    ( "DiGammaDecays" , 
      m_dd_digamma    , 
      "The di-gamma-decays" ) ;
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
  StatusCode sc = GaudiTool::initialize () ; 
  if ( sc.isFailure() ) { return sc ; }                              // RETURN 
  //
  svc<IService>( "LoKiSvc" , true ) ;
  //
  // get particle property service 
  m_ppSvc = svc<LHCb::IParticlePropertySvc> ( "LHCb::ParticlePropertySvc" , true );
  // validate  
  sc = m_longLived.validate  ( m_ppSvc ) ;
  if ( sc.isFailure() ) 
  { return Error ( "Unable to validate Long-Lived  particles" , sc ) ; }
  sc = m_shortLived.validate ( m_ppSvc ) ;
  if ( sc.isFailure() ) 
  { return Error ( "Unable to validate Short-Lived particles" , sc ) ; }
  sc = m_gammaLike.validate  ( m_ppSvc ) ;
  if ( sc.isFailure() ) 
  { return Error ( "Unable to validate Gamma-Like  particles" , sc ) ; }
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
    // disable gamma_c treatment
    m_digammaLike = Decays::Trees::Any_<const LHCb::Particle*>() ;
    m_digammaLike = Decays::Trees::Not_<const LHCb::Particle*>( m_digammaLike ) ;
    Warning ( "The special treatment of DiGamma is disabled" , ok ) ;
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
      log << "Short-Lived particles : " << std::endl ;
      LHCb::ParticleProperties::printAsTable ( m_shortLived.accepted () , log , m_ppSvc ) ;
      log << endmsg ;
    }
    if ( !m_longLived.accepted().empty() ) 
    {
      log << "Long-Lived  particles : " << std::endl ;
      LHCb::ParticleProperties::printAsTable ( m_longLived .accepted () , log , m_ppSvc ) ;
      log << endmsg ;
    }
    //
    // Gamma
    if ( !m_gamma_like.empty() ) 
    {
      log << "Gamma-like   particles : " << m_gammaLike << std::endl ;
      LHCb::ParticleProperties::printAsTable ( m_gamma_like , log , m_ppSvc ) ;
      log << endmsg ;
    }
    // GammaC 
    if ( !m_gammaC_like.empty() ) 
    {
      log << "GammaC-like   particles : " << m_gammaCLike << std::endl ;
      LHCb::ParticleProperties::printAsTable ( m_gammaC_like , log , m_ppSvc ) ;
      log << endmsg ;
    }
    // DiGamma 
    if ( !m_digamma_like.empty() ) 
    {
      log << "DiGamma-like  particles : " << m_digammaLike << std::endl ;
      LHCb::ParticleProperties::printAsTable ( m_digamma_like , log , m_ppSvc ) ;
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
  return GaudiTool::finalize () ;
}
// ============================================================================
// get the particle type 
// ============================================================================
LoKi::KalmanFilter::ParticleType 
LoKi::ParticleClassificator::particleType ( const LHCb::Particle* p ) const 
{
  //
  if ( 0 == p ) { return LoKi::KalmanFilter::UnspecifiedParticle ; }  // RETURN 
  return particleType_ ( *p ) ;
}
// ============================================================================
// get the particle type 
// ============================================================================
LoKi::KalmanFilter::ParticleType 
LoKi::ParticleClassificator::particleType_ ( const LHCb::Particle& p ) const 
{
  //
  if      ( m_gammaCLike  ( &p ) )
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
// good for vertex ? 
// ============================================================================
template <class PARTICLE>
inline
std::size_t LoKi::ParticleClassificator::nForVertex 
( PARTICLE first ,
  PARTICLE last  ) const 
{
  //
  std::size_t nTr = 0 ;
  //
  for ( ; first != last ; ++first ) 
  {
    const LHCb::Particle* p = *first ;
    if ( 0 == p ) { continue ; }
    //
    LoKi::KalmanFilter::ParticleType pType = particleType ( p ) ;
    //
    // 1 for long-lived particles 
    if      ( LoKi::KalmanFilter::LongLivedParticle  == pType )
    { ++nTr ; }
    // number for long-lived particles for short lived resonance 
    else if ( LoKi::KalmanFilter::ShortLivedParticle == pType ) 
    {
      const SmartRefVector<LHCb::Particle>& daughters = p->daughters() ;
      nTr += nForVertex ( daughters.begin() , daughters.end() ) ;
    }
    //
  }  
  return nTr ;  
}
// ============================================================================
// good for vertex ? 
// ============================================================================
bool LoKi::ParticleClassificator::goodForVertex 
( const LHCb::Particle::ConstVector& parts ) const 
{
  /// two or more long-lived particles are required for vertex 
  return 2 <= nForVertex ( parts.begin() , parts.end() ) ; 
}
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
