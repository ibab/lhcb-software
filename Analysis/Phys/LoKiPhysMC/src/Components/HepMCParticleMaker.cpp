// $Id$
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
#include "MCInterfaces/IHepMCParticleSelector.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/Particle.h"
#include "Event/HepMCEvent.h"
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/ILoKiSvc.h"
// ============================================================================
// LoKiGen 
// ============================================================================
#include "LoKi/GenParticleCuts.h" 
#include "LoKi/GenExtract.h" 
// ============================================================================
// Relations
// ============================================================================
#include "Relations/IRelation.h"
#include "Relations/IRelation2D.h"
#include "Relations/Relation2D.h"

// ============================================================================
// LoKiPhysMC
// ============================================================================
#include "Kernel/RC2HepMC.h"
// ============================================================================
// DaVinciMCKernel
// ============================================================================
#include "Kernel/MCParticleMakerBase.h"
//#include "GaudiKernel/IParticlePropertySvc.h"
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"

// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /** @class HepMCParticleMaker
   *
   *  Simple class to create the LHCb::Particle objects 
   *  ("recontructed particles") 
   *  from HepMC::GenParticles ("generator particles") 
   *
   *  @see MCParticleMakerBase 
   *
   *  @attention for default configuration, 
   *    the smearing of 4-momentum and position is disabled 
   *
   *  This file is a part of LoKi project - 
   *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
   *
   *  The package has been designed with the kind help from
   *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
   *  contributions and advices from G.Raven, J.van Tilburg, 
   *  A.Golutvin, P.Koppenburg have been used in the design.
   *
   *
   *  @author Vanya BELYAEV ibelyaev@physcis.syr.edu
   *  @date 2006-09-25
   *  @modifications Victor Coco victor.coco@cern.ch
   *  @date 2009-10-25
   */
  // ==========================================================================
  class HepMCParticleMaker 
    : public         MCParticleMakerBase
    , public virtual  IParticleMaker 
  {
    // ========================================================================
    /// friend factory for instantiation 
    friend class ToolFactory<LoKi::HepMCParticleMaker> ;
    // ========================================================================
  public:
    // ========================================================================
    /// standard initialization of the tool
    StatusCode initialize() ;
    /// create the particles 
    StatusCode makeParticles 
    ( LHCb::Particle::ConstVector& particles ) ;
    // ========================================================================
  public:
    // ========================================================================
    /// use the particle ?
    bool use ( const HepMC::GenParticle* p ) const ;
    // ========================================================================
  protected:
    // ========================================================================
    /// standard constructor
    HepMCParticleMaker 
    ( const std::string& type   , ///< tool type ??? 
      const std::string& name   , ///< tool name 
      const IInterface*  parent ) 
    //
      : MCParticleMakerBase ( type , name , parent ) 
    //
      , m_addresses ( 1 , LHCb::HepMCEventLocation::Default )
    //
      , m_processes ()
      , m_particles () 
      , m_mothers   ()
      , m_useSelector(false)
      
    //
      , m_ids  ( LoKi::Cuts::GNONE ) 
      , m_moms ( LoKi::Cuts::GALL  ) 
      , m_forceNoCutAcceptance ( false )
      , m_excludeFromMother ( false )
      , m_excludeMotherNumber ( 39 )
    // 
      , m_charged    ( LoKi::Cuts::GALL ) 
      , m_gamma      ( LoKi::Cuts::GALL ) 
      , m_chargedcut ( LoKi::Cuts::GALL ) 
      , m_gammacut   ( LoKi::Cuts::GALL ) 
    //
      , m_minPtGamma     (  150.0 * Gaudi::Units::MeV     ) 
      , m_minThetaGamma  ( ( 32.0 * Gaudi::Units::cm    ) / 
                           ( 12.4 * Gaudi::Units::meter ) )
      , m_maxThetaXGamma ( (  4.0 * Gaudi::Units::meter ) / 
			   ( 12.4 * Gaudi::Units::meter ) ) 
      , m_maxThetaYGamma ( (  3.0 * Gaudi::Units::meter ) / 
			   ( 12.4 * Gaudi::Units::meter ) )
    //
      , m_minPCharged     (   3 * Gaudi::Units::GeV  )
      , m_minPtCharged    (  10 * Gaudi::Units::MeV  )
      , m_minThetaCharged (  10 * Gaudi::Units::mrad ) 
      , m_maxThetaCharged ( 250 * Gaudi::Units::mrad )
    //
      , m_maxZproduction   ( 50 * Gaudi::Units::cm ) 
      , m_maxRhoProduction (  5 * Gaudi::Units::cm ) 
      , m_minZend          ( 50 * Gaudi::Units::cm )
      , m_partonicmode   (false)
    //        , m_OutputTable ("Relations/Phys/StdHepMCParticlesRef2HepMC")
    {
      //
      declareInterface<IParticleMaker>(this);
      //
      declareProperty ( "Inputs"           , m_addresses        ) ;    
      //
      m_particles.push_back ( "e+"  ) ;
      m_particles.push_back ( "mu+" ) ;
      m_particles.push_back ( "pi+" ) ;
      m_particles.push_back ( "K+"  ) ;
      m_particles.push_back ( "p+"  ) ;
      declareProperty ( "ExcludeFromMother"        , m_excludeFromMother        ) ;
      declareProperty ( "ExcludeMotherNumber"        , m_excludeMotherNumber        ) ;
      declareProperty ( "Processes"        , m_processes        ) ;
      declareProperty ( "Particles"        , m_particles        ) ;
      declareProperty ( "From"             , m_mothers          ) ;
      declareProperty ( "ForceNoAcceptanceCut"   , m_forceNoCutAcceptance ) ;
      declareProperty ( "UseSelector"    , m_useSelector   ) ;
      declareProperty ( "MotherSelectorName" , m_motherSelector );
      //
      declareProperty ( "MinPtGamma"       , m_minPtGamma       ) ;
      declareProperty ( "MinThetaGamma"    , m_minThetaGamma    ) ;
      declareProperty ( "MaxThetaXGamma"   , m_maxThetaXGamma   ) ;
      declareProperty ( "MaxThetaYGamma"   , m_maxThetaYGamma   ) ;
      //
      declareProperty ( "MinPCharged"      , m_minPCharged      ) ;
      declareProperty ( "MinPtCharged"     , m_minPtCharged     ) ;
      declareProperty ( "MinThetaCharged"  , m_minThetaCharged  ) ;
      declareProperty ( "MaxThetaCharged"  , m_maxThetaCharged  ) ;
      //
      declareProperty ( "MaxZProduction"   , m_maxZproduction   ) ;
      declareProperty ( "MaxRhoProduction" , m_maxRhoProduction ) ;
      declareProperty ( "MinZend"          , m_minZend          ) ;

      declareProperty ( "PartonicMode"     , m_partonicmode ) ;
      
      //      declareProperty ( "OutputTable"          , m_OutputTable         ) ;
      // 
      setProperty     ( "SmearParticle"    , false ) ;
    } 
    /// virtual protected destructor 
    virtual ~HepMCParticleMaker() {} 
    // ========================================================================
  private:
    // ========================================================================
    // default constructor is disabled 
    HepMCParticleMaker() ;
    // copy constructor is disabled 
    HepMCParticleMaker( const HepMCParticleMaker& ) ;
    // assignement operator is disabled 
    HepMCParticleMaker& operator=( const HepMCParticleMaker& ) ;
    // ========================================================================
  protected:
    // ========================================================================
    /// fill particle from generator particle 
    StatusCode fillParticle 
    ( const HepMC::GenParticle* gen ,
      LHCb::Particle*           rcp ) ;
    // ========================================================================
  private:
    // ========================================================================
    typedef std::vector<std::string> Addresses ;
    //
    Addresses           m_addresses         ;
    Gaudi::SymMatrix7x7 m_covariance        ;
    typedef std::vector<std::string> IDs    ;
    typedef std::vector<int> INTs    ;
    INTs                m_processes;
    IDs                 m_particles         ;
    IDs                 m_mothers           ;
    bool                m_useSelector       ;
    std::string         m_motherSelector    ;
    //
    LoKi::Types::GCut   m_ids               ;
    LoKi::Types::GCut   m_moms              ;
    //
    bool    m_forceNoCutAcceptance   ;
    bool    m_excludeFromMother;
    int     m_excludeMotherNumber;
    //std::string         m_OutputTable       ;
    
    LoKi::Types::GCut   m_charged           ;
    LoKi::Types::GCut   m_gamma             ;
    LoKi::Types::GCut   m_chargedcut        ;
    LoKi::Types::GCut   m_gammacut          ;
    //
    double              m_minPtGamma        ;
    double              m_minThetaGamma     ;
    double              m_maxThetaXGamma    ;
    double              m_maxThetaYGamma    ;
    //
    double              m_minPCharged       ;
    double              m_minPtCharged      ;
    double              m_minThetaCharged   ;
    double              m_maxThetaCharged   ;
    //
    double              m_maxZproduction    ;
    double              m_maxRhoProduction  ;
    double              m_minZend           ;

    bool                m_partonicmode            ;

    LHCb::IParticlePropertySvc* m_ppSvc;
    IHepMCParticleSelector*     m_motherSelectorTool;
    //
    // ========================================================================
  };
  // ==========================================================================
} // end of namespace LoKi 


