// $Id$
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToolFactory.h" 
#include "GaudiKernel/IIncidentListener.h" 
#include "GaudiKernel/IIncidentSvc.h" 
#include "GaudiKernel/SystemOfUnits.h" 
#include "GaudiKernel/ToStream.h" 
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiTool.h"
// ============================================================================
// Relations 
// ============================================================================
#include "Relations/IRelation.h"
#include "Relations/Relation2.h"
#include "Relations/IRelation2D.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/MCParticle.h"
#include "Event/HepMCEvent.h"
// ============================================================================
// DaVinciMCKernel 
// ============================================================================
#include "Kernel/IHepMC2MC.h"
#include "Kernel/HepMC2MC.h"
// ============================================================================
// LoKi 
// ============================================================================
#include "LoKi/ILoKiSvc.h"
#include "LoKi/MCParticleCuts.h"
#include "LoKi/MCVertexCuts.h"
#include "LoKi/GenParticleCuts.h"
#include "LoKi/Kinematics.h"
#include "LoKi/ParticleProperties.h"
#include "LoKi/Inherits.h"
// ============================================================================
// Boost
// ============================================================================
#include "boost/static_assert.hpp"
// ============================================================================
/** @file
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-01-23 
 */
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /** @class HepMC2MC
   *  The most simplest implementation of IHepMC2MC interface
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2001-01-23 
   */
  class HepMC2MC 
    : public virtual IHepMC2MC         
    , public virtual IIncidentListener 
    , public         GaudiTool  
  {
    // ========================================================================
    // friend factory for instantiation
    friend class ToolFactory<LoKi::HepMC2MC> ;
    // ========================================================================
  public:
    // ========================================================================
    /** return a relation table for HepMC::GenParticle -> LHCb::MCParticle 
     *  relations 
     *  
     *  @code 
     *  typedef IHepMC2MC::HepMC2MC Table ;
     *  typedef Table::Range        Range ;
     * 
     *  // tool itself 
     *  const LoKi::IHepMC2MC*  imc = ... ;
     *  // get the relation table 
     *  const Table* table = imc->hepMC2MC() ;
     *  if ( 0 == table ) { ... } ; // error here!!
     * 
     *  // 
     *  const HepMC::GenParticle* hepMC = ... ;
     *  // get MC particles associated with given HepMC::GenParticle
     *  Range links = table->relations( hepMC ) ; 
     *
     *  @endcode 
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@lapp.in2p3.fr
     *  @date 2005-05-12 
     */  
    virtual const IHepMC2MC::HepMC2MC* hepMC2MC () const 
    {
      //
      if  ( valid() ) { return m_table.direct() ; }
      // 
      // build  and validate the table 
      StatusCode sc = build () ;
      // the final check  
      Assert ( sc.isSuccess () , "Unable to build the table" , sc ) ;
      Assert ( valid()         , "The table is invalid!"          ) ;
      // 
      return m_table.direct() ;
      //
    } 
    // ========================================================================
    /** return a relation table for LHCb::MCParticle -> HepMC::GenParticle 
     *  relations 
     *  
     *  @code 
     *  typedef IHepMC2MC::MC2HepMC Table ;
     *  typedef Table::Range        Range ;
     * 
     *  // tool itself 
     *  const LoKi::IHepMC2MC*  imc = ... ;
     *  // get the relation table 
     *  const Table* table = imc->mc2HepMC() ;
     *  if ( 0 == table ) { ... } ; // error here!!
     * 
     *  // 
     *  const LHCb::MCParticle* mc = ... ;
     *  // get HepMC particles associated with given MCParticle
     *  Range links = table->relations( mc ) ; 
     *
     *  @endcode 
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@lapp.in2p3.fr
     *  @date 2005-05-12 
     */  
    virtual const IHepMC2MC::MC2HepMC* mc2HepMC () const 
    {
      //
      if  ( valid() ) { return m_table.inverse () ; }
      // 
      // build  and validate the table 
      StatusCode sc = build () ;
      // the final check  
      Assert ( sc.isSuccess () , "Unable to build the table" , sc ) ;
      Assert ( valid()         , "The table is invalid!"          ) ;
      // 
      return m_table.inverse () ;
      //
    }
    // ========================================================================
  public:
    // ========================================================================
    /// standard initialization of the tool 
    virtual StatusCode initialize () 
    {
      StatusCode sc = GaudiTool::initialize () ;
      if ( sc.isFailure() ) { return sc ; }
      
      IIncidentSvc* isvc = incSvc() ;
      Assert ( 0 != isvc , "IIncidentSvc* points to NULL" );
      isvc -> addListener ( this , IncidentType::BeginEvent , 10 ) ;
      
      // load LoKi service 
      svc<LoKi::ILoKiSvc>( "LoKiSvc" ) ;
      
      if ( m_hepmcs . empty () ) 
      { Warning ( "Empty list of HepMC-locations " ) ; }
      if ( m_mcps   . empty () ) 
      { Warning ( "Empty list of    MC-locations " ) ; }
    
      return StatusCode::SUCCESS ;
    }
    // ========================================================================
    /// handle new incident 
    virtual void handle ( const Incident& ) 
    { 
      // invalidate the table 
      m_valid = false ;
      m_table.clear () . ignore () ;      
    }
    // ========================================================================
  protected:
    // ========================================================================
    /** standart constructor 
     *  @param type tool type 
     *  @param name tool name 
     *  @param parent parent object
     */
    HepMC2MC 
    ( const std::string& type   , 
      const std::string& name   , 
      const IInterface*  parent )
      : GaudiTool ( type, name , parent )
      ///
      , m_hepmcs (  1 , LHCb::HepMCEventLocation:: Default )
      , m_mcps   (  1 , LHCb::MCParticleLocation:: Default )
      , m_cut1   ( 0.1  * Gaudi::Units::MeV * Gaudi::Units::MeV )
      , m_cut2   ( 1.e-4 )
      , m_vcut   ( 100  * Gaudi::Units::micrometer )
      //
      , m_table  ( ) 
      , m_valid  ( false )
    {
      declareInterface<IIncidentListener> ( this ) ; 
      declareInterface<IHepMC2MC>         ( this ) ; 
      // 
      // ======================================================================
      declareProperty 
        ( "HepMCEvents" , 
          m_hepmcs      , 
          "The list of TES locations for HepMC-events ") ;
      declareProperty 
        ( "MCParticles" , 
          m_mcps         ,
          "The list of TES locations for Monte Carlo particles") ;
      declareProperty 
        ( "Cut1"  , 
          m_cut1  ,
          "The maximum allowed value for delta2eucludian (absolute)" ) ;
      declareProperty 
        ( "Cut2"  , 
          m_cut2  ,
          "The maximum allowed value for delta2eucludian (relative)" ) ;
      declareProperty 
        ( "VertexCut" , 
          m_vcut      ,
          "The maximum allowed value for vertex-distance (if applicable)" ) ;
      // ======================================================================
    }
    // ========================================================================
    /// destructor (virual and protected)
    virtual ~HepMC2MC () {} 
    // ========================================================================
  private:
    // ========================================================================
    /// default constructor  is disabled 
    HepMC2MC ()  ;                         /// default constructor  is disabled 
    /// copy constructor     is disabled 
    HepMC2MC ( const HepMC2MC& ) ;              // copy constructor is disabled 
    /// assignement operator is disabled 
    HepMC2MC& operator= ( const HepMC2MC& ) ;        // no assignement operator
    // ========================================================================
  protected:
    // ========================================================================
    /// rebuild the table 
    StatusCode build () const ;    
    // check validity flag
    bool valid() const { return m_valid ; }
    // ========================================================================
  private:
    // ========================================================================
    /// the actual type of addresses 
    typedef std::vector<std::string> Addresses ;
    /// The list of TES locations for HepMC-events 
    Addresses    m_hepmcs ;                   // TES locations for HepMC-events 
    /// The list of TES locations for Monte Carlo particles") ;
    Addresses    m_mcps   ;          // TES locations for Monte Carlo particles
    /// The maximum allowed value for delta2eucludian (absolute)
    double m_cut1 ;                   // The maximum delta2eucludian (absolute)
    /// The maximum allowed value for delta2eucludian (relative)
    double m_cut2 ;                   // The maximum delta2eucludian (relative)
    /// The maximum allowed value for vertex-distance (if applicable)
    double m_vcut ;                                              // vertex cut 
    // ========================================================================
  private:
    // ========================================================================
    /// the actual interface class (not exported) 
    typedef Relations::Relation2<HepMC::GenParticle*,LHCb::MCParticle> Table ;
    /// check fot the type 
    BOOST_STATIC_ASSERT(INHERITS(Table,IHepMC2MC::HepMC2MC)) ;
    /// the relation table itself
    mutable Table m_table ;                        // the relation table itself
    /// the validity flag 
    mutable bool  m_valid ;                               /// the validity flag 
    // ========================================================================
  };
  // ==========================================================================
} // end of namespace LoKi 
// rebuild the table 
// ============================================================================
StatusCode LoKi::HepMC2MC::build () const 
{
  // invalidate the table 
  m_table.clear() .ignore() ;
  m_valid = false ;
  
  // avoid long names
  using namespace LoKi        ;
  using namespace LoKi::Types ;
  using namespace LoKi::Cuts  ;
  
  typedef std::vector<HepMC::GenParticle*>     GPs   ;
  typedef std::map<LHCb::ParticleID,GPs>       HMCPs ;
  typedef std::vector<const LHCb::MCParticle*> MCPs  ;
  typedef std::map<LHCb::ParticleID,MCPs>      MMCPs ;
  
  // temporary map 
  HMCPs hmcps ;
  // loop over all HepMC constainers and pick up the particles
  for ( Addresses::const_iterator ih = m_hepmcs.begin() ; 
        m_hepmcs.end() != ih ; ++ih ) 
  {
    const LHCb::HepMCEvents* evts = get<LHCb::HepMCEvents>( *ih ) ;
    if ( 0 == evts ) { continue ; }
    for ( LHCb::HepMCEvents::const_iterator ie = evts->begin() ; 
          evts->end() != ie ; ++ie ) 
    {
      LHCb::HepMCEvent* evt = *ie ;
      if ( 0 == evt                               ) { continue ; }
      HepMC::GenEvent* event = evt->pGenEvt() ;
      if ( 0 == event || event->particles_empty() ) { continue ; }
      for ( HepMC::GenEvent::particle_iterator ip = event->particles_begin() ; 
            event->particles_end() != ip ; ++ip ) 
      {
        HepMC::GenParticle* p = *ip ;
        if ( 0 == p ) { continue ; }
        LHCb::ParticleID pid( p->pdg_id() ) ;
        GPs& ps = hmcps[pid] ;
        if ( ps.empty() ) { ps.reserve( 100 ) ; }
        ps.push_back ( p ) ;
      }
    }
  } // end of loop over HepMC data containers

  
  // auxillary map 
  MMCPs mcMap ;
  // loop over all MC-containers and pick up the particles 
  for ( Addresses::const_iterator ih = m_mcps.begin() ; 
        m_mcps.end() != ih ; ++ih ) 
  {
    const LHCb::MCParticle::Container* mc = 
      get<LHCb::MCParticle::Container> ( *ih ) ;
    if ( 0 == mc ) { continue ; }
    for ( LHCb::MCParticle::Container::const_iterator imcp = mc->begin() ; 
          mc->end() != imcp ; ++imcp )
    {
      const LHCb::MCParticle* mcp = *imcp ;
      if ( 0 == mcp ) { continue ; }
      MCPs& mcps = mcMap[ mcp->particleID() ];
      if ( mcps.empty() ) { mcps.reserve ( 200 ) ; }
      mcps.push_back ( mcp ) ;
    } // end of the loop over MC-particles 
  } // end of loop over MC data containers


  // loop over the classified GenParticles 
  for ( HMCPs::iterator ih = hmcps.begin() ; hmcps.end() != ih ; ++ih ) 
  {
    // get all GenParticles of the given type
    GPs& gps = ih->second ;
    if ( gps.empty() ) { continue ; }
    // get all MCParticles of the same type
    MMCPs::const_iterator _imc = mcMap.find( ih->first ) ;
    if ( mcMap.end() == _imc ) { continue ; }                   // CONTNINUE     
    const MCPs& mcs = _imc->second ;
    if ( mcs.empty()         ) { continue ; }                   // CONTINNUE 
    
    // loop over all GenParticle of the given type 
    for ( GPs::iterator ip = gps.begin() ; gps.end() != ip ; ++ip ) 
    {
      HepMC::GenParticle* p = *ip  ;
      if ( 0 == p ) { continue ; }                  // CONTINUE
      // find a "nearest" MC particle to given through 
      // looping over all MC particles with the same ParticleID 
      const LoKi::LorentzVector vct ( p->momentum() ) ;      
      MCFun dist  = MCMOMDIST ( vct ) ;
      
      // find the nearest particle:
      MCPs::const_iterator imc = mcs.end() ;
      //  check also the validity and the distance between 
      //  production and origin vertices  
      //       const HepMC::GenVertex* pv = p->production_vertex () ;
      //       if ( 0 != pv ) 
      //       {
      //         // The minimum is searched within MC  particles, which 
      //         // have the valid origin vertex within certain distance 
      //         // from production vertex or have no valid origin vertex 
      //         // at all 
      //         const LoKi::Point3D pp ( pv->point3d()  ) ;
      //         MCCut mcCut1 = !MCOVALID ;
      //         MCCut mcCut2 = MCVFASPF( MCVDIST( pp ) ) <  m_vcut   ;
      //         imc = LoKi::select_min 
      //           ( mcs.begin () , 
      //             mcs.end   () , 
      //             dist         , 
      //             mcCut1 || mcCut2 ) ;    
      
      //         MCFun fun2 = MCVFASPF( MCVDIST( pp ) ) ;
      //         std::cout << " PARAMS 0  " 
      //                   << " DISTM : " << dist ( mcs.front() ) 
      //                   << " DISTR : " << fun2 ( mcs.front() ) 
      //                   << " CUT="     << (mcCut1||mcCut2)( mcs.front() ) 
      //                   << " FOUND "   << Gaudi::Utils::toString ( mcs.end() != imc ) 
      //                   << std::endl ;
      
      //         const LHCb::MCVertex* v = mcs.front() -> originVertex() ;
      //         if ( 0 != v ) 
      //         {
      //           std::cout 
      //             << " GenVertex " << pv->point3d () 
      //             << " MCVertex "  << v ->position  () << std::endl;   
      //         }
      
      //       }
      //       else // look for all MC particles
      { imc = LoKi::select_min ( mcs.begin () , mcs.end () , dist ) ; }
      
       if ( mcs.end() == imc ) { continue ; }                  // CONTINUE 
      
      const LHCb::MCParticle* mcp = *imc ;
      if ( 0 == mcp         ) { continue ; }                  // CONTINUE 
      //
      const double value = 
        LoKi::Kinematics::delta2euclidian ( vct ,  mcp->momentum() ) ;
      if ( m_cut1 < value ) { continue ; }                        // CUT
      const double v1 = LoKi::Kinematics::euclidianNorm2  ( vct             ) ;
      const double v2 = LoKi::Kinematics::euclidianNorm2  ( mcp->momentum() ) ;
      // ======================================================================
      // finally: fill the table 
      if ( value <= m_cut2 * std::min ( v1 , v2 ) )              // CUT 
      {  m_table.i_push ( p , *imc ) ; }                // NB! "i_push" is used 
      // ======================================================================
    } // loop over all HepMC particles with the given PID     
  } // looo over ParticleID species 
  
  
   // mandatory call for "i_sort" after "i_push" ;
  m_table.i_sort() ;                  // NB! i_sort after i_push is MANDATORY!!
  
  if ( m_table.relations().empty() ) 
  { Warning("The relation table is empty!") ; }
 
  // DECORATION:
  
  // total number of established links 
  const size_t links = m_table.relations().size() ;
  
  if ( printStat() || msgLevel( MSG::DEBUG ) ) 
  { counter ( "#HepMC->MC" ) += links ; }
  
  if ( msgLevel ( MSG::DEBUG ) )
  { debug() << " Number of 'HepMC<-->MC' links : " << links  << endmsg ; }

  
  /// IMPORTANT! 
  m_valid = true ;                                      // IMPORTANT: validate!
  
  return StatusCode::SUCCESS ;
}
// ============================================================================
DECLARE_NAMESPACE_TOOL_FACTORY( LoKi, HepMC2MC ) 
// ============================================================================
// The END 
// ============================================================================
