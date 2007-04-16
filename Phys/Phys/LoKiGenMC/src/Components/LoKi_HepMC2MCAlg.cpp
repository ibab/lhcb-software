// $Id: LoKi_HepMC2MCAlg.cpp,v 1.15 2007-04-16 16:16:45 pkoppenb Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.15 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.14  2007/03/04 16:33:27  ibelyaev
//  v2r1: fix a bug .. need to think about the problme
//
// Revision 1.13  2006/10/27 13:38:22  ibelyaev
//  fix for SLC4 platform
//
// Revision 1.12  2006/08/29 11:35:46  ibelyaev
//  many fixes after detailed tests
//
// ============================================================================
// Include files  
// ============================================================================
// STD & STL 
// ============================================================================
#include <map>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/SystemOfUnits.h" 
#include "GaudiKernel/ToStream.h" 
// ============================================================================
// Event 
// ============================================================================
#include "Event/MCParticle.h" 
#include "Event/HepMCEvent.h" 
// ============================================================================
// LoKiCore
// ============================================================================
#include "LoKi/ILoKiSvc.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/MCParticleCuts.h"
#include "LoKi/MCVertexCuts.h"
#include "LoKi/GenParticleCuts.h"
#include "LoKi/Kinematics.h"
#include "LoKi/ParticleProperties.h"
// ============================================================================
// Relations 
// ============================================================================
#include "Relations/Relation2D.h"
// ============================================================================
// LoKiGenMC
// ============================================================================
#include "Kernel/HepMC2MC.h"
// ============================================================================

// ============================================================================
/** @file
 *
 *  Implementation file for class LoKi_HepMC2MCAlg
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

// ============================================================================
namespace std
{
  /** comparison criteria for LHCb::ParticleID objects 
   *  nesessary for proper usage as ker for map-like containers 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 200-01-21
   */
  template<> 
  struct less<const LHCb::ParticleID> : 
    public std::binary_function<const LHCb::ParticleID,
                                const LHCb::ParticleID,bool>
  {
    inline bool operator() 
      ( const LHCb::ParticleID& id1 , 
        const LHCb::ParticleID& id2 ) const 
    { return id1.pid() < id2.pid() ; }
  };
  /** comparison criteria for LHCb::ParticleID objects 
   *  nesessary for proper usage as the key for map-like containers 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 200-01-21
   */
  template<> 
  struct less<LHCb::ParticleID> : 
    public std::less<const LHCb::ParticleID> {};
};
// ============================================================================

// ============================================================================
/** @class LoKi_HepMC2MCAlg
 *
 *  helper algorthm to build HepMC  <---> MCParticle/MCVertex relations 
 *  
 *  It associates the HepMC::GenParticle p1 to LHCb::MCPArticle p2 
 *  according to the folowing rule: For all MC particles of the same 
 *  type, it finds the particle "nearest" in euclidian distance in 4-momenta.
 *  For the nearest particle 
 * 
 *    - LHCb::ParticleID ( p1->pdg_id() ) == p2->particleID() ;
 *    - It finds the
 *  The object "p1" of type HepMC::GenParticle is related/associated to 
 *  the object "p2" of the type LHCb::MCParticle if 
 *
 *  @attention the algorithm is relatively slow! 
 *
 *  @see LHCb::MCParticle
 *  @see HepMC::GenParticle
 *  @see LHCb::HepMC2MC
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@lapp.in2p3.fr
 *  @date 2005-07-13
 */
