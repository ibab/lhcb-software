// $Id: DecayTruth.cpp,v 1.2 2008-05-20 09:43:00 cattanem Exp $
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/LoKiAlgoMC.h"
#include "LoKi/ParticleProperties.h"
#include "LoKi/ParticleCuts.h"
#include "LoKi/MCParticleCuts.h"
#include "LoKi/PhysMCParticleCuts.h"
#include "LoKi/DecayChain.h"
#include "LoKi/BuildMCTrees.h"
#include "LoKi/Child.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /** @file 
   *
   *  helper algorithm to inspect MC-truth for the selected signal candidates
   *
   *  The algorthm has been kindly updated for DC06 environmet by Lesya SHCHUTSKA
   *
   *  The usage of the algorithm is fairly trivial. Assuming one 
   *   needs to inspect <c>"B+"</c> and <c>"B-"</c> candidates from some 
   *   TES location <c>"SomeLocationInTES"</c>
   *
   *  @code
   *  
   *  ApplicationMgr.TopAlg += { "LoKi:DecayTruth/MCTruth" } ;
   * 
   *  MCTruth.Particles = { "B+" , "B-"} ;
   *  MCTruth.Desktop.InputLocations = {"SomeLocationInTES"} ;
   *
   *  @endcode 
   *
   *  There are many configuration propeties, but for the default settings
   *  the algorithm is perfectly fine for 99% of cases. 
   * 
   *  The most important properties:
   * 
   *   - <b>"Particles"</b>: list of particle IDs to be inspected, 
   *                         the default is a list of stable beauty mesons, 
   *                         <c>["B0","B~0","B+","B-","B_s0","B_s~0"]</c>
   *   - <b>"RCDepth"</b>: the depth of the printout level, the default is 9
   *   - <b>"RCMode"</b>: the mode of kinematical-printout, 
   *                      the default value is 0, @see LoKi::DecayChain::Mode
   *   - <b>"RCVertex"</d>: print also the decay-vertex information, 
   *                     the default value is <c>false</c> 
   *   - <b>"MCDepth"</b>: the depth of the printout level for Monte Carlo 
   *                       particles, the default is 5
   *   - <b>"MCMode"</b>: the mode of kinematical-printout, 
   *                      the default value is 0, @see LoKi::DecayChain::Mode
   *   - <b>"MCVertex"</d>: print the origin-vertex information for
   *                       Monte Carlo particle, the default value is <c>false</c> 
   *   - <b>"MCVertexE"</d>: print the end-vertices information for
   *                       Monte Carlo particle, requires also 
   *                       the activation of <c>"MCVertex"</c> flag,
   *                       the default value is <c>false</c> 
   *   - <b>"MCVertexD"</d>: If <c>true</c> treat <b>ONLY</b> the 
   *                        decay vertices and ignore all MonteCarlo 
   *                        particles from other sources (e.g. interactions 
   *                        in the detector material),
   *                        the default value is <c>false</c> 
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date   2004-08-17
   */
  class DecayTruth: public LoKi::AlgoMC
  {
    // ========================================================================
    // friend factory for instantiation 
    friend class AlgFactory<LoKi::DecayTruth>;
    // ========================================================================
  public:
    // ========================================================================
    /** analyse the event 
     *  @see LoKi::Algo
     *  @return status code 
     */
    virtual StatusCode analyse   () ;
    // ========================================================================
  protected:
    // ========================================================================
    /** standard constructor
     *  @param name algorithm name instance 
     *  @param psvc pointer to service locator 
     */
    DecayTruth 
    ( const std::string& name , 
      ISvcLocator*       psvc ) ;
    /// virtual & protected destructor 
    virtual ~DecayTruth(){}; // virtual & protected destructor 
    // ========================================================================    
  private:
    // ========================================================================
    /// default constructor is disabled 
    DecayTruth(); // default constructor is disabled 
    /// copy constructor is disabled 
    DecayTruth( const  DecayTruth& ) ; // copy constructor is disabled 
    /// assignement operator is disabled 
    DecayTruth& operator=( const  DecayTruth& ) ; // no assignement 
    // ========================================================================
  private:
    // ========================================================================    
    typedef std::vector<std::string> Strings ;
    /// the particles to be selected 
    Strings      m_particles  ; // the particles to be selected 
    // ========================================================================
    /// the maximal printout depth for RC-particles 
    int          m_rc_depth   ; // the maximal depth for RC-particles 
    /// the printout mode for RC-particles 
    int          m_rc_mode    ; // the printout mode for RC-particles 
    /// print vertex info ? 
    bool         m_rc_vertex  ; // print vertex info ?
    /// format for the mass     (RC)
    std::string  m_rc_fmt_m   ; // format for the mass (RC)
    /// format for the momentum (RC)
    std::string  m_rc_fmt_p   ; // format for the momentum (RC)
    /// format for the transverse momentum (RC)
    std::string  m_rc_fmt_pt  ; // format for the transverse momentum (RC) 
    /// format for vertex information (RC)
    std::string  m_rc_fmt_v   ; // format for vertex information (RC)
    /// format for doubles  (RC)
    std::string  m_rc_fmt_d   ; // format for doubles (RC)
    /// format for integers (RC)
    std::string  m_rc_fmt_i   ; // format for integers (RC)
    /// the maximal printout depth for <MCC-particles 
    int          m_mc_depth   ; /// the maximal depth for MC-particles 
    /// the printout mode for MC-particles 
    int          m_mc_mode    ; // the printout mode for MC-particles 
    /// print vertex info ? 
    bool         m_mc_vertex  ; // print vertex info ? 
    /// print end-vertex info ? 
    bool         m_mc_vertexe ; // print end-vertex info ? 
    /// print decay vertex info only ? 
    bool         m_mc_vertexd ; // print decay vertex info only ? 
    /// format for the mass  (MC)
    std::string  m_mc_fmt_m   ; // format for the mass  (MC)
    /// format for the momentum (MC)
    std::string  m_mc_fmt_p   ; // format for the momentum (MC)
    /// format for the transverse momentum (MC)
    std::string  m_mc_fmt_pt  ; // format for the transverse momentum (MC)
    /// format for the vertex information (MC)
    std::string  m_mc_fmt_v   ; // format for the vertex information (MC)
    /// format for doubles  (MC)
    std::string  m_mc_fmt_d   ; // format for doubles  (MC)
    /// format for integers (MC)
    std::string  m_mc_fmt_i   ; // format for integers (MC)
    // ========================================================================    
  };
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
/*  standard constructor
 *  @param name algorithm name instance 
 *  @param psvc pointer to service locator 
 */