// ============================================================================
// standard initialization of the tool 
// ============================================================================
StatusCode LoKi::HepMCParticleMaker::initialize() 
{
  //
  StatusCode sc = MCParticleMakerBase::initialize() ;

  //m_ppSvc = svc<IParticlePropertySvc>("ParticlePropertySvc", true);
  m_ppSvc = svc<LHCb::IParticlePropertySvc> ("LHCb::ParticlePropertySvc" , true ) ;
  if ( sc.isFailure() ) { return sc ; }

  // locate LoKi service (needed for some functions) 
  LoKi::ILoKiSvc* loki = svc<LoKi::ILoKiSvc>( "LoKiSvc" , true ) ;
  if ( 0 == loki ) { return StatusCode::FAILURE ; }

  // check the status of mother control...
  // if mother should be chosen from ID
  if ( !m_mothers.empty() ) 
    {
      m_moms = m_mothers[0] == LoKi::Cuts::GABSID ;
      for ( IDs::const_iterator id = m_mothers.begin()+1 ; 
	    m_mothers.end() != id ; ++id ) 
	{ 
	  if( m_partonicmode && (  "b" != (*id) &&  "g" != (*id) &&  "u" != (*id) &&  "d" != (*id) &&  "s" != (*id) &&  "c" != (*id)))
	    { return Error ( "Some of the particles are not parton while the partonic mode is set" ) ; }
	  m_moms = m_moms || (*id)== LoKi::Cuts::GABSID ;
	}
    }
  
  if ( m_useSelector){
    // check that a mother container location have been specified  
    if ( m_motherSelector.empty()){
      return Error ( "No HepMC mother selector have been specified!" ) ;
    }
    m_motherSelectorTool = tool<IHepMCParticleSelector>(m_motherSelector ,m_motherSelector, this );
  }
  
  // Check the list of particles to be created
  if ( m_particles.empty() )
    { return Error ( "Empty list of particles!" ) ; }
  //
  using namespace LoKi::Cuts ;

  m_ids = m_particles[0] == GABSID ;

  for ( IDs::const_iterator id = m_particles.begin()+1 ; 
        m_particles.end() != id ; ++id ) 
    { m_ids = m_ids || (*id)== GABSID ; }
  //
  info ()
    << " Particles to be selected: " 
    << Gaudi::Utils::toString ( m_particles ) 
    << " Cut to be used " << m_ids << endmsg ;

  if(m_partonicmode)info()<<"Partonic Mode on"<<endmsg;
  
  // if partonic mode, skip acceptance cut
  if (m_partonicmode) return StatusCode::SUCCESS ;

  m_gamma = ("gamma" == GID || "KL0" == GABSID|| "n0" == GABSID);


  m_charged = 
    ( "e+"  == GABSID || "mu+" == GABSID || 
      "pi+" == GABSID || "K+"  == GABSID || "p+" == GABSID ) ;
  
  m_gammacut = GALL ;
  m_chargedcut = GALL ;  

  if( ! m_forceNoCutAcceptance ) {
    m_gammacut = GALL && 0 < GPZ ;
    if ( 0 <  m_minPtGamma    ) 
      { m_gammacut = m_gammacut && m_minPtGamma < GPT ; }
    if ( 0 <  m_minThetaGamma ) 
      {
	m_gammacut = m_gammacut && 
	  ( m_minThetaGamma < atan2 ( abs ( GPX ) , GPZ ) ||  
	    m_minThetaGamma < atan2 ( abs ( GPY ) , GPZ ) ) ;  
      }
    if ( 0 <  m_maxThetaXGamma ) 
      { m_gammacut = m_gammacut && m_maxThetaXGamma > atan2 ( abs ( GPX ) , GPZ ) ; }
    if ( 0 <  m_maxThetaYGamma ) 
      { m_gammacut = m_gammacut && m_maxThetaYGamma > atan2 ( abs ( GPY ) , GPZ ) ; }

  
    m_chargedcut = GALL ;  
    if ( 0 < m_minPCharged ) 
      {  m_chargedcut = m_chargedcut &&  m_minPCharged     < GP      ; }
    if ( 0 < m_minPtCharged ) 
      {  m_chargedcut = m_chargedcut &&  m_minPtCharged    < GPT     ; }
    if ( 0 < m_minThetaCharged ) 
      {  m_chargedcut = m_chargedcut &&  m_minThetaCharged < GTHETA  ; }
    if ( 0 < m_maxThetaCharged ) 
      {  m_chargedcut = m_chargedcut &&  m_maxThetaCharged > GTHETA  ; }
  }
  info()
    << " Gammas: "           << m_gamma   
    << " Cuts for gammas: "  << m_gammacut   
    << " Charged: "          << m_charged 
    << " Cuts for charged: " << m_chargedcut 
    << endmsg ;

  return StatusCode::SUCCESS ;
} 
// ============================================================================
// INTERMEZZO 
// ============================================================================
namespace 
{
  // ==========================================================================
  class UseIt
    : public LoKi::BasicFunctors<const HepMC::GenParticle*>::Predicate
  {
  public:
    /// constructor
    UseIt( const LoKi::HepMCParticleMaker* maker ) 
      : LoKi::BasicFunctors<const HepMC::GenParticle*>::Predicate () 
      , m_maker ( maker ) 
    {}
    /// copy constructor
    UseIt ( const UseIt& right )
      : LoKi::AuxFunBase                          ( right )
      , LoKi::BasicFunctors<const HepMC::GenParticle*>::Predicate ( right )  
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
    LoKi::Interface<LoKi::HepMCParticleMaker> m_maker ;
  } ;
  // ==========================================================================
}
// ============================================================================
#define INHERITS(T1,T2)						\
  (Relations::IsConvertible<const T1*,const T2*>::value &&	\
   !Relations::IsConvertible<const T1*,const void*>::same)


