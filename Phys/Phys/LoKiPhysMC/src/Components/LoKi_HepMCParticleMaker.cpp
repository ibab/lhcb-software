// $Id: LoKi_HepMCParticleMaker.cpp,v 1.1 2006-09-26 10:57:25 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/ToolFactory.h" 
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/IParticleMaker.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/Particle.h"
#include "Event/HepMCEvent.h"
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/ILoKiSvc.h"
#include "LoKi/Interface.h"
// ============================================================================
// LoKiGen 
// ============================================================================
#include "LoKi/GenParticleCuts.h" 
#include "LoKi/GenExtract.h" 
// ============================================================================
// Local 
// ============================================================================
#include "DaVinciMCTools/MCParticleMakerBase.h"
// ============================================================================
/** @class LoKi_HepMCParticleMaker LoKi_HepMCParticleMaker.cpp
 *  Simple class to create the LHCb::Particle objects 
 *  ("recontructed particles" from HepMC::GenParticles (generator particles) 
 *  @author Vanya BELYAEV ibelyaev@physcis.syr.edu
 *  @date 2006-09-25
 */
// ============================================================================
class LoKi_HepMCParticleMaker 
  : public         MCParticleMakerBase
  , public virtual  IParticleMaker 
{
  /// friend factory for instantiation 
  friend class ToolFactory<LoKi_HepMCParticleMaker> ;
public:
  /// standard initialization of the tool
  StatusCode initialize() ;
  /// create the particles 
  StatusCode makeParticles 
  ( LHCb::Particle::ConstVector& particles ) ;
public:
  /// use the particle ?
  bool use ( const HepMC::GenParticle* p ) const ;
protected:
  /// standard constructor
  LoKi_HepMCParticleMaker 
  ( const std::string& type   , ///< tool type ??? 
    const std::string& name   , ///< tool name 
    const IInterface*  parent ) 
    //
    : MCParticleMakerBase ( type , name , parent ) 
    //
    , m_addresses ( 1 , LHCb::HepMCEventLocation::Default )
    //
    , m_particles () 
    , m_mothers   () 
    //
    , m_ids  ( LoKi::Cuts::GNONE ) 
    , m_moms ( LoKi::Cuts::GALL  ) 
    // 
    , m_charged    ( LoKi::Cuts::GALL ) 
    , m_gamma      ( LoKi::Cuts::GALL ) 
    , m_chargedcut ( LoKi::Cuts::GALL ) 
    , m_gammacut   ( LoKi::Cuts::GALL ) 
    //
    , m_minPtGamma      ( 150 * Gaudi::Units::MeV ) 
    , m_minThetaGamma   ( 32.0 / 1200.0 ) 
    , m_maxThetaXGamma  ( 4.0  / 1200.0 ) 
    , m_maxThetaYGamma  ( 3.0  / 1200.0 ) 
    //
    , m_minPCharged     (   3 * Gaudi::Units::GeV  )
    , m_minPtCharged    (  10 * Gaudi::Units::MeV  )
    , m_minThetaCharged (  10 * Gaudi::Units::mrad ) 
    , m_maxThetaCharged ( 250 * Gaudi::Units::mrad )
    //
    , m_maxZproduction   ( 50 * Gaudi::Units::cm ) 
    , m_maxRhoProduction (  5 * Gaudi::Units::cm ) 
    , m_minZend          ( 50 * Gaudi::Units::cm ) 
  {
    //
    declareInterface<IParticleMaker>(this);
    //
    declareProperty ( "Inputs"    , m_addresses ) ;    
    //
    m_particles.push_back ( "e+"  ) ;
    m_particles.push_back ( "mu+" ) ;
    m_particles.push_back ( "pi+" ) ;
    m_particles.push_back ( "K+"  ) ;
    m_particles.push_back ( "p+"  ) ;
    
    declareProperty ( "Particles" , m_particles ) ;
    declareProperty ( "From"      , m_mothers   ) ;
    //
    declareProperty ( "MinPtGamma"      , m_minPtGamma      ) ;
    declareProperty ( "MinThetaGamma"   , m_minThetaGamma   ) ;
    declareProperty ( "MaxThetaXGamma"  , m_maxThetaXGamma  ) ;
    declareProperty ( "MaxThetaYGamma"  , m_maxThetaYGamma  ) ;
    //
    declareProperty ( "MinPCharged"     , m_minPCharged     ) ;
    declareProperty ( "MinPtCharged"    , m_minPtCharged    ) ;
    declareProperty ( "MinThetaCharged" , m_minThetaCharged ) ;
    declareProperty ( "MaxThetaCharged" , m_maxThetaCharged ) ;
    //
    declareProperty ( "MaxZProduction"   , m_maxZproduction   ) ;
    declareProperty ( "MaxRhoProduction" , m_maxRhoProduction ) ;
    declareProperty ( "MinZend"          , m_minZend          ) ;
  } ;
  /// virtual protected destructor 
  virtual ~LoKi_HepMCParticleMaker() {} ;
private:
  // default constructor is disabled 
  LoKi_HepMCParticleMaker() ;
  // copy constructor is disabled 
  LoKi_HepMCParticleMaker( const LoKi_HepMCParticleMaker& ) ;
  // assignement operator is disabled 
  LoKi_HepMCParticleMaker& operator=( const LoKi_HepMCParticleMaker& ) ;
protected:
  /// fill particle from generator particle 
  StatusCode fillParticle 
  ( const HepMC::GenParticle* gen ,
    LHCb::Particle*           rcp ) ;
private:
  typedef std::vector<std::string> Addresses ;
  Addresses           m_addresses  ;
  Gaudi::SymMatrix7x7 m_covariance ;
  typedef std::vector<std::string> IDs     ;
  IDs                 m_particles  ;
  IDs                 m_mothers    ;
  //
  LoKi::Types::GCut   m_ids        ;
  LoKi::Types::GCut   m_moms       ;
  //
  LoKi::Types::GCut   m_charged    ;
  LoKi::Types::GCut   m_gamma      ;
  LoKi::Types::GCut   m_chargedcut ;
  LoKi::Types::GCut   m_gammacut   ;
  //
  double              m_minPtGamma      ;
  double              m_minThetaGamma   ;
  double              m_maxThetaXGamma  ;
  double              m_maxThetaYGamma  ;
  //
  double              m_minPCharged     ;
  double              m_minPtCharged    ;
  double              m_minThetaCharged ;
  double              m_maxThetaCharged ;
  //
  double              m_maxZproduction    ;
  double              m_maxRhoProduction  ;
  double              m_minZend           ;
};
// ============================================================================
/// Declaration of the Tool Factory
// ===========================================================================
DECLARE_TOOL_FACTORY(LoKi_HepMCParticleMaker);
// ============================================================================
/// standard initialization of the tool 
// ============================================================================
StatusCode LoKi_HepMCParticleMaker::initialize() 
{
  StatusCode sc = MCParticleMakerBase::initialize() ;
  if ( sc.isFailure() ) { return sc ; }
  // locate LoKi service (needed for some functions) 
  LoKi::ILoKiSvc* loki = svc<LoKi::ILoKiSvc>( "LoKiSvc" , true ) ;
  if ( 0 == loki ) { return StatusCode::FAILURE ; }
  //
  if ( m_particles.empty() ) 
  { return Error ( "Empty list of particles!" ) ; }
  //
  using namespace LoKi::Cuts ;
  //
  m_ids = m_particles[0] == GABSID ;
  for ( IDs::const_iterator id = m_particles.begin()+1 ; m_particles.end() != id ; ++id ) 
  { m_ids = m_ids || (*id)== GABSID ; }
  //
  info ()
    << " Particles to be selected: " 
    << Gaudi::Utils::toString ( m_particles ) 
    << " Cut to be used " << m_ids << endreq ;  
  //
  if ( !m_mothers.empty() ) 
  {
    m_moms = m_mothers[0] == GID ;
    for ( IDs::const_iterator id = m_mothers.begin()+1 ; m_mothers.end() != id ; ++id ) 
    { m_moms = m_moms || (*id)== GID ; }
  }
  
  m_gamma = "gamma" == GID ;
  
  m_gammacut = GALL ;
  if ( 0 <  m_minPtGamma    ) 
  { m_gammacut = m_gammacut && m_minPtGamma < GPT ; }
  if ( 0 <  m_minThetaGamma ) 
  {
    m_gammacut = m_gammacut && 
      abs(GPX/GPZ) > m_minThetaGamma && 
      abs(GPY/GPZ) > m_minThetaGamma ;  
  }
  if ( 0 <  m_maxThetaXGamma ) 
  { m_gammacut = m_gammacut && abs(GPX/GPZ) < m_maxThetaXGamma ; }
  if ( 0 <  m_maxThetaYGamma ) 
  { m_gammacut = m_gammacut && abs(GPY/GPZ) < m_maxThetaYGamma ; }

  m_charged = 
    ( "e+"  == GABSID || "mu+" == GABSID || 
      "pi+" == GABSID || "K+"  == GABSID || "p+" == GABSID ) ;
  
  m_chargedcut = GALL ;  
  if ( 0 < m_minPCharged ) 
  {  m_chargedcut = m_chargedcut &&  m_minPCharged     < GP      ; }
  if ( 0 < m_minPtCharged ) 
  {  m_chargedcut = m_chargedcut &&  m_minPtCharged    < GPT     ; }
  if ( 0 < m_minThetaCharged ) 
  {  m_chargedcut = m_chargedcut &&  m_minThetaCharged < GTHETA  ; }
  if ( 0 < m_maxThetaCharged ) 
  {  m_chargedcut = m_chargedcut &&  m_maxThetaCharged > GTHETA  ; }
  
  info()
    << " Gammas: "           << m_gamma   
    << " Cuts for gammas: "  << m_gammacut   
    << " Charged: "          << m_charged 
    << " Cuts for charged: " << m_chargedcut 
    << endreq ;
  
  return StatusCode::SUCCESS ;
} ;
// ============================================================================
/// INTERMEZZO 
// ============================================================================
namespace 
{
  class UseIt: 
    public LoKi::Predicate<const HepMC::GenParticle*>
  {
  public:
    /// constructor
    UseIt( const LoKi_HepMCParticleMaker* maker ) 
      : LoKi::Predicate<const HepMC::GenParticle*>() 
      , m_maker ( maker ) 
    {}
    /// copy constructor
    UseIt ( const UseIt& right )
      : LoKi::Predicate<const HepMC::GenParticle*>( right ) 
        , m_maker ( right.m_maker ) 
    {}
    /// virtual destructor 
    virtual ~UseIt(){}
    /// MANDATORY: clone method ("")
    virtual UseIt* clone() const { return new UseIt(*this) ; }
    /// MANATORY: the only one essential method 
    virtual result_type operator() ( argument p ) const 
    { return m_maker->use ( p ) ; }
    virtual std::ostream& fillStream( std::ostream& s ) const 
    { return s << "GUSED" ; }
  private:
    UseIt();
  private:
    LoKi::Interface<LoKi_HepMCParticleMaker> m_maker ;
  } ;
}
// ============================================================================
/// create the particles 
// ============================================================================
StatusCode LoKi_HepMCParticleMaker::makeParticles 
( LHCb::Particle::ConstVector& particles ) 
{
  // avoid long names 
  using namespace LoKi        ;
  using namespace LoKi::Types ;
  using namespace LoKi::Cuts  ; 
  
  const size_t N0 = particles.size() ;

  typedef LoKi::GenTypes::GenContainer Container ;
  
  GCut cut = m_ids && UseIt( this ) ;
  
  if ( !m_mothers.empty() ) 
  {
    Container moms ;
    moms.reserve ( 500 ) ;
    for ( Addresses::const_iterator iaddr = m_addresses.begin() ; 
          m_addresses.end() != iaddr ; ++iaddr ) 
    {
      const LHCb::HepMCEvent::Container* events =
        get<LHCb::HepMCEvent::Container> ( *iaddr ) ;
      /// copy good particles 
      LoKi::Extract::genParticles 
        ( events , std::back_inserter( moms ) , m_moms ) ;
    }
    if ( moms.empty() ) 
    { Warning ( "No valid mother particles are selected, empty result is expected") ;}
    cut = cut && GFROMTREE( moms.begin() , moms.end() ) ;
  }

  // collect "good" particles into the container 
  Container good ;
  good.reserve ( 500 ) ;
  {
    for ( Addresses::const_iterator iaddr = m_addresses.begin() ; 
          m_addresses.end() != iaddr ; ++iaddr ) 
    {
      const LHCb::HepMCEvent::Container* events =
        get<LHCb::HepMCEvent::Container> ( *iaddr ) ;
      /// copy good particles 
      LoKi::Extract::genParticles 
        ( events , std::back_inserter( good ) , cut ) ;
    }  
  }
  // convert good generator particles into recontructed particles 
  particles.reserve( particles.size() + good.size() ) ;
  for ( Container::const_iterator ip = good.begin() ; good.end() != ip ; ++ip ) 
  {
    const HepMC::GenParticle* p = *ip ;
    if ( 0 == p ) { continue ; };
    // create new particle 
    LHCb::Particle* particle = new LHCb::Particle( p->barcode() ) ;
    // fill it 
    StatusCode sc = fillParticle ( p , particle ) ;
    if ( sc.isFailure() ) 
    { 
      Warning("Error from fillParticle, skip it") ;
      delete particle ;
      continue ;                                               // CONTINUE 
    }
    // add to the container 
    particles.push_back ( particle ) ;
  }
  
  if ( statPrint() || msgLevel ( MSG::DEBUG ) ) 
  {
    counter("#particles") += particles.size() - N0 ;
  }
  return StatusCode::SUCCESS ;
} ;
// ============================================================================
StatusCode LoKi_HepMCParticleMaker::fillParticle 
( const HepMC::GenParticle* gen ,
  LHCb::Particle*           rcp ) 
{
  if ( 0 == gen ) { return Error ( "HepMC::GenParticle* points to NULL" ) ; }
  if ( 0 == rcp ) { return Error ( "LHCb::Particle*     points to NULL" ) ; }
  Gaudi::LorentzVector mom ( gen->momentum() ) ;
  StatusCode sc = generateCovariance ( mom , m_covariance ) ;
  if ( sc.isFailure() ) 
  { return Error ( "Error form generate covariance" , sc ) ; }
  Gaudi::XYZPoint point ( 0 , 0 , 0 ) ;
  const HepMC::GenVertex* v = gen->production_vertex() ;
  if ( 0 != v ) { point = Gaudi::XYZPoint( v -> point3d() ) ; }
  else 
  { Warning ( "HepMC::GenVertex* points to NULL, use (0,0,0) instead" ) ; }
  //
  return MCParticleMakerBase::fillParticle 
    ( mom , point ,  LHCb::ParticleID( gen->pdg_id() ) , m_covariance , *rcp ) ;
}
// ============================================================================
/// use the particle?
// ============================================================================
bool LoKi_HepMCParticleMaker::use ( const HepMC::GenParticle* p ) const 
{
  using namespace LoKi::Cuts  ;
  using namespace LoKi::Types ;
  
  if ( 0 == p ) { return false ; }
  
  // check the origin cylinder  
  const HepMC::GenVertex* v0 = p->production_vertex() ;
  if ( 0 == v0 ) 
  {
    Warning("Production vertex is NULL, skip it") ;
    return false ;                                         // RETURN 
  }
  const Gaudi::XYZPoint p0 (  v0->point3d() ) ;
  // check for production vertex 
  if ( 0 < m_maxZproduction   && fabs( p0.Z() ) > m_maxZproduction    ) { return false ; }
  if ( 0 < m_maxRhoProduction && p0.Rho()       > m_maxRhoProduction  ) { return false ; }
  
  // check for decay vertex 
  const HepMC::GenVertex* ve = p->end_vertex() ;
  if ( 0 != ve ) 
  {
    const Gaudi::XYZPoint pe ( ve->point3d() ) ;
    if ( 0 < m_minZend  && pe.Z() < m_minZend ) { return false ; }  
  }
  
  if      ( m_gamma   ( p ) ) { return m_gammacut   ( p ) ; }
  else if ( m_charged ( p ) ) { return m_chargedcut ( p ) ; }

  // other particles 
  return true ;
} ;
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
