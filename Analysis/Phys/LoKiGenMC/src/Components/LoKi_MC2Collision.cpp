// $Id$
// ============================================================================
// Include files
// ============================================================================
#include <algorithm>
#include <functional>
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
#include "Relations/Relation.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/MCVertex.h"
#include "Event/GenCollision.h"
#include "Event/HepMCEvent.h"
// ============================================================================
// DaVinciMCKernel
// ============================================================================
#include "Kernel/IMC2Collision.h"
#include "Kernel/MC2Collision.h"
// ============================================================================
// LoKiCore
// ============================================================================
#include "LoKi/ILoKiSvc.h"
#include "LoKi/Math.h"
#include "LoKi/MCTypes.h"
#include "LoKi/KinTypes.h"
#include "LoKi/MCVertexCuts.h"
#include "LoKi/Inherits.h"
// ============================================================================
// Boost
// ============================================================================
#include "boost/static_assert.hpp"
// ============================================================================
/** @file
 *
 *  Implementation file for class : LoKi::MC2GenCollision
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
 *  @date 2006-03-18
 */
// ============================================================================
namespace LoKi
{
  /** @class MC2GenCollision
   *  The most simplest implementation of IMC2Collision interface
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2001-01-23
   */
  // ==========================================================================
  class MC2GenCollision
    : public virtual IMC2Collision
    , public virtual IIncidentListener
    , public         GaudiTool
  {
    // ========================================================================
    /// friend factory for instantiation
    friend class ToolFactory<LoKi::MC2GenCollision> ;
    // ========================================================================
  public:
    // ========================================================================
    /** get the relation table for LHCb::MCVertex -> LHCb::GenCollision
     *
     *  @code
     *
     *  // get tht tool
     *  const IMC2Collision* t = tool<IMC2Collision>( .... ) ;
     *
     *  // get the reation table form the tool
     *  const LHCb::MCVertex2Collision* table = t->vertex2collision() ;
     *
     *  @endcode
     *
     *  @see MC2Collision
     *  @see LHCb::MCVertex2Collision
     *  @see LHCb::MCVertex
     *  @see LHCb::GenCollision
     *  @see IMC2Collision
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-03-18
     */
    virtual const MCVertex2Collision* vertex2collision() const
    {
      // valid table?
      if  ( valid() ) { return &m_table ; }
      // build  and validate the table
      StatusCode sc = build () ;
      // the final check
      Assert ( sc.isSuccess () , "Unable to build the table" , sc ) ;
      Assert ( valid()         , "The table is invalid!"          ) ;
      //
      return &m_table ;
    }
    // ========================================================================
  public:
    // ========================================================================
    /// standard initialization of the tool
    virtual StatusCode initialize ()
    {
      StatusCode sc = GaudiTool::initialize () ;
      if ( sc.isFailure() ) { return sc ; }
      //
      IIncidentSvc* isvc = incSvc() ;
      Assert ( 0 != isvc , "IIncidentSvc* points to NULL" );
      isvc -> addListener ( this , IncidentType::BeginEvent , 10 ) ;
      // load LoKi service
      svc<LoKi::ILoKiSvc>( "LoKiSvc" ) ;
      //
      m_valid = false ;
      m_table.clear() ;
      //
      return StatusCode::SUCCESS ;
    }
    // ========================================================================
    /// handle new incident (invalidate the table)
    virtual void handle ( const Incident& )
    {
      m_valid = false  ;
      m_table.clear () ;
    }
    // ========================================================================
    /// valid table ?
    bool valid() const { return m_valid ; }
    // ========================================================================
  protected:
    // ========================================================================
    /** standart constructor
     *  @param type tool type
     *  @param name tool name
     *  @param parent parent object
     */
    MC2GenCollision
    ( const std::string& type   ,
      const std::string& name   ,
      const IInterface*  parent )
      : GaudiTool ( type, name , parent )
      ///
      , m_vertices     ( LHCb::MCVertexLocation     ::Default )
      , m_collisions   ( LHCb::GenCollisionLocation ::Default )
      , m_threshold    ( 0.1 * Gaudi::Units::micrometer )
      ///
      , m_table ()
      , m_valid ()
    {
      declareInterface<IIncidentListener> ( this ) ;
      declareInterface<IMC2Collision>     ( this ) ;
      //
      declareProperty ( "MCVertices"    , m_vertices   ) ;
      declareProperty ( "GenCollisions" , m_collisions ) ;
      declareProperty ( "Threshold"     , m_threshold  ) ;
      //
    }
    // ========================================================================
    /// destructor (virual and protected)
    virtual ~MC2GenCollision () {} ;
    // ========================================================================
  private:
    // ========================================================================
    /// default constructor  is disabled
    MC2GenCollision ()  ;                   // default constructor  is disabled
    /// copy constructor     is disabled
    MC2GenCollision ( const MC2GenCollision& ) ; // copy constructor is disabled
    /// assignement operator is disabled
    MC2GenCollision& operator= ( const MC2GenCollision& ) ;   // no assignement
    // ========================================================================
  protected:
    // ========================================================================
    /// build the table
    StatusCode build() const ;
    // ========================================================================
  private:
    // ========================================================================
    /// location of MC-vertices
    std::string    m_vertices ;                      // location of MC-vertices
    /// location of MC-vertices
    std::string    m_collisions ;                  // location of GenCollisions
    /// the threshold
    double         m_threshold ;                               // the threshold
    // ========================================================================
  private:
    // ========================================================================
    /// the actual interface class (not exported)
    typedef Relations::Relation<LHCb::MCVertex,LHCb::GenCollision> Table ;
    /// static check
    BOOST_STATIC_ASSERT(INHERITS(Table,LHCb::MCVertex2Collision));
    /// the relationtabel itself
    mutable Table m_table ;                          // the relationtabel itself
    /// validity flag
    mutable bool  m_valid ;                                    // validity flag
    // ========================================================================
  } ;
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
// build the table
// ============================================================================
StatusCode LoKi::MC2GenCollision::build() const
{
  // avoid long names :
  using namespace LoKi::Cuts  ;
  using namespace LoKi::Types ;

  // the internal storage of primary vertices
  typedef std::vector<const LHCb::MCVertex*> PRIMARIES ;

  // 0) invalidate the table
  m_table.clear () ;
  m_valid = false  ;


  // 1) get vertices from the TES
  const LHCb::MCVertices*    vertices   = get<LHCb::MCVertices>   ( m_vertices  ) ;
  if ( 0 == vertices   ) { return StatusCode::FAILURE ; }

  // 2) get collisions from the TES
  const LHCb::GenCollisions* collisions = get<LHCb::GenCollisions>( m_collisions ) ;
  if ( 0 == collisions ) { return StatusCode::FAILURE ; }

  // select all primary vertices
  PRIMARIES prims ;
  std::copy_if ( vertices->begin()            ,
                 vertices->end  ()            ,
                 std::back_inserter ( prims ) ,
                 std::cref(MCPRIMARY)         ) ;

  if ( prims.empty() )
  { Warning ( "build: Empty container of primary MC-vertices!" ) ; }

  /// loop over all collisions
  for ( LHCb::GenCollisions::const_iterator ic = collisions->begin() ;
        collisions->end() != ic ; ++ic )
  {
    const LHCb::GenCollision* c = *ic ;
    if ( !c )  { continue ; }                               // CONTINUE

    const LHCb::HepMCEvent* e = c->event() ;
    if ( !e )
    { Warning ( "build: LHCb::GenCollision::event() points to NULL!" ) ; continue ; }
    const HepMC::GenEvent* evt = e->pGenEvt() ;
    if ( !evt )
    { Warning ( "build: LHCb::HepMCEvent::pGenEvt() points to NULL!"  ) ; continue ; }
    const HepMC::GenVertex* vx = evt->barcode_to_vertex(-1) ;
    if ( !vx  )
    {
      Warning ( "build::HepMC::GenEvent::barcode_to_vertex(-1) points to NULL!"  ) ;
      continue ;                                                   // CONTINUE
    }
    // find the primary vertex with minimal distance
    MCVFun fun = MCVDIST( LoKi::Point3D( vx->point3d() ) ) ;

    //
    auto imin = LoKi::select_min ( prims.begin() , prims.end  () , fun ) ;
    if ( prims.end() == imin )
    { Warning ( "The minimum value is not found! (empty?)"); continue ; }
    if ( fun( *imin ) > m_threshold )
    {
      Warning ( "build::The minimum value is above the threshold "
                + Gaudi::Utils::toString( m_threshold / Gaudi::Units::micrometer )
                + "[um]") ;
      continue ;                                                   // CONTINUE
    }
    // fill the table!!!
    m_table.i_push ( *imin , c ) ; // NB! i_push is in the game !!!
  }

  // sort the table
  m_table.i_sort () ; // MANDATORY

  // Final check:
  if ( m_table.relations().empty() )
  { Warning ( "build:Empty relation table!") ; }

  // make a statistics
  if ( statPrint() || msgLevel( MSG::DEBUG ) )
  { counter ( "#MC->GenCollision" ) += m_table.relations().size() ; }

  // set the proper validity flag!
  m_valid = true ;

  return StatusCode::SUCCESS ;
}
// ============================================================================
/// the factory
// ============================================================================
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi,MC2GenCollision)
// ============================================================================
// The END
// ============================================================================
