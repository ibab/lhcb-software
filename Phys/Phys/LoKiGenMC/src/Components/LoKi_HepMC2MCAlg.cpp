// $Id: LoKi_HepMC2MCAlg.cpp,v 1.4 2006-02-18 18:15:44 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.4 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2006/02/09 17:52:35  ibelyaev
//  regular update: add *.opts configuration files
//
// ============================================================================
// Include files  
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/AlgFactory.h" 
// ============================================================================
// Event 
// ============================================================================
#include "Event/MCParticle.h" 
#include "Event/HepMCEvent.h" 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/MCParticleCuts.h"
#include "LoKi/MCVertexCuts.h"
#include "LoKi/GenParticleCuts.h"
#include "LoKi/Kinematics.h"
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
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot et al.: 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23 
 */
// ============================================================================

// ============================================================================
namespace std
{
  /** comparison criteria for LHCb::ParticleID objects 
   *  nesessary for proper usage as ker for map-like ocntainers 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 200-01-21
   */
  template<> 
  struct std::less<const LHCb::ParticleID> : 
    public std::binary_function<const LHCb::ParticleID,
                                const LHCb::ParticleID,bool>
  {
    inline bool operator() 
      ( const LHCb::ParticleID& id1 , 
        const LHCb::ParticleID& id2 ) const 
    { return id1.pid() < id2.pid() ; }
  };
  
  /** comparison criteria for LHCb::ParticleID objects 
   *  nesessary for proper usage as ker for map-like ocntainers 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 200-01-21
   */
  template<> 
  struct std::less<LHCb::ParticleID> : 
    public std::less<const LHCb::ParticleID> {};
  
};
// ============================================================================

// ============================================================================
namespace Relations
{
  /** partical (empty)_ specialization for 
   *  serialization of HepMC::GenParticle objects..
   *  pure technical trick..
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 200-01-21
   */   
  template<>
  StreamBuffer& 
  TypeSerializer<HepMC::GenParticle*>::serialize 
  ( StreamBuffer&                                              buffer    , 
    const TypeSerializer<HepMC::GenParticle*>::Type&  /* object */ ) 
  { return buffer ; }
  
};
// ============================================================================


// ============================================================================
/** @class LoKi_HepMC2MCAlg
 *
 *  helper algorthm to build HepMC  <---> MCParrticle/MCVertex relations 
 *  
 *  It associates the HepMC::GenParticle p1 to LHCb::MCPArticle p2 
 *  according to the folowing rule. For all MC particles of the same 
 *  type, it finds the particle "nearest" in euclidian distance in 4-momenta.
 *  For the nearest particle 
 * 
 *    - LHCb::ParticleID ( p1->pdg_id() ) == p2->particleID() ;
 *    - It finds the
 *  The object "p1" of type HepMC::GenParticle is related/associated to 
 *  the object "p2" of the type LHCb::MCParticle if 
 *
 *  @see LHCb::MCParticle
 *  @see HepMC::GenParticle
 *  @see LHCb::Relations::HepMC2MC
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@lapp.in2p3.fr
 *  @date 2005-07-13
 */