// ============================================================================
class LoKi_HepMC2MCAlg
  : public GaudiAlgorithm 
{
  friend class AlgFactory<LoKi_HepMC2MCAlg>;
public:
  /// initialization of the algorithm
  virtual StatusCode intialize () 
  {
    StatusCode sc = GaudiAlgorithm::initialize() ;
    if ( sc.isFailure() ) { return sc ; }
    // load LoKi service 
    svc<LoKi::ILoKiSvc>( "LoKiSvc" ) ;
    return StatusCode::SUCCESS ;
  } ;
  /// execution of the algorithm
  virtual StatusCode execute() ;
protected:
  /**standard constructor 
   * @param name algorithm instance name 
   * @param pSvc pointer to Service Locator 
   */
  LoKi_HepMC2MCAlg 
  ( const std::string& name , 
    ISvcLocator*       pSvc ) 
    : GaudiAlgorithm ( name , pSvc )
    , m_hepmcs    (  1 , LHCb::HepMCEventLocation:: Default )
    , m_mcps      (  1 , LHCb::MCParticleLocation:: Default )
    , m_output    (      LHCb::HepMC2MCLocation::   Default )
    , m_cut1      ( 0.1  * Gaudi::Units::MeV * Gaudi::Units::MeV )
    , m_cut2      ( 1.e-4                   )
    , m_vcut      ( 100  * Gaudi::Units::micrometer )
  {
    declareProperty ( "HepMCEvents" , m_hepmcs ) ;
    declareProperty ( "MCParticles" , m_mcps   ) ;
    declareProperty ( "Output"      , m_output ) ;
    declareProperty ( "Cut1"        , m_cut1   ) ;
    declareProperty ( "Cut2"        , m_cut2   ) ;
    declareProperty ( "VertexCut"   , m_vcut   ) ;
    //
    setProperty     ( "StatPrint"   , "true"   ) ;
    //
  };
  // destructor, virtual and protected 
  virtual ~LoKi_HepMC2MCAlg() {} ;
private:
  // default constructor is disabled 
  LoKi_HepMC2MCAlg            () ;
  // copy constructor is disabled 
  LoKi_HepMC2MCAlg            ( const LoKi_HepMC2MCAlg& ) ;
  // assignement operator is disabled 
  LoKi_HepMC2MCAlg& operator= ( const LoKi_HepMC2MCAlg& ) ;
private:
  typedef std::vector<std::string> Addresses ;
  Addresses    m_hepmcs ;
  Addresses    m_mcps   ;
  std::string  m_output ;
  double       m_cut1   ;
  double       m_cut2   ;
  double       m_vcut   ;
};
// ============================================================================
DECLARE_ALGORITHM_FACTORY(LoKi_HepMC2MCAlg);
// ============================================================================
namespace 
{
  // the actual type of relation table 
  typedef LHCb::Relation2D<HepMC::GenParticle*,LHCb::MCParticle> Table  ;
} ;
// ============================================================================
/// execution of the algorithm
// ============================================================================
StatusCode LoKi_HepMC2MCAlg::execute() 
{

  using namespace LoKi        ;
  using namespace LoKi::Types ;
  using namespace LoKi::Cuts  ;
  
  typedef std::vector<HepMC::GenParticle*>     GPs   ;
  typedef std::map<LHCb::ParticleID,GPs>       HMCPs ;
  typedef std::vector<const LHCb::MCParticle*> MCPs  ;
  typedef std::map<LHCb::ParticleID,MCPs>      MMCPs ;
  
  // create the relation table and register it into TES 
  Table* table = new Table() ;
  put ( table , m_output ) ;

  HMCPs hmcps ;
  { 
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
  };
  
  // auxillary map 
  MMCPs mcMap ;
  {
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
        if ( mcps.empty() ) { mcps.reserve ( 100 ) ; }
        mcps.push_back ( mcp ) ;
      } // end of the loop over MC-particles 
    } // end of loop over MC data containers     
  }
  
  // loop over the classified GenParticles 
  for ( HMCPs::iterator ih = hmcps.begin() ; 
        hmcps.end() != ih ; ++ih ) 
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
      double value =
        LoKi::Kinematics::delta2euclidian ( vct ,  mcp->momentum() ) ;
      if ( m_cut1 < value ) { continue ; }                        // CUT
      const double v1 = 
        LoKi::Kinematics::euclidianNorm2  ( vct             ) ;
      const double v2 = 
        LoKi::Kinematics::euclidianNorm2  ( mcp->momentum() ) ;
      // finally: fill the table 
      if ( value <= m_cut2 * std::min ( v1 , v2 ) )              // CUT 
      {  table->i_push ( p , *imc ) ; } // NB! "i_push" is used 
      
    } // loop over all HepMC particles
    
    
  } // looo over ParticleID species 
  
  // mandatory call for "i_sort" after "i_push" ;
  table->i_sort() ;
  
  // DECORATIONS:
  
  if ( table->relations().empty() ) { Warning("The relation table is empty!") ; }
  
  // total number of established links 
  const size_t links = table->relations().size() ;
  // make a statistics 
  if ( printStat() || msgLevel( MSG::DEBUG ) ) { counter ( "#HepMC->MC" ) += links ; }
  //
  if ( msgLevel ( MSG::DEBUG ) )
  { debug() << " Number of 'HepMC<-->MC' links : " << links  << endreq ; }
  
  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================