// ============================================================================
LoKi::DecayTruth::DecayTruth 
( const std::string& name , 
  ISvcLocator*       psvc ) 
  : LoKi::AlgoMC ( name , psvc ) 
  ///
  , m_particles ()
  //
  , m_rc_depth   ( 9          )
  , m_rc_mode    ( 0          )
  , m_rc_vertex  ( false      )
  , m_rc_fmt_m   ( "%|=9.4g|" )
  , m_rc_fmt_p   ( "%|=9.4g|" )
  , m_rc_fmt_pt  ( "%|=9.4g|" )
  , m_rc_fmt_v   ( "%|=9.4g|" )
  , m_rc_fmt_d   ( "%|=9.4g|" )
  , m_rc_fmt_i   ( "%|=4l|"   )
  //
  , m_mc_depth   ( 5          )
  , m_mc_mode    ( 0          )
  , m_mc_vertex  ( false      )
  , m_mc_vertexe ( false      )
  , m_mc_vertexd ( false      )
  , m_mc_fmt_m   ( "%|=9.4g|" )
  , m_mc_fmt_p   ( "%|=9.4g|" )
  , m_mc_fmt_pt  ( "%|=9.4g|" )
  , m_mc_fmt_v   ( "%|=9.4g|" )
  , m_mc_fmt_d   ( "%|=9.4g|" )
  , m_mc_fmt_i   ( "%|=4l|"   )
  //
{
  m_particles.push_back ( "B0"    ) ;
  m_particles.push_back ( "B~0"   ) ;
  m_particles.push_back ( "B+"    ) ;
  m_particles.push_back ( "B-"    ) ;
  m_particles.push_back ( "B_s0"  ) ;
  m_particles.push_back ( "B_s~0" ) ;
  
  declareProperty ( "Particles" , m_particles ) ;
  //
  declareProperty ( "RCdepth"      , m_rc_depth    , 
                    "The printout depth for reconstruted trees"  ) ;                
  declareProperty ( "RCmode"       , m_rc_mode     , 
                    "The printpout mode for reconstructed trees" ) ;
  declareProperty ( "RCvertex"     , m_rc_vertex   , 
                    "Print vertex information?"    ) ;
  declareProperty ( "RCformatM"    , m_rc_fmt_m    ) ;
  declareProperty ( "RCformat4V"   , m_rc_fmt_p    ) ;
  declareProperty ( "RCformatPT"   , m_rc_fmt_pt   ) ;
  declareProperty ( "RCformat3V"   , m_rc_fmt_v    ) ;
  declareProperty ( "RCformatD"    , m_rc_fmt_d    ) ;
  declareProperty ( "RCformatI"    , m_rc_fmt_i    ) ;
  //
  declareProperty ( "MCdepth"      , m_mc_depth    ,
                    "The printout depth for Monte Carlo trees" ) ;
  declareProperty ( "MCmode"       , m_mc_mode     ,
                    "The printpout mode for Monte Carlo trees" ) ;
  declareProperty ( "MCvertex"     , m_mc_vertex   ,
                    "Print vertex information?"    ) ;
  declareProperty ( "MCvertexe"    , m_mc_vertexe  ,
                    "Print end-vertex information?") ;
  declareProperty ( "MCvertexD"    , m_mc_vertexd  ,
                    "Treat only DECAY-vertices?"   ) ;
  declareProperty ( "MCformatM"    , m_mc_fmt_m    ) ;
  declareProperty ( "MCformat4V"   , m_mc_fmt_p    ) ;
  declareProperty ( "MCformatPT"   , m_mc_fmt_pt   ) ;
  declareProperty ( "MCformat3V"   , m_mc_fmt_v    ) ;
  declareProperty ( "MCformatD"    , m_mc_fmt_d    ) ;
  declareProperty ( "MCformatI"    , m_mc_fmt_i    ) ;
}
// ============================================================================
/*  analyse the event 
 *  @see LoKi::Algo
 *  @return status code 
 */
