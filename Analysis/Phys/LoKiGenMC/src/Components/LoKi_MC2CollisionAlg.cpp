// $Id$
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
#include "LoKi/Inherits.h"
// ============================================================================
// Relations
// ============================================================================
#include "Relations/Relation1D.h"
// ============================================================================
// DaVinciMCKernel
// ============================================================================
#include "Kernel/MC2Collision.h"
#include "Kernel/IMC2Collision.h"
// ============================================================================
// Boost 
// ============================================================================
#include "boost/static_assert.hpp"
// ============================================================================
/** @file
 *
 * Implementation file for class : LoKi::MC2GenCollisionAlg
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
  /** @class MC2GenCollisionAlg
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
  class MC2GenCollisionAlg : public GaudiAlgorithm 
  {
    // ==========================================================================
    // friend factory for instantiation 
    friend class AlgFactory<LoKi::MC2GenCollisionAlg> ;
    // ==========================================================================
  public:
    // ==========================================================================
    /// initialization of the algorithm
    virtual StatusCode initialize () 
    {
      StatusCode sc = GaudiAlgorithm::initialize() ;
      if ( sc.isFailure() ) { return sc ; }
      // load LoKi service 
      svc<LoKi::ILoKiSvc>( "LoKiSvc" ) ;
      // get the tool 
      m_mc2col = tool<IMC2Collision> 
        ( "LoKi::MC2GenCollision/MC2GenCollision" , this ) ;
      /// private tool?
      if ( this != m_mc2col->parent() ) 
      { return Error ( "The tool must be private tool!" ) ; }
      // set the properties of the provate tool 
      sc = Gaudi::Utils::setProperty 
        ( m_mc2col , "MCVertices"    , m_vertices   ) ;
      if ( sc.isFailure() ) 
      { return Error ( "Unable to (re)set property 'MCVertices' ", sc )  ; }
      sc = Gaudi::Utils::setProperty 
        ( m_mc2col , "GenCollisions" , m_collisions ) ;
      if ( sc.isFailure() ) 
      { return Error ( "Unable to (re)set property 'GenCollisions' ", sc )  ; }
      sc = Gaudi::Utils::setProperty 
        ( m_mc2col , "Threshold"     , m_threshold, ""  ) ;
      if ( sc.isFailure() ) 
      { return Error ( "Unable to (re)set property 'Threshold' ", sc )  ; }
      //
      return StatusCode::SUCCESS ;
    } 
    /// execution of the algorithm
    virtual StatusCode execute  () ;
    /// finaliza the algorithm
    virtual StatusCode finalize () 
    {
      m_mc2col = 0 ;
      return GaudiAlgorithm::finalize () ;
    }
    // ========================================================================
  protected:
    // ========================================================================
    /** standard constructor 
     *  @param name algorithm instance name 
     *  @param pSvc pointer to Service Locator 
     */
    MC2GenCollisionAlg 
    ( const std::string& name , 
      ISvcLocator*       pSvc )
      : GaudiAlgorithm ( name , pSvc ) 
      , m_vertices     ( LHCb::MCVertexLocation     ::Default ) 
      , m_collisions   ( LHCb::GenCollisionLocation ::Default ) 
      , m_threshold    ( 0.1 * Gaudi::Units::micrometer ) 
      , m_output       ( LHCb::MC2CollisionLocation ::Default )
      //
      , m_mc2colName ( "LoKi::MC3GenCollision/MC2GenCollision" )
      , m_mc2col ( 0 )
    {
      //
      declareProperty 
        ( "MCVertices"    , 
          m_vertices      , 
          "The TES location of Monte Carlo Vertices  (LHCb::MCVertex)"    ) ;
      declareProperty 
        ( "GenCollisions" , 
          m_collisions    , 
          "The TES location of Generator Collisions (LHCb::GenCollision)") ;
      declareProperty 
        ( "Threshold"     , 
          m_threshold     ,
          "The threshold for LHCb::MCVertex ---> LHCb::GenCollision links") ;
      declareProperty 
        ( "MC2Collision"  , 
          m_mc2colName    , 
          "The type/name of IMCCollision tool ") ;
      declareProperty 
        ( "OutputTable"   , 
          m_output        , 
          "The TES location of MCVeretx->GenCollision relation table") ;
      //
     } 
    // ========================================================================
    // destructor: protected and virtual 
    virtual ~MC2GenCollisionAlg(){} ;
    // ========================================================================
  private:
    // ========================================================================
    // default constructor is disabled 
    MC2GenCollisionAlg () ;
    // copy constructor is disabled 
    MC2GenCollisionAlg           ( const MC2GenCollisionAlg& ) ;
    // assignement is disabled 
    MC2GenCollisionAlg& operator=( const MC2GenCollisionAlg& ) ;
    // ========================================================================
  private:
    // ========================================================================
    /// location of MC-vertices
    std::string m_vertices   ;                       // location of MC-vertices
    /// location of collisions 
    std::string m_collisions ;                        // location of collisions 
    /// threshold  
    double      m_threshold  ;                                     // threshold
    /// location of output relation table 
    std::string m_output     ;             // location of output relation table 
    /// the tool 
    std::string m_mc2colName ;                                      // the tool 
    /// the tool 
    IMC2Collision* m_mc2col  ;                                      // the tool 
    // ========================================================================
  };
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
namespace 
{
  // the actual type of relation table 
  typedef LHCb::Relation1D<LHCb::MCVertex,LHCb::GenCollision> Table  ;
  // make the useful check 
  BOOST_STATIC_ASSERT(INHERITS(Table,LHCb::MCVertex2Collision)); 
} 
// ============================================================================
StatusCode LoKi::MC2GenCollisionAlg::execute() 
{
  // check the tool 
  Assert ( 0 != m_mc2col , "IMC2Collision* points to NULL!" ) ;
  
  // get the links from the tool  
  const IMC2Collision::MCVertex2Collision* links = 
    m_mc2col -> vertex2collision() ;
  
  if ( 0 == links ) 
  { return Error ( "MCVertex2Collision* points to NULL!" ) ; }
  
  // register the ouput table in TES 
  Table* table = new Table( *links ) ;
  put ( table , m_output ) ;
  
  // Final check:
  if ( table->relations().empty() ) { Warning ( "Empty relation table!") ; }
  
  // make a statistics
  if ( statPrint() || msgLevel( MSG::DEBUG ) )
  { counter ( "#links" ) += table->relations().size() ; }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
/// the factory 
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LoKi,MC2GenCollisionAlg)
// ============================================================================
// The END
// ============================================================================

