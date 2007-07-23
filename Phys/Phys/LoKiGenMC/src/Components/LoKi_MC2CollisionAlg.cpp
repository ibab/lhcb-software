// $Id: LoKi_MC2CollisionAlg.cpp,v 1.7 2007-07-23 17:29:54 ibelyaev Exp $
// ============================================================================
// Include file
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SystemOfUnits.h"
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/MCVertex.h"
#include "Event/GenCollision.h"
#include "Event/HepMCEvent.h"
// ============================================================================
// LoKiCore
// ============================================================================
#include "LoKi/ILoKiSvc.h"
#include "LoKi/Math.h"
#include "LoKi/Print.h"
#include "LoKi/Geometry.h"
// ============================================================================
// LoKiMC
// ============================================================================
#include "LoKi/MCTypes.h"
#include "LoKi/MCVertexCuts.h"
// ============================================================================
// Relations
// ============================================================================
#include "Relations/Relation1D.h"
// ============================================================================
// Kernel(???)
// ============================================================================
#include "Kernel/MC2Collision.h"
// ============================================================================
/** @file
 *
 * Implementation file for class : LoKi_MC2CollisionAlg
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
/** @class LoKi_MC2CollisionAlg
 *
 *  helper algorthm to build MCVertex  <---> LHCb::GenCollision relations 
 *  
 *  It associates the primary MCVertex "v" to "nearest" GenCollision "c"
 *
 *  @see LHCb::MCVertex
 *  @see LHCb::GenCollision
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@lapp.in2p3.fr
 *  @date 2005-07-13
 */
// ============================================================================
class LoKi_MC2CollisionAlg : public GaudiAlgorithm 
{
  friend class AlgFactory<LoKi_MC2CollisionAlg> ;
public:
  // ==========================================================================
  /// initialization of the algorithm
  virtual StatusCode intialize () 
  {
    StatusCode sc = GaudiAlgorithm::initialize() ;
    if ( sc.isFailure() ) { return sc ; }
    // load LoKi service 
    svc<LoKi::ILoKiSvc>( "LoKiSvc" ) ;
    return StatusCode::SUCCESS ;
  } ;
  // ==========================================================================
  /// execution of the algorithm
  virtual StatusCode execute() ;
  // ==========================================================================
protected:
  // ==========================================================================
  /**standard constructor 
   * @param name algorithm instance name 
   * @param pSvc pointer to Service Locator 
   */
  LoKi_MC2CollisionAlg 
  ( const std::string& name , 
    ISvcLocator*       pSvc )
    : GaudiAlgorithm ( name , pSvc ) 
    , m_vertices     ( LHCb::MCVertexLocation     ::Default ) 
    , m_collisions   ( LHCb::GenCollisionLocation ::Default ) 
    , m_output       ( LHCb::MC2CollisionLocation ::Default )
    , m_threshold    ( 0.1 * CLHEP::micrometer ) 
  {
    declareProperty ( "MCVertices"    , m_vertices   ) ;
    declareProperty ( "GenCollisions" , m_collisions ) ;
    declareProperty ( "OutputTable"   , m_output     ) ;
    
    setProperty     ( "StatPrint"     , "true"       )  ;
  } ;
  // ==========================================================================
  // destructor: protected and virtual 
  virtual ~LoKi_MC2CollisionAlg(){} ;
  // ==========================================================================
private:
  // default constructor is disabled 
  LoKi_MC2CollisionAlg () ;
  // copy constructor is disabled 
  LoKi_MC2CollisionAlg           ( const LoKi_MC2CollisionAlg& ) ;
  // assignement is disabled 
  LoKi_MC2CollisionAlg& operator=( const LoKi_MC2CollisionAlg& ) ;
private:
  // location of MC-vertices
  std::string m_vertices   ; ///< location of MC-vertices
  // location of collisions 
  std::string m_collisions ; ///< location of collisions 
  // location of output relation table 
  std::string m_output     ; ///< location of output relation table 
  // threshold  
  double      m_threshold  ; ///< threshold
};
// ============================================================================
DECLARE_ALGORITHM_FACTORY(LoKi_MC2CollisionAlg)
// ============================================================================
namespace 
{
  // the actual type of relation table 
  typedef LHCb::Relation1D<LHCb::MCVertex,LHCb::GenCollision> Table  ;
} 
// ============================================================================
StatusCode LoKi_MC2CollisionAlg::execute() 
{
  using namespace LoKi::Cuts  ;
  using namespace LoKi::Types ;
  
  // the internal storage of primary vertices 
  typedef std::vector<const LHCb::MCVertex*> PRIMARIES ;
  
  // get vertices form the TES
  const LHCb::MCVertices*    vertices   = get<LHCb::MCVertices>   ( m_vertices  ) ;
  if ( 0 == vertices   ) { return StatusCode::FAILURE ; }
  
  // get collisions from the TES
  const LHCb::GenCollisions* collisions = get<LHCb::GenCollisions>( m_collisions ) ;
  if ( 0 == collisions ) { return StatusCode::FAILURE ; }
  
  // register the ouput table in TES 
  Table* table = new Table( collisions->size() ) ;
  put ( table , m_output ) ;
  
  // select all primary vertices 
  PRIMARIES prims ;
  LoKi::select ( vertices->begin()            , 
                 vertices->end  ()            , 
                 std::back_inserter ( prims ) ,
                 MCPRIMARY                    ) ;

  if ( prims.empty() ) 
  { Warning ( "Empty container of primary vertices!" ) ; }
  
  // try to make relations
  
  // loop over all collisions 
  for ( LHCb::GenCollisions::const_iterator ic = collisions->begin() ; 
        collisions->end() != ic ; ++ic ) 
  {
    const LHCb::GenCollision* c = *ic ;
    if ( 0 ==   c )  { continue ; }                               // CONTINUE
    
    const LHCb::HepMCEvent* e = c->event() ;
    if ( 0 ==   e )  
    { Warning ( "LHCb::GenCollision::event() points to NULL!" ) ; continue ; }
    const HepMC::GenEvent* evt = e->pGenEvt() ;
    if ( 0 == evt ) 
    { Warning ( "LHCb::HepMCEvent::pGenEvt() points to NULL!"  ) ; continue ; }
    const HepMC::GenVertex* vx = evt->barcode_to_vertex(-1) ;
    if ( 0 == vx  ) 
    { 
      Warning ( "HepMC::GenEvent::barcode_to_vertex(-1) points to NULL!"  ) ; 
      continue ;                                                   // CONTINUE 
    }
    // find the primary vertex with minimal distance 
    MCVFun fun = MCVDIST( LoKi::Point3D( vx->point3d() ) ) ;
    //
    PRIMARIES::const_iterator imin = 
      LoKi::select_min ( prims.begin() , prims.end  () , fun ) ;
    if ( prims.end() == imin ) 
    { Warning ( "The minimum value is not found! (empty?)"); continue ; }
    if ( fun( *imin ) > m_threshold )
    {
      Warning ( "The minmum value is above the threshold "
                + Gaudi::Utils::toString( m_threshold / Gaudi::Units::micrometer ) 
                + "[um]") ;
      continue ;                                                   // CONTINUE 
    }
    // fill the table!!!
    table->i_relate( *imin , c ) ;
  } ;
  
  // Final check:
  if ( table->relations().empty() ) { Warning ( "Empty relation table!") ; }
  
  // make a statistics
  if ( statPrint() || msgLevel( MSG::DEBUG ) )
  { counter ( "#links" ) += table->relations().size() ; }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// The END
// ============================================================================