// ============================================================================
// create the particles 
// ============================================================================
StatusCode LoKi::HepMCParticleMaker::makeParticles 
( LHCb::Particle::ConstVector& particles ) 
{
  // avoid long names 
  using namespace LoKi        ;
  using namespace LoKi::Types ;
  using namespace LoKi::Cuts  ;


  // Create the relation table between Particles and GenParticles
  typedef LHCb::Relation2D<LHCb::Particle,HepMC::GenParticle*> Table ;
  static_assert(INHERITS(Table,LHCb::RC2HepMC2D),"Table must be convertible from LHCb::RC2HepMC2D");
  Table* table = 0 ;  
  if ( !outputTable().empty()  ) 
    {
      table = new Table(100) ;
      put ( table , outputTable()  ) ;
    }
  const size_t N0 = particles.size() ;

  typedef LoKi::GenTypes::GenContainer Container ;
  
  // Set the main cut (should have the ID of m_ids and pass the use function)
  GCut cut = m_ids ;
  if ( !m_partonicmode ) cut = cut && UseIt( this );
  else cut = cut  &&  GSTATUS != 3 ;

  // The major cut to find mother
  GCut m_motherCut = m_moms;
  
  if (! m_mothers.empty() && m_useSelector){
    GCut m_momsSel = GNONE;
    Container mothersSelected ;
    // get the mother vector
    for ( Addresses::const_iterator iaddr = m_addresses.begin() ; 
          m_addresses.end() != iaddr ; ++iaddr ) 
      {
	const LHCb::HepMCEvent::Container* events =  get<LHCb::HepMCEvent::Container> ( *iaddr ) ;
	LoKi::Extract::genParticles ( events , std::back_inserter( mothersSelected  ) , m_moms) ;
      }   
    // for each element of the vector, put the energy and pt equality as cut
    for  ( Container::const_iterator ip = mothersSelected.begin() ; mothersSelected.end() != ip ; ++ip )
      {
	if (m_motherSelectorTool->accept(*ip)){
	  //  always()<<"A MOOOOOOOOOOOOOOOOOOOOOOOOOOOOTHER IS FOUND...................."<<endmsg;
        
	  m_momsSel = m_momsSel || ( GE(*ip) == GE ) ; 
	}
      }  
    m_motherCut = m_motherCut && m_momsSel;
  }

  // Add a FIX for DC06 Hidden valley partons
  /*if( !m_mothers.empty()){ 
    if ( m_mothers[0] == "H_30")
    {
    Warning("You are using a DC06 fix... probably because you ask H_30 mother and the fix was not removed");
    GCut m_momsSel = GNONE;
    Container mothersSelected ;
    for ( Addresses::const_iterator iaddr = m_addresses.begin() ; 
    m_addresses.end() != iaddr ; ++iaddr ) 
    {
    const LHCb::HepMCEvent::Container* events =  get<LHCb::HepMCEvent::Container> ( *iaddr ) ;
    LoKi::Extract::genParticles ( events , std::back_inserter( mothersSelected  ) , m_moms) ;
    }   
    // for each element of the vector, put the energy and pt equality as cut
    for  ( Container::const_iterator ip = mothersSelected.begin() ; mothersSelected.end() != ip ; ++ip )
    {
    if (m_motherCut(*ip)){
    double liftetime = (((*ip)->end_vertex()->point3d().x()-(*ip)->production_vertex()->point3d().x())*
    ((*ip)->end_vertex()->point3d().x()-(*ip)->production_vertex()->point3d().x()) +
    ((*ip)->end_vertex()->point3d().y()-(*ip)->production_vertex()->point3d().y())*
    ((*ip)->end_vertex()->point3d().y()-(*ip)->production_vertex()->point3d().y())+
    ((*ip)->end_vertex()->point3d().z()-(*ip)->production_vertex()->point3d().z())*
    ((*ip)->end_vertex()->point3d().z()-(*ip)->production_vertex()->point3d().z()));
    if(liftetime>0.0000000001){m_momsSel = m_momsSel || ( GE(*ip) == GE ) ;}
    }
    }
    m_motherCut = m_motherCut && m_momsSel;
    }
    }*/
  
  
  // get all the particles that satisfy the mother cut and update the global cut
  if ( !m_mothers.empty()  ) 
    {
      // select all HepMC particles as mother 
      Container moms ;
      //moms.reserve ( 1000 ) ;
      for ( Addresses::const_iterator iaddr = m_addresses.begin() ; 
	    m_addresses.end() != iaddr ; ++iaddr ) 
	{
	  const LHCb::HepMCEvent::Container* events = get<LHCb::HepMCEvent::Container> ( *iaddr ) ;
	  /// copy good particles matching with m_moms cut (mother ID or E/PT equality depending on the mother source or ALL if no mother)
	  LoKi::Extract::genParticles ( events , std::back_inserter( moms ) , m_motherCut ) ;
	}
      if ( moms.empty() )  Warning ( "No valid mother particles are selected, empty result is expected") ;
      cut = cut && GFROMTREE( moms.begin() , moms.end() ) ;
    }
  
  Gaudi::LorentzVector included_energy(0.0,0.0,0.0,0.0);
  Gaudi::LorentzVector excluded_energy(0.0,0.0,0.0,0.0);
  // collect "good" particles into the container 
  Container good ;
  good.reserve ( 2000 ) ;
  {
    for ( Addresses::const_iterator iaddr = m_addresses.begin() ;  m_addresses.end() != iaddr ; ++iaddr ) 
      {
	const LHCb::HepMCEvent::Container* events =    get<LHCb::HepMCEvent::Container> ( *iaddr ) ;
	/// copy good particles
	LoKi::Extract::genParticles ( events , std::back_inserter( good ) , cut ) ;
      }  
  }
  // convert good generator particles into recontructed particles 
  particles.reserve( particles.size() + good.size() ) ;
  for ( Container::const_iterator ip = good.begin() ; good.end() != ip ; ++ip ) 
    {
      const HepMC::GenParticle* p = *ip ;
      
      if ( 0 == p ) { continue ; };
      if (!m_processes.empty()){

      const HepMC::GenEvent* evt = p->parent_event();
      bool GoodPro = false;
      int PROID = evt->signal_process_id();

      for(INTs::const_iterator iINT = m_processes.begin();  m_processes.end() != iINT; ++iINT)
	if(*iINT == PROID) { GoodPro = true; break;}
      
      //reject not good processes, if requested
      if(!GoodPro){continue;};
      }

      if (m_excludeFromMother){
	//reject particles here whose parent (and that particle's parent, etc) includes m_excludeMotherNumber.
	bool in_loop = true;
	bool from_mother = false;
	const HepMC::GenParticle* p2 = p;     
	while (in_loop) {
	  if (!p2) {
	  break;
	  }
	  HepMC::GenVertex * thePV =  p2 -> production_vertex() ;
	  if (!thePV) break;
	  for(auto p_in = thePV -> particles_begin( HepMC::parents); p_in != thePV -> particles_end(HepMC::parents); ++p_in){
	    if (GABSID(*p_in)==m_excludeMotherNumber) {
	      from_mother = true;
	      in_loop = false;
	      break;
	    }
	    p2=*p_in;
	  }
	}
	if (from_mother){
	  Gaudi::LorentzVector exc_mom ( p->momentum() ) ;
	  excluded_energy+=exc_mom;
	  continue;
	}
      }

      // create new particle 
      LHCb::Particle* particle = new LHCb::Particle() ;
      // fill it 
      StatusCode sc = fillParticle ( p , particle ) ;
      if ( sc.isFailure() ) 
	{ 
	  Warning("Error from fillParticle, skip it") ;
	  delete particle ;
	  continue ;                                               // CONTINUE 
	}
      // fill the relation table if needed 
      if ( table ) { table->i_push( particle , p ) ; }     // NB: i_push!
      Gaudi::LorentzVector inc_mom ( p->momentum() ) ;
      included_energy+=inc_mom;
      // add to the container 
      particles.push_back ( particle ) ;
    }
  
  // MANDATORY CALL FOR i_sort after i_push !
  if ( table ) { table->i_sort() ; }  // ATTENTION!
  // some decorations 
  if ( statPrint() || msgLevel ( MSG::DEBUG ) ) 
    {
      counter("#particles") += particles.size() - N0 ;
      if ( table ) { counter("#links") += table->relations().size() ; }
    }
  //
  if ( msgLevel(MSG::DEBUG) ) debug() <<"Included Momentum 4-vector of event: "<< included_energy<<std::endl;
  if ( msgLevel(MSG::DEBUG) ) debug() <<"Excluded Momentum 4-vector of event: "<< excluded_energy<<std::endl;
  if ( msgLevel(MSG::DEBUG) ) debug() <<"Invariant Mass of Excluded Momentum 4-vector: "<< excluded_energy.M()<<std::endl;
  if ( msgLevel(MSG::DEBUG) ) debug() <<"Total Momentum 4-vector of event: "<< included_energy+excluded_energy<<std::endl;
  return StatusCode::SUCCESS ;
} 
// ============================================================================
// Fill the particle 
// ============================================================================
StatusCode LoKi::HepMCParticleMaker::fillParticle 
( const HepMC::GenParticle* gen ,
  LHCb::Particle*           rcp ) 
{
  if ( !gen ) { return Error ( "HepMC::GenParticle* points to NULL" ) ; }
  if ( !rcp ) { return Error ( "LHCb::Particle*     points to NULL" ) ; }
  Gaudi::LorentzVector mom ( gen->momentum() ) ;
  StatusCode sc = generateCovariance ( mom , m_covariance ) ;
  if ( sc.isFailure() ) 
    { return Error ( "Error form generate covariance" , sc ) ; }
  Gaudi::XYZPoint point ( 0 , 0 , 0 ) ;
  const HepMC::GenVertex* v = gen->production_vertex() ;
  if ( v ) { point = Gaudi::XYZPoint( v -> point3d() ) ; }
  else 
    { Warning ( "HepMC::GenVertex* points to NULL, use (0,0,0) instead" ) ; }
  //
  return MCParticleMakerBase::fillParticle 
    ( mom , point ,  LHCb::ParticleID( gen->pdg_id() ) , m_covariance , *rcp ) ;
}
// ============================================================================
// use the particle?
// ============================================================================
bool LoKi::HepMCParticleMaker::use ( const HepMC::GenParticle* p ) const 
{
  using namespace LoKi::Cuts  ;
  using namespace LoKi::Types ;
  
  if ( !p ) { return false ; }
  
  // check the origin cylinder  
  const HepMC::GenVertex* v0 = p->production_vertex() ;
  if ( !v0 )
    {
      Warning("Production vertex is NULL, skip it") ;
      return false ;                                         // RETURN 
    }
  const Gaudi::XYZPoint p0 (  v0->point3d() ) ;
  // check for production vertex 
  if( ! m_forceNoCutAcceptance ){  
    if ( 0 < m_maxZproduction   && std::fabs( p0.Z() ) > m_maxZproduction    ) 
      { return false ; }
    if ( 0 < m_maxRhoProduction && p0.Rho()       > m_maxRhoProduction  ) 
      { return false ; }
  }
  // check for decay vertex 
  const HepMC::GenVertex* ve = p->end_vertex() ;
  if( ! m_forceNoCutAcceptance ){  
    if ( ve )
      {
	const Gaudi::XYZPoint pe ( ve->point3d() ) ;
	if ( 0 < m_minZend  && pe.Z() < m_minZend ) { return false ; }  
      }
    if      ( m_gamma   ( p ) ) { return m_gammacut   ( p ) ; }
    else if ( m_charged ( p ) ) { return m_chargedcut ( p ) ; }
  }
  else{
    return ve == nullptr;
  }

  // other particles 
  return true ;
}

// ============================================================================
/// Declaration of the Tool Factory
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi,HepMCParticleMaker)
// ============================================================================
// The END 
// ============================================================================