// ============================================================================
class LoKi_HepMC2MCAlg : public GaudiAlgorithm 
{
  friend class AlgFactory<LoKi_HepMC2MCAlg>;
public:
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
    , m_cut1      ( 0.01 * CLHEP::MeV * CLHEP::MeV )
    , m_cut2      ( 1.e-6                   )
    , m_vcut      ( 100 * CLHEP::micrometer )
  {
    declareProperty ( "HepMCEvents" , m_hepmcs ) ;
    declareProperty ( "MCParticles" , m_hepmcs ) ;
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


// ============================================================================
namespace 
{
  // ==========================================================================
  /** @var s_Factory
   *  concrete local instance of algorithm factory to create 
   *  the algorithm of type LoKi_HepMC2MCAlg 
   *  @author Vanya BELYAEV Ivan.Belyaev@lapp.in2p3.fr
   *  @date 2005-07-13
   */
  // ==========================================================================
  const  AlgFactory<LoKi_HepMC2MCAlg>          s_Factory ;
  // ==========================================================================
};
// ============================================================================
/** @var LoKi_HepMC2MCAlgFactory ;
 *  abstract algorithm factory to create 
 *  the algorithm of type LoKi_HepMC2MCAlg 
 *  @author Vanya BELYAEV Ivan.Belyaev@lapp.in2p3.fr
 *  @date 2005-07-13
 */
// ============================================================================
const   IAlgFactory&LoKi_HepMC2MCAlgFactory =  s_Factory ; 
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
  
  typedef std::vector<HepMC::GenParticle*>     HMCPs ;
  typedef std::vector<const LHCb::MCParticle*> MCPs  ;
  typedef std::map<LHCb::ParticleID,MCPs>      MMCPs ;
  
  // create the relation table and register it into TES 
  Table* table = new Table() ;
  StatusCode sc = put ( table , m_output ) ;
  if ( sc.isFailure() ) { return sc ; }
  
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
        const HepMC::GenEvent* event = evt->pGenEvt() ;
        if ( 0 == event || event->particles_empty() ) { continue ; }
        //
        hmcps.reserve ( hmcps.size() + event->particles_size() ) ;
        // it does not work with current vesion of HepMC 
        //hmcps.insert ( hmcps.end()               , 
        //               event->particles_begin () ,
        //               event->particles_end   () ) ;
        // make an explicit loop 
        for ( HepMC::GenEvent::particle_const_iterator ip = 
                event->particles_begin () ; 
              event->particles_end()  != ip ; ++ip ) 
        { hmcps.push_back( *ip ) ; }
      }
    } // end of loop over HepMC data containers 
  };
  
  MCPs mcps ;
  {
    for ( Addresses::const_iterator ih = m_mcps.begin() ; 
          m_mcps.end() != ih ; ++ih ) 
    {
      const LHCb::MCParticles* mc = get<LHCb::MCParticles> ( *ih ) ;
      if ( 0 == mc ) { continue ; }
      mcps.insert ( mcps.end  () , 
                    mc->begin () , 
                    mc->end   () );
    } // end of loop over MC data containers     
  }
  
  // auxillary map 
  MMCPs mcMap ;
  for ( HMCPs::const_iterator ih = hmcps.begin() ; 
        hmcps.end() != ih ; ++ih ) 
  {
    HepMC::GenParticle* p = *ih  ;
    if ( 0 == p ) { continue ; }                  // CONTINUE
    
    const LHCb::ParticleID pid ( p->pdg_id() ) ;
    // fill the slot, if not yet done 
    if ( mcMap.end() == mcMap.find( pid ) ) 
    {
      MCPs& mcs = mcMap[pid] ;
      LoKi::select ( mcps.begin   () , 
                     mcps.end     () ,
                     std::back_inserter ( mcs ) ,
                     pid == MCID  ) ;
    }
    // get all MC particles with the same ID 
    const MCPs& mcs = mcMap[pid] ;
    
    if ( mcs.empty() ) { continue ; }             // CONTINUE 
    
    // find a "nearest" MC particle to given through 
    // looping over all MC particles with the same ParticleID 
    const LoKi::LorentzVector vct ( p->momentum() * GeV ) ;
    MCFun dist  = MCMOMDIST ( vct ) ;
    // find the nearest particle:
    MCPs::const_iterator imc = mcs.end() ;
    //  check also the validity and the distance between 
    //  production and origin vertices  
    const HepMC::GenVertex* pv = p->production_vertex () ;
    if ( 0 == pv ) 
    {
      // The minimum is searched within MC  particles, which 
      // have the valid origin vertex within certain distance 
      // from production vertex or have no vali dorigin vertex 
      // at all 
      const LoKi::Point3D pp ( pv->point3d()  ) ;
      MCCut mcCut1 = !MCOVALID ;
      MCCut mcCut2 = MCVFASPF( MCVDIST( pp ) ) < ( m_vcut )  ;
      imc = LoKi::select_min 
        ( mcs.begin () , 
          mcs.end   () , 
          dist         , 
          mcCut1 || mcCut2 ) ;    
    }
    else // look for all MC particles 
    { imc == LoKi::select_min ( mcs.begin () , mcs.end () , dist ) ; }
    //
    if ( mcs.end() == imc ) { continue ; }                  // CONTINUE 
    const LHCb::MCParticle* mcp = *imc ;
    if ( 0 == mcp         ) { continue ; }                  // CONTINUE 
    
    double value =
      LoKi::Kinematics::delta2euclidian ( vct ,  mcp->momentum() ) ;
    if ( m_cut1 < value ) { continue ; }                        // CUT
    const double v1 = 
      LoKi::Kinematics::euclidianNorm   ( vct             ) ;
    if ( 0 != v1 ) { value /= v1 ; }
    const double v2 = 
      LoKi::Kinematics::euclidianNorm   ( mcp->momentum() ) ;
    if ( 0 != v2 ) { value /= v2 ; }
    
    if ( value < m_cut2 )                                       // CUT 
    { table->i_push ( p , *imc ) ; } // NB! "i_push" is used 
    
  } // loop over all HepMC particles
  
  // mandatory call for "i_sort" after "i_push" ;
  table->i_sort() ;
  
  // DECORATIONS:
  
  // total number of established links 
  const size_t links = table->relations().size() ;
  // make a statistics 
  counter ( "#HepMC->MC" ) += links ;
  if ( msgLevel ( MSG::DEBUG ) ) 
  { debug() << " Number of 'HepMC<-->MC' links : " << links  << endreq ; }
  
  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================

