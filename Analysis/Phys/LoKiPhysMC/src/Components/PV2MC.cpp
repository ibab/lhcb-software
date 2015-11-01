// $Id$
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <utility>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiTool.h"
// ============================================================================
// Relations 
// ============================================================================
#include "Relations/IRelation.h"
#include "Relations/IRelationWeighted.h"
#include "Relations/IRelationWeighted2D.h"
#include "Relations/Relation2Weighted.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/RecVertex.h"
#include "Event/MCVertex.h"
#include "Event/GenCollision.h"
// ============================================================================
// DaVinciMCKernel
// ============================================================================
#include "Kernel/IMC2Collision.h"
#include "Kernel/MC2Collision.h"
#include "Kernel/IPV2MC.h"
#include "Kernel/PV2MC.h"
#include "Kernel/Track2MC.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/ILoKiSvc.h"
#include "LoKi/Geometry.h"
#include "LoKi/select.h"
#include "LoKi/PhysTypes.h"
#include "LoKi/Vertices1.h"
#include "LoKi/MCVertexCuts.h"
#include "LoKi/Inherits.h"
// ============================================================================
// Boost 
// ============================================================================
#include "boost/static_assert.hpp"
// ============================================================================
/** @file
 *
 *  Implementation file for class LoKi::PV2MC
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
  // ==========================================================================
  /** @class PV2MC
   *
   *  helper tool to access LHCb::RecVertex <---> LHCb::MCVertex and
   *  LHCb::RecVertex <--> LHCb::GenCollision relations 
   *  
   *  @see IPV2MC
   *  @see LHCb::RecVertex
   *  @see LHCb::GenCollision
   *  @see LHCb::MCVertex
   *  @see LHCb::PV2MC
   *  @see LoKi_PV2MCAlg
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-03-18
   */
  // ============================================================================
  class PV2MC
    : public virtual IPV2MC            ,
      public virtual IIncidentListener ,
      public         GaudiTool  
  {
    // ========================================================================
    // friend factory for instantiation
    friend class ToolFactory<LoKi::PV2MC> ;
    // ========================================================================
  public:
    // ========================================================================
    /** return a relation table for 
     *  LHCb::RecVertex ---> LHCb::MCVertex relations
     *
     *  @code
     * 
     *  typedef IPV2MC::PV2MC Table ;
     *  typedef Table::Range  Range ;
     *
     *  // tool itself 
     *  const IPV2MC* tool = ... ;
     *  // get a table form the tool 
     *  const Table* table = tool->pv2MC() ;
     *  // primary vertex:
     *  const LHCb::RecVertex* prim = ... ;
     *  // get all related MC-vertices
     *  Range links = table->relations( prim ) ;
     *  // make  a loop over all links:
     *  for ( Range::iterator ilink = links.begin() ; 
     *                        links.end() != ilink ; ++ilink ) 
     *   {
     *      // (primary) MC-vertex:
     *      const LHCb::MCVertex* mc = ilink->to() ;
     *      // number of true MC track from the given primary MC:
     *      const int    nTrack = ilink->weight().first    ;
     *      // chi2 of th editance in between MC and RC vertex:
     *      const double chi2   = ilink->weight().second   ;
     *   }
     *
     *  @endcode 
     *
     *  @see LHCb::PV2MC
     *  @see LHCb::RecVertex
     *  @see LHCb::MCVertex
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-03-19
     */
    virtual const IPV2MC::PV2MC* pv2MC() const 
    {
      // the table is valid , get it! 
      if ( valid1 () ) { m_table1.direct() ; }
      // build & validate 
      StatusCode sc = build1 () ;
      // the final checks 
      Assert ( sc.isSuccess () , "Unable to build the table_1" , sc ) ;
      Assert ( valid1()        , "The table_1 is invalid!"          ) ;
      //
      return m_table1.direct() ;
    } 
    // ========================================================================
    /** return a relation table for 
     *  (primary)LHCb::MCVertex --> LHCb::RecVertex relations
     *
     *  @see LHCb::MC2PV
     *  @see LHCb::RecVertex
     *  @see LHCb::MCVertex
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-03-19
     */
    virtual const IPV2MC::MC2PV* mc2PV() const 
    {
      // the table is valid , get it! 
      if ( valid1 () ) { m_table1.inverse () ; }
      // build & validate 
      StatusCode sc = build1 () ;
      // the final checks 
      Assert ( sc.isSuccess () , "Unable to build the table_1" , sc ) ;
      Assert ( valid1()        , "The table_1 is invalid!"          ) ;
      //
      return m_table1.inverse () ;
    }
    // ========================================================================
    /** return a relation table for 
     *  LHCb::RecVertex ---> LHCb::GenCollision relations
     *
     *  @code
     * 
     *  typedef IPV2MC::PV2Collision Table ;
     *  typedef Table::Range         Range ;
     *
     *  // tool itself 
     *  const IPV2MC* tool = ... ;
     *  // get a table form the tool 
     *  const Table* table = tool->pv2Collision() ;
     *  // primary vertex:
     *  const LHCb::RecVertex* prim = ... ;
     *  // get all related pp-Collisions
     *  Range links = table->relations( prim ) ;
     *  // make  a loop over all links:
     *  for ( Range::iterator ilink = links.begin() ; 
     *                        links.end() != ilink ; ++ilink ) 
     *   {
     *      // pp-Collision
     *      const LHCb::GenCollision* collision = ilink->to() ;
     *      // number of true MC track from the given primary MC:
     *      const int    nTrack = ilink->weight().first    ;
     *      // chi2 of the distance in between MC and RC vertex:
     *      const double chi2   = ilink->weight().second   ;
     *   }
     *
     *  @endcode 
     *
     *  @see LHCb::PV2Collision
     *  @see LHCb::RecVertex
     *  @see LHCb::GenCollision
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-03-19
     */
    virtual const IPV2MC::PV2Collision* pv2Collision() const 
    {
      // the table is valid , get it! 
      if ( valid2 () ) { m_table2.direct() ; }
      // build & validate 
      StatusCode sc = build2 () ;
      // the final checks 
      Assert ( sc.isSuccess () , "Unable to build the table_2" , sc ) ;
      Assert ( valid2()        , "The table_2 is invalid!"          ) ;
      //
      return m_table2.direct() ;
    } 
    // ========================================================================
    /** return a relation table for 
     *  LHCb::GenCollision --> LHCb::RecVertex relations
     *
     *  @see LHCb::Collision2PV
     *  @see LHCb::RecVertex
     *  @see LHCb::GenCollision
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-03-19
     */
    virtual const IPV2MC::Collision2PV* collision2PV() const 
    {
      // the table is valid , get it! 
      if ( valid2 () ) { m_table2.inverse () ; }
      // build & validate 
      StatusCode sc = build1 () ;
      // the final checks 
      Assert ( sc.isSuccess () , "Unable to build the table_2" , sc ) ;
      Assert ( valid2()        , "The table_2 is invalid!"          ) ;
      //
      return m_table2.inverse () ;
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
      
      return StatusCode::SUCCESS ;
    }
    /// handle new incident : invalidate the tables 
    virtual void handle ( const Incident& ) 
    {
      m_valid1 = false    ;
      m_table1 . clear () . ignore () ;
      m_valid2 = false    ;
      m_table2 . clear () . ignore () ;
    }
    // ========================================================================
  protected:
    // ========================================================================
    /** standart constructor 
     *  @param type tool type 
     *  @param name tool name 
     *  @param parent parent object
     */
    PV2MC 
    ( const std::string& type   , 
      const std::string& name   , 
      const IInterface*  parent )
      : GaudiTool ( type, name , parent )
      ///
      , m_primaries  ( LHCb::RecVertexLocation::      Primary )
      , m_vertices   ( LHCb::MCVertexLocation::       Default )
      , m_track2MC   ( LHCb::Track2MCLocation::       Default )
      , m_mc2colName ( "LoKi::MC2GenCollision/MC2GenCollision" )
      //
      , m_mc2col    ( (IMC2Collision*) 0 )
      ///
      , m_table1    ( ) 
      , m_table2    ( )
      , m_valid1    ( false ) 
      , m_valid2    ( false )
      ///
    {
      declareInterface<IIncidentListener> ( this ) ; 
      declareInterface<IPV2MC>            ( this ) ; 
      // 
      declareProperty 
        ( "Primaries"  , 
          m_primaries  ,
          "The TES location of Primary Vertices     (LHCb::RecVertex)") ;
      declareProperty 
        ( "MCVertices" , 
          m_vertices   , 
          "The TES location of Monte Carlo Vertices (LHCb::MCVertex)") ;
      declareProperty 
        ( "Track2MC"   , 
          m_track2MC   , 
          "The TES location of Track->MC table      (LHCb::Track2MC)") ;
      declareProperty 
        ( "MC2Collision" , 
          m_mc2colName   ,
          "Type/nemae of IMC2Collision tool" ) ;
      //
      setProperty     ( "StatPrint"    , true ) ;
      //
    }
    /// destructor (virual and protected)
    virtual ~PV2MC () {} 
    // ========================================================================
  private:
    // ========================================================================
    // default constructor  is disabled 
    PV2MC ()  ;
    // copy constructor     is disabled 
    PV2MC ( const PV2MC& ) ;
    // assignement operator is disabled 
    PV2MC& operator= ( const PV2MC& ) ;
    // ========================================================================
  protected:
    // ========================================================================
    /// build the first table 
    StatusCode build1 () const  ;
    /// build the second table 
    StatusCode build2 () const  ;
    /// get the validity flag for the first  table 
    bool valid1 () const { return m_valid1 ; }
    /// get the validity flag for the second table 
    bool valid2 () const { return m_valid2 ; }    
    // ========================================================================
  private:
    // ========================================================================
    /// the location of the primary vertices 
    std::string m_primaries  ;          // the location of the primary vertices 
    /// the location of the Monte-Carlo vertices 
    std::string m_vertices   ;          // the location of Monte Carlo vertices 
    /// location of track -> MC relation table 
    std::string m_track2MC   ;        // location of track -> MC relation table 
    // ========================================================================
  private:
    // ========================================================================    
    /// the type/name of tool for MCVertex <---> GenCollision relations 
    std::string m_mc2colName ;      // the tool for MCVertex <---> GenCollision
    /// the tool for MC Vertex <---> GenCollision links 
    mutable LoKi::Interface<IMC2Collision> m_mc2col   ;
    // ========================================================================
  private:
    // ========================================================================
    // the actual inte
    typedef Relations::Relation2Weighted<LHCb::RecVertex    ,
                                         LHCb::MCVertex     ,
                                         LHCb::Relations_::PV2MCWeight> Table1 ;
    typedef Relations::Relation2Weighted<LHCb::RecVertex    ,
                                         LHCb::GenCollision ,
                                         LHCb::Relations_::PV2MCWeight> Table2 ;
    /// check the types 
    BOOST_STATIC_ASSERT(INHERITS(Table1,LHCb::PV2MC2D)) ;
    BOOST_STATIC_ASSERT(INHERITS(Table2,LHCb::PV2Collision2D)) ;    
    // =========================================================================
    /// the actual relation table 
    mutable Table1 m_table1 ;                       // the actual relation table 
    /// the actual relation table 
    mutable Table2 m_table2 ;                      // the actual relation table 
    /// the validity flag 
    mutable bool   m_valid1 ;                              // the validity flag 
    /// the validity flag 
    mutable bool   m_valid2 ;                              // the validity flag
    // ========================================================================
  };
  // ==========================================================================
} //end of namespace LoKi
// ============================================================================
// build the first table 
// ============================================================================
StatusCode LoKi::PV2MC::build1 () const  
{
  // invalidate the tables 
  m_valid1 = false    ;
  m_table1 . clear () . ignore () ;
  m_valid2 = false    ;
  m_table2 . clear () . ignore () ;
  
  using namespace LoKi::Types ;
  using namespace LoKi::Cuts  ;
  
  // type of container for primary MC-vertices 
  typedef std::vector<const LHCb::MCVertex*>   PRIMARIES ;
  
  // get the primary vertices from TES
  const LHCb::RecVertices* primaries = get<LHCb::RecVertices>  ( m_primaries ) ;
  if ( 0 == primaries ) { return StatusCode::FAILURE ; }         // RETURN 
  
  // get MC-vertices from TES
  const LHCb::MCVertices*   vertices = get<LHCb::MCVertices>   ( m_vertices  ) ;
  if ( 0 == vertices  ) { return StatusCode::FAILURE ; }         // RETURN
  
  // the actual table for track-> relations 
  const LHCb::Track2MC* track2MC = get<LHCb::Track2MC>         ( m_track2MC  ) ;
  if ( 0 == track2MC  ) { return StatusCode::FAILURE ; }         // RETURN
  
  // from all MC-vertices select only the primaries
  PRIMARIES prims0 ;
  LoKi::select 
    ( vertices->begin () , 
      vertices->end   () , 
      std::back_inserter ( prims0 ) , 
      MCPRIMARY ) ;
  if ( prims0.empty()  )
  { Warning ( " Empty container of primary MC-vertices! " ) ; }
  
  // get MC  <---> collision tool (if needed) 
  if ( !m_mc2col ) 
  { m_mc2col = tool<IMC2Collision>  ( m_mc2colName , this ) ; }
  
  // get the relation table form the tool 
  typedef IMC2Collision::MCVertex2Collision MC2C ;
  //
  const MC2C* mc2col = m_mc2col -> vertex2collision () ;
  
  // select only "good" primary vertices, associated with pp-collisions 
  PRIMARIES prims ;
  prims.reserve ( prims0.size() ) ;
  for ( PRIMARIES::const_iterator ip = prims0.begin() ; prims0.end() != ip ; ++ip ) 
  {
    const LHCb::MCVertex* p = *ip ;
    if ( 0 == p    ) { continue ; }                                 // CONTINUE 
    LHCb::MCVertex2Collision::Range r = mc2col->relations( p ) ;
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
  
  StatEntity& ghosts       = counter( "ghost eff" ) ;
  const bool  countGhosts  = statPrint() || msgLevel( MSG::DEBUG ) ;
  
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
      LHCb::Track2MC::Range r = track2MC->relations( track ) ;
      // count ghosts 
      if ( countGhosts ) { ghosts += !r.empty() ; }
      // ghost ? 
      if ( r.empty()   ) { continue ; }                             // CONTINUE 
      const LHCb::MCParticle* mcp = r.front().to() ;
      if ( 0 == mcp    ) { continue ; }                             // CONTINUE
      // get the primary vertex!!!
      const LHCb::MCVertex* mcpv = mcp -> primaryVertex() ;
      if ( 0 == mcpv   ) { continue ; }                              // CONTINUE 
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
      m_table1.i_relate ( pv , mc , weight ).ignore () ;
      
    } // end of loop over Monte Carlo primary vertices    
  }  // end of loop over reconstructed primary vertices
  //
  // DECORATION: make a statistics
  if ( statPrint() || msgLevel ( MSG::DEBUG ) ) 
  { counter ( "#PV->MCVertex" ) += m_table1.relations().size() ; }
  //
  // ATTENTION!!
  m_valid1 = true ;                                              // ATTENTION!!
  // 
  return StatusCode::SUCCESS ;
}
// ============================================================================
// build the second table 
// ============================================================================
StatusCode LoKi::PV2MC::build2 () const  
{
  // invalidate the second table:
  m_valid2 = false    ;
  m_table2 . clear () . ignore () ;
  // check the first table:
  if ( !valid1() ) 
  {
    StatusCode sc = build1 () ;
    if ( sc.isFailure() ) { return Error ( "build2: error from build1" , sc ); }
  }
  //
  if ( !valid1() ) 
  { return Error ( "build2: the first table is invalid!" ) ; }
  // locate the tool (if needed) 
  if ( !m_mc2col ) 
  { m_mc2col = tool<IMC2Collision>  ( m_mc2colName , this ) ; }
  //
  // get the relation table form the tool 
  typedef IMC2Collision::MCVertex2Collision MC2C ;
  //
  const MC2C* mc2col = m_mc2col -> vertex2collision () ;
  //
  if ( 0 == mc2col ) 
  { return Error("IMC2Collision::MCVertex2Collision* points to NULL!") ; }
  //
  // make the loop over the first table 
  Table1::Range r1 = m_table1.relations() ;
  for ( Table1::iterator it = r1.begin() ; r1.end() != it ; ++it ) 
  {
    const LHCb::RecVertex* rv = it -> from () ;
    if ( 0 == rv ) { continue ; }                                  // CONTINUE 
    const LHCb::MCVertex*  mc = it -> to   () ;
    if ( 0 == mc ) { continue ; }                                  // CONTINUE 
    // get the collision for MC-vertex 
    MC2C::Range r = mc2col->relations ( mc ) ;
    if ( r.empty () ) 
    { Warning ("Empty MCVertex->GenCollision range") ; continue ; } // CONTINUE 
    if ( 1 != r.size () ) { Warning ( " No 1<-->1 link ") ; }
    const LHCb::GenCollision* gc = r.front().to() ;
    if ( 0 == gc ) { continue ; }                                   // CONTINUE
    // fill the relation 
    m_table2.i_relate ( rv , gc , it->weight () ).ignore()  ;
  } 
  //
  if ( m_table1.relations().size() != m_table2.relations().size() ) 
  { Warning ( "mismatchion table1/table2 size!" ) ; }    
  //
  // make a statistics
  if ( statPrint() || msgLevel ( MSG::DEBUG ) ) 
  { counter ( "#PV->GenCollision" ) += m_table2.relations().size() ; }
  //
  // ATTENTION!!
  m_valid2 = true ;                                              // ATTENTION!!
  // 
  return StatusCode::SUCCESS ;
}
// ============================================================================
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi,PV2MC) 
// ============================================================================
// The END
// ============================================================================