// ============================================================================
StatusCode LoKi::DecayTruth::analyse   () 
{
  using namespace LoKi          ;
  using namespace LoKi::Cuts    ;
  using namespace LHCb ;
  using namespace LoKi::Types;
  
  typedef LoKi::PhysTypes::Container RCCont ;
  typedef LoKi::MCTypes::MCContainer MCCont ;
  typedef RCCont::const_iterator     RCIter ;
  typedef MCCont::iterator           MCIter ;
  
  
  Cut cut = PNONE ;
  for ( Strings::const_iterator it = m_particles.begin() ; 
        m_particles.end() != it ; ++it ) 
  { cut = cut || ( (*it) == ID ) ; }
  
  if ( msgLevel ( MSG::DEBUG ) ) 
  { debug() << " cut used for selection of RC particles '" 
            << cut.printOut() << "'" << endreq ; }
  
  /// select all recostructed "interesting" particles: 
  Range RC = select( "RC" , cut ) ;
  
  if ( RC.empty() ) 
  { return Warning(" No   Particles are selected " , StatusCode::SUCCESS ) ; }

  /// select all Monte Carlo particles 
  LoKi::Types::MCRange MC = mcselect ( "MC" , MCALL ) ;
  if ( MC.empty() ) 
  { return Warning(" No MCParticles are selected " , StatusCode::SUCCESS ) ; }
  
  // create the printer for RC-decays  
  DecayChain rcPrinter 
    ( m_rc_depth                   , 
      m_rc_vertex                  , 
      false                        , 
      (DecayChain::Mode) m_rc_mode ) ;
  
  rcPrinter.setFmt_M   ( m_rc_fmt_m  ) ;
  rcPrinter.setFmt_PT  ( m_rc_fmt_pt ) ;
  rcPrinter.setFmt_P   ( m_rc_fmt_p  ) ;
  rcPrinter.setFmt_V   ( m_rc_fmt_v  ) ;
  rcPrinter.setFmt_D   ( m_rc_fmt_d  ) ;
  rcPrinter.setFmt_I   ( m_rc_fmt_i  ) ;
  
  // create the printer for MC-decays  
  DecayChain mcPrinter 
    ( m_mc_depth                   , 
      m_mc_vertex                  , 
      m_mc_vertexe                 , 
      (DecayChain::Mode) m_mc_mode ) ;
  // 
  mcPrinter.setFmt_M   ( m_mc_fmt_m   ) ;
  mcPrinter.setFmt_PT  ( m_mc_fmt_pt  ) ;
  mcPrinter.setFmt_P   ( m_mc_fmt_p   ) ;
  mcPrinter.setFmt_V   ( m_mc_fmt_v   ) ;
  mcPrinter.setFmt_D   ( m_mc_fmt_d   ) ;
  mcPrinter.setFmt_I   ( m_mc_fmt_i   ) ;
  //
  mcPrinter.setVertexD ( m_mc_vertexd ) ;
  
  LoKi::MCMatch match = mcTruth("DecTruth") ;
  
  // start the loop over reconstructed 'Particles'
  for ( Range::iterator irc = RC.begin() ; RC.end() != irc ; ++irc ) 
  {
    const size_t index =  irc - RC.begin() + 1 ;
    const Particle* rc = *irc ;
    if ( 0 == rc ) { continue ; }
    MsgStream& log = always() ;
    
    log << std::string(80,'*') << endreq ;
    log << " Reconstructed "
        << LoKi::Particles::nameFromPID( rc->particleID() ) 
        << "  #" << index << "/" << RC.size() ;
    if ( rc->hasKey() ) { log << " (key:" << rc->key() << ")" ; }
    log << std::endl ;
    rcPrinter.print ( rc , log.stream() , '\n'       , PALL  , PNONE ) ;
    log << endreq ;
    
    // get all daughters of this 'Particle'  
    RCCont children ;
    LoKi::Extract::particles( rc , std::back_inserter( children ) ) ;
    
    // loop over all daughter partucles (NB: 'self' is included!)
    for ( RCIter d =  children.begin() ; children.end() != d ; ++d ) 
    {
      const Particle* child = *d ;
      if ( 0 == child ) { continue ; }
      
      // get all Monte Carlo particles which 'matched' with this daugter particle 
      MCCut  rcChild   = RCTRUTH ( match , child ) ;
      MCCont mcCont ;
      LoKi::select
        ( MC.begin () , 
          MC.end   () , 
          std::back_inserter ( mcCont ) , rcChild ) ;
      
      // transform this container into container of trees:
      MCIter trees = LoKi::MCTrees::buildTrees ( mcCont.begin() , mcCont.end() ) ;
      
      const size_t indx   = d - children.begin() + 1 ;
      const size_t nTrees = trees - mcCont.begin() ;
      
      MsgStream& log = always() ;
      log << " Inspect reconstructed daughter particle " 
          << " '" << LoKi::Particles::nameFromPID( child->particleID() ) << "' " 
          << indx << "/" << children.size()  ;
      if ( child->hasKey() ) { log << " (key:" << child->key() << "): " ; }
      log << " #'matched' MC trees "          << nTrees 
          << " (#'matched' MC particles "     << mcCont.size() << ")" 
          << std::endl ;
      // print all matched MC trees and colorize/mark the 'matched' particles  
      mcPrinter.print 
        ( mcCont.begin() , trees , log.stream() , '\n' , MCALL , rcChild ) ;
      log << endreq ;
    } // end of loop over children
    always () << std::string(80,'*') << endreq ;
  } // end of loop over reconstructed & selected particles
  // ==========================================================================
  return StatusCode::SUCCESS ;
  // ==========================================================================
}
// ============================================================================
///  static factory for algorithm instantiation
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LoKi,DecayTruth);
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
