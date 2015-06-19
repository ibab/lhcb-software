// $Id$
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision$
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.16  2008/11/04 12:43:47  ibelyaev
//  remove opts-files & provide python configurables
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
// LoKi
// ============================================================================
// Relations 
// ============================================================================
#include "Relations/Relation2D.h"
// ============================================================================
// LoKiGenMC
// ============================================================================
#include "Kernel/HepMC2MC.h"
#include "Kernel/IHepMC2MC.h"
// ============================================================================
// LoKiCore
// ============================================================================
#include "LoKi/ILoKiSvc.h"
#include "LoKi/Inherits.h"
// ============================================================================
// Boost
// ============================================================================
#include "boost/static_assert.hpp"
// ============================================================================
/** @file
 *
 *  Implementation file for class LoKi::HepMC2MCAlg
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
  /** @class HepMC2MCAlg
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
  // ==========================================================================
  class HepMC2MCAlg : public GaudiAlgorithm 
  {
    // ========================================================================
    /// friend factory fo instantiations 
    friend class AlgFactory<LoKi::HepMC2MCAlg>;
    // ========================================================================
  public:
    /// initialization of the algorithm
    virtual StatusCode initialize () 
    {
      StatusCode sc = GaudiAlgorithm::initialize() ;
      if ( sc.isFailure() ) { return sc ; }
      // load LoKi service 
      svc<LoKi::ILoKiSvc>( "LoKiSvc" ) ;
      // load the tool 
      m_tool = tool<IHepMC2MC> ( m_toolName , this ) ;
      // it must be private tool! 
      if ( m_tool->parent() != this ) 
      { return Error ("HepMC2MC tool must be private here!")  ; }
      // (re)set properties of private tool: 
	  sc = Gaudi::Utils::setProperty ( m_tool , "HepMCEvents"    , m_hepmcs, "" ) ;
      if ( sc.isFailure() ) 
      { return Error ( "Unable to (re)set property 'HepMCEvents' ", sc )  ; }
      sc = Gaudi::Utils::setProperty ( m_tool , "MCParticles"    , m_mcps, ""   ) ;
      if ( sc.isFailure() ) 
      { return Error ( "Unable to (re)set property 'MCParticles' ", sc )  ; }
      sc = Gaudi::Utils::setProperty ( m_tool , "Cut1"           , m_cut1, ""   ) ;
      if ( sc.isFailure() ) 
      { return Error ( "Unable to (re)set property 'Cut1' "      , sc )  ; }
      sc = Gaudi::Utils::setProperty ( m_tool , "Cut2"           , m_cut2, ""   ) ;
      if ( sc.isFailure() ) 
      { return Error ( "Unable to (re)set property 'Cut2' "      , sc )  ; }
      sc = Gaudi::Utils::setProperty ( m_tool , "VertexCut"      , m_vcut, ""   ) ;
      if ( sc.isFailure() ) 
      { return Error ( "Unable to (re)set property 'VertexCut' " , sc )  ; }
      //
      return StatusCode::SUCCESS ;
    } 
    /// execution of the algorithm
    virtual StatusCode execute  () ;
    /// finalize of the algorithm
    virtual StatusCode finalize ()
    {
      m_tool = 0 ;
      return GaudiAlgorithm::finalize () ;
    }
    // ========================================================================
  protected:
    // ========================================================================
    /**standard constructor 
     * @param name algorithm instance name 
     * @param pSvc pointer to Service Locator 
     */
    HepMC2MCAlg 
    ( const std::string& name , 
      ISvcLocator*       pSvc ) 
      : GaudiAlgorithm ( name , pSvc )
      , m_hepmcs    (  1 , LHCb::HepMCEventLocation:: Default )
      , m_mcps      (  1 , LHCb::MCParticleLocation:: Default )
      , m_cut1      ( 0.1  * Gaudi::Units::MeV * Gaudi::Units::MeV )
      , m_cut2      ( 1.e-4                   )
      , m_vcut      ( 100  * Gaudi::Units::micrometer )
      , m_output    (      LHCb::HepMC2MCLocation::   Default )
      , m_toolName  ( "LoKi::HepMC2MC/HepMC2MC" )
      // 
        , m_tool ( 0 )
    {
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
      declareProperty 
        ( "Output"  , 
          m_output  ,
          "TES location of the output HepMC<---> relatiuon table" ) ;
      declareProperty 
        ( "HepMC2MC"  , 
          m_toolName  ,
          "Tool type/name for building HepMC<---> Mc relations " ) ;
      //
    }
    // destructor, virtual and protected 
    virtual ~HepMC2MCAlg() {} ;
    // ========================================================================
  private:
    // ========================================================================
    /// default constructor is disabled 
    HepMC2MCAlg            () ;              // default constructor is disabled 
    /// copy constructor is disabled 
    HepMC2MCAlg            ( const HepMC2MCAlg& ) ;          // no copy allowed 
    /// assignement operator is disabled 
    HepMC2MCAlg& operator= ( const HepMC2MCAlg& ) ;   // no assignement allowed 
    // ========================================================================
  private:
    // ========================================================================
    typedef std::vector<std::string> Addresses ;
    Addresses    m_hepmcs ;
    Addresses    m_mcps   ;
    double       m_cut1   ;
    double       m_cut2   ;
    double       m_vcut   ;
    std::string  m_output ;
    // ========================================================================
  private:
    // ========================================================================
    std::string m_toolName ;
    IHepMC2MC*  m_tool     ;
    // ========================================================================
  };
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
namespace 
{
  // ==========================================================================
  /// the actual type of relation table 
  typedef LHCb::Relation2D<HepMC::GenParticle*,LHCb::MCParticle> Table  ;
  /// check the type:
  BOOST_STATIC_ASSERT(INHERITS(Table,LHCb::HepMC2MC2D)) ;
  // ==========================================================================
} 
// ============================================================================
// execution of the algorithm
// ============================================================================
StatusCode LoKi::HepMC2MCAlg::execute() 
{
  // check the tool 
  Assert  ( 0 != m_tool , "IHepMC2MC* points to NULL!" ) ;
  
  // get the table form the tool 
  const IHepMC2MC::HepMC2MC* hepmc = m_tool -> hepMC2MC () ;
  
  Assert  ( 0 != hepmc , "IHepMC2MC::HepMC2MC* points to NULL!" ) ;

  // create the relation table and register it into TES 
  Table* table = new Table ( *hepmc ) ;
  put ( table , m_output ) ;
  
  // DECORATIONS:
  
  if ( table->relations().empty() ) 
  { Warning ( "The relation table is empty!" ) ; }
  
  // total number of established links 
  const size_t links = table->relations().size() ;
  // make a statistics 
  if ( printStat() || msgLevel( MSG::DEBUG ) ) 
  { counter ( "#HepMC->MC" ) += links ; }
  //
  if ( msgLevel ( MSG::DEBUG ) )
  { debug() << " Number of 'HepMC<-->MC' links : " << links  << endreq ; }
  
  return StatusCode::SUCCESS;
}
// ============================================================================
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LoKi,HepMC2MCAlg)
// ============================================================================
// The END 
// ============================================================================

