// $Id: PV2MCAlg.cpp,v 1.1 2008-06-25 17:27:39 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <utility>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
// ============================================================================
// Relations 
// ============================================================================
#include "Relations/IRelation.h"
#include "Relations/RelationWeighted1D.h"
#include "Relations/RelationWeighted2D.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/RecVertex.h"
#include "Event/MCVertex.h"
// ============================================================================
// Kernel? 
// ============================================================================
#include "Kernel/PV2MC.h"
#include "Kernel/MC2Collision.h"
// ============================================================================
// LoKi & LoKiMC
// ============================================================================
#include "LoKi/ILoKiSvc.h"
#include "LoKi/Geometry.h"
#include "LoKi/select.h"
#include "LoKi/PhysTypes.h"
#include "LoKi/Vertices1.h"
#include "LoKi/MCVertexCuts.h"
// ============================================================================
/** @file
 *
 *  Implementation file for class LoKi::PV2MCAlg
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
 *  @date 2006-03-19 
 */
// ============================================================================
namespace LoKi 
{
  /** @class PV2MCAlg
   *
   *  helper algorthm to build LHCb::PrimVertex <---> LHCb::MCVertex relations 
   *  
   *  It associates the LHCb::PrimVertex "vP" to (primary)LHCb::MCVertex 
   *  with the weight, which is a pair of
   *    - number of true MC tracks from the given collision 
   *      used in the reconstruction of primary vertex
   *    - chi2 of the distance in between recontricted and MC vertices 
   *
   *  @see LHCb::MCVertex
   *  @see LHCb::PrimVertex
   *  @see LHCb::PV2MC
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@lapp.in2p3.fr
   *  @date 2005-07-13
   */
  // ==========================================================================
  class PV2MCAlg : public GaudiAlgorithm 
  {
    // ========================================================================
    /// the friend factory for instantiation 
    friend class AlgFactory<PV2MCAlg>;
    // ========================================================================
  public:
    // ========================================================================
    /// initialization of the algorithm
    virtual StatusCode intialize () 
    {
      StatusCode sc = GaudiAlgorithm::initialize() ;
      if ( sc.isFailure() ) { return sc ; }
      // load LoKi service 
      svc<LoKi::ILoKiSvc>( "LoKiSvc" ) ;
      return StatusCode::SUCCESS ;
    } 
    /// execution of the algorithm
    virtual StatusCode execute() ;
    // ========================================================================
  protected:
    // ========================================================================
    /**standard constructor 
     * @param name algorithm instance name 
     * @param pSvc pointer to Service Locator 
     */
    PV2MCAlg 
    ( const std::string& name , 
      ISvcLocator*       pSvc ) 
      : GaudiAlgorithm ( name , pSvc )
      , m_primaries    ( LHCb::RecVertexLocation::      Primary )
      , m_vertices     ( LHCb::MCVertexLocation::       Default )
      , m_collision    ( LHCb::MC2CollisionLocation::   Default )
      , m_track2MC     ( "Relations/" + LHCb::TrackLocation::Default )
      , m_output1      ( LHCb::PV2MCLocation::          Default ) 
      , m_output2      ( LHCb::PV2CollisionLocation::   Default ) 
    {
      declareProperty ( "Primaries"           , m_primaries ) ;
      declareProperty ( "MCVertices"          , m_vertices  ) ;
      declareProperty ( "MC2Collision"        , m_collision ) ;
      declareProperty ( "Track2MC"            , m_track2MC  ) ;
      declareProperty ( "OutputPV2MC"         , m_output1   ) ;
      declareProperty ( "OutputPV2Collision"  , m_output2   ) ;
      //
      setProperty     ( "StatPrint"    , "true"      ) ;
      //
    }
    // destructor, virtual and protected 
    virtual ~PV2MCAlg() {} ;
    // ========================================================================
  private:
    // ========================================================================
    // default constructor is disabled 
    PV2MCAlg            () ;
    // copy constructor is disabled 
    PV2MCAlg            ( const PV2MCAlg& ) ;
    // assignement operator is disabled 
    PV2MCAlg& operator= ( const PV2MCAlg& ) ;
    // ========================================================================
  private:
    // ========================================================================
    // TES address of primary vertices
    std::string  m_primaries ; ///< TES address of primary vertices
    // TES address of MC  vertices
    std::string  m_vertices  ; ///< TES address of MC vertices
    // TES address of MC->Collision links
    std::string  m_collision ; ///< TES address of MC->Collision links
    // TES address of Track->MC links
    std::string  m_track2MC  ; ///< TES address of Track->MC links
    // TES location of output relation table 
    std::string  m_output1   ; ///< TES location of output relation table 
    // TES location of output relation table 
    std::string  m_output2   ; ///< TES location of output relation table 
    // ========================================================================
  };
} // end of namespace LoKi
// ============================================================================
namespace 
{
  /// the actual type of relation table in TES
  typedef LHCb::RelationWeighted2D
  <LHCb::RecVertex,LHCb::MCVertex,LHCb::PV2MCWeight>     Table1 ;
  /// the actual type of relation table in TES
  typedef LHCb::RelationWeighted2D
  <LHCb::RecVertex,LHCb::GenCollision,LHCb::PV2MCWeight> Table2 ;
}
// ============================================================================
// execution of the algorithm
// ============================================================================
StatusCode LoKi::PV2MCAlg::execute() 
{ 
  using namespace LoKi::Types ;
  using namespace LoKi::Cuts  ;
  
  // type of container for primary MC-vertices 
  typedef std::vector<const LHCb::MCVertex*>   PRIMARIES ;
  
  // create the relation tables and register them into TES 
  Table1* table1 = new Table1() ;
  Table2* table2 = new Table2() ;
  put ( table1 , m_output1 ) ;
  put ( table2 , m_output2 ) ;
  
  // get the primary vertices from TES
  const LHCb::RecVertices* primaries = get<LHCb::RecVertices>( m_primaries ) ;
  if ( 0 == primaries ) { return StatusCode::FAILURE ; }         // RETURN 
  
  // get MC-vertices from TES
  const LHCb::MCVertices*   vertices = get<LHCb::MCVertices>   ( m_vertices  ) ;
  if ( 0 == vertices  ) { return StatusCode::FAILURE ; }         // RETURN 

  // get Vertex -> Collision links
  const LHCb::MCVertex2Collision* collision =  
    get<LHCb::MCVertex2Collision> ( m_collision ) ;
  if ( 0 == collision ) { return StatusCode::FAILURE ; }         // RETURN 
  
  // get Track  -> MC links 
  typedef const IRelationWeighted<LHCb::Track,LHCb::MCParticle,double> TrackTable ;
  //typedef const IRelation<LHCb::Track,LHCb::MCParticle> TrackTable ;
  typedef TrackTable::Range                             TrackRange ;
  const TrackTable* trackMC = get<TrackTable> ( m_track2MC ) ;
  if ( 0 == trackMC ) { return StatusCode::FAILURE ; }           // RETURN 
  
  // from all MC-vertices select only the primaries
  PRIMARIES prims0 ;
  LoKi::select 
    ( vertices->begin () , 
      vertices->end   () , 
      std::back_inserter ( prims0 ) , 
      MCPRIMARY ) ;
  if ( prims0.empty()  )
  { Warning ( " Empty container of primary MC-vertices! " ) ; }
  
  // select only "good" primary vertices, associated with pp-collisions 
  PRIMARIES prims ;
  prims.reserve ( prims0.size() ) ;
  for ( PRIMARIES::const_iterator ip = prims0.begin() ; prims0.end() != ip ; ++ip ) 
  {
    const LHCb::MCVertex* p = *ip ;
    if ( 0 == p    ) { continue ; }                                 // CONTINUE 
    LHCb::MCVertex2Collision::Range r = collision->relations( p ) ;
    if ( r.empty() ) { continue ; }                                 // CONTINUE 
    const LHCb::GenCollision* c = r.front().to() ;
    if ( 0 == c    ) { continue ; }                                 // CONTINUE 
    prims.push_back ( p ) ;
  }
  if ( prims.size() != prims0.size() ) 
  { Warning ( " Some primary MC-vertices are not good "         ) ; }
  if ( prims.empty() ) 
  { Warning ( " Empty container of valid primary MC-vertices! " ) ; }
  
  typedef std::map<const LHCb::MCVertex*,size_t> MAP    ;
  typedef const SmartRefVector<LHCb::Track>      TRACKS ;
  
  // double loop 
  for ( LHCb::RecVertices::const_iterator ipv = primaries->begin() ;
        primaries->end() != ipv ; ++ipv ) 
  {
    const LHCb::RecVertex* pv = *ipv ;
    if ( 0 == pv ) { continue ; }                                  // CONTINUE 
    MAP _map ;
    const TRACKS& trs = pv->tracks() ;
    for ( TRACKS::const_iterator itr = trs.begin() ; trs.end() != itr ; ++itr )
    {
      const LHCb::Track* track = *itr ;
      if ( 0 == track ) { continue ; }                             // CONTINUE 
      // get MC-truth for the given track 
      TrackRange r = trackMC->relations( track ) ;
      // ghost ? 
      if ( r.empty()  ) { continue ; }                             // CONTINUE 
      const LHCb::MCParticle* mcp = r.front().to() ;
      if ( 0 == mcp   ) { continue ; }                             // CONTINUE
      // get the primary vertex!!!
      const LHCb::MCVertex* mcpv = mcp -> primaryVertex() ;
      if ( 0 == mcpv ) { continue ; }                              // CONTINUE 
      // fill the temporary map (count number of tracks from MC-vertices)
      _map[mcpv] += 1 ;
    } 
    
    for ( PRIMARIES::const_iterator imc = prims.begin() ; prims.end() != imc ; ++imc )
    {
      const LHCb::MCVertex* mc = *imc ;
      if ( 0 == mc ) { continue ; }                                // CONTINUE 
      
      // chi2 distance from the given point 
      VFun vd = LoKi::Vertices::VertexChi2Distance ( mc->position () ) ;
      
      // find the chi2  of distance 
      const double chi2 = vd ( pv ) ;
      MAP::const_iterator ic = _map.find( mc ) ;
      const size_t nTrack = _map.end() == ic ? 0 : _map[mc] ;
      
      LHCb::PV2MCWeight weight ( nTrack , chi2 ) ;
      
      // Fill the relation table LHCb::RecVertes <--> LHCb::MCParticle
      table1 -> relate ( pv , mc , weight ) ;
      
      // it has been checked already that it is OK!!!
      const LHCb::GenCollision* c = 
        collision->relations( mc ).front().to() ;
      
      // Fill the relation table LHCb::PrimVertes <--> LHCb::GenCollision
      table2->relate ( pv , c , weight ) ;
      
    } // end of loop over Monte Carlo primary vertices    
  }  // end of loop over reconstructed primary vertices
  // DECORATIONS:
  {
    // total number of established links 
    const size_t links = table1->relations().size() ;
    // make a statistics 
    counter ( "#PV->MC" ) += links ;
    if ( msgLevel ( MSG::DEBUG ) ) 
    { debug() << " Number of 'PV->MC' links : " << links  << endreq ; }
  }
  {
    // total number of established links 
    const size_t links = table2->relations().size() ;
    // make a statistics 
    counter ( "#PV->Collision" ) += links ;
    if ( msgLevel ( MSG::DEBUG ) ) 
    { debug() << " Number of 'PV->Colllsion' links : " << links  << endreq ; }
  }
  
  return StatusCode::SUCCESS ;
} 
// ============================================================================
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LoKi,PV2MCAlg) ;
// ============================================================================
// The END
// ============================================================================

