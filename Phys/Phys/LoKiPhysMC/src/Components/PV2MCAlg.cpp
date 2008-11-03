// $Id: PV2MCAlg.cpp,v 1.2 2008-11-03 18:35:49 ibelyaev Exp $
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
#include "Kernel/IPV2MC.h"
#include "Kernel/MC2Collision.h"
#include "Kernel/IMC2Collision.h"
#include "Kernel/Track2MC.h"
// ============================================================================
// LoKi & LoKiMC
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
  // ==========================================================================
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
    virtual StatusCode initialize () 
    {
      StatusCode sc = GaudiAlgorithm::initialize() ;
      if ( sc.isFailure() ) { return sc ; }
      // load LoKi service 
      svc<LoKi::ILoKiSvc>( "LoKiSvc" ) ;
      // 
      // get the tool 
      m_pv2mc = tool<IPV2MC> ( m_pv2mcName , this ) ;
      /// private tool?
      if ( this != m_pv2mc->parent() ) 
      { return Error ( "The tool must be private tool!" ) ; }
      /// set the properties of the private tool 
      sc = Gaudi::Utils::setProperty 
        ( m_pv2mc , "Primaries"    , m_primaries ) ;
      if ( sc.isFailure() ) 
      { return Error ( "Unable to (re)set property 'Primaries' ", sc )  ; }
      /// set the properties of the private tool 
      sc = Gaudi::Utils::setProperty 
        ( m_pv2mc , "MCVertices"    , m_vertices ) ;
      if ( sc.isFailure() ) 
      { return Error ( "Unable to (re)set property 'MCVertices' ", sc )  ; }
      /// set the properties of the private tool 
      sc = Gaudi::Utils::setProperty 
        ( m_pv2mc , "Track2MC"       , m_track2MC ) ;
      if ( sc.isFailure() ) 
      { return Error ( "Unable to (re)set property 'Track2MC' "  , sc )  ; }
      /// set the properties of the private tool 
      sc = Gaudi::Utils::setProperty 
        ( m_pv2mc , "MC2Collision"   , m_mc2collision ) ;
      if ( sc.isFailure() ) 
      { return Error ( "Unable to (re)set property 'MC2Collision'", sc )  ; }
      //
      return StatusCode::SUCCESS ;
    } 
    /// execution of the algorithm
    virtual StatusCode execute  () ;
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
      , m_track2MC     ( LHCb::Track2MCLocation::       Default ) 
      // 
      , m_mc2collision ( "LoKi::MC2GenCollision/MC2GenCollision" )
      , m_pv2mcName    ( "LoKi::PV2MC/PV2MC" ) 
      , m_pv2mc        ( 0 )
      //
      , m_output1      ( LHCb::PV2MCLocation::          Default ) 
      , m_output2      ( LHCb::PV2CollisionLocation::   Default )
    {
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
          m_mc2collision ,
          "Type/nemae of IMC2Collision tool" ) ;
      declareProperty 
        ( "PV2MC"      , 
          m_pv2mcName  , 
          "The type/name of IPVMC tool ") ;
      declareProperty 
        ( "OutputPV2MC" , 
          m_output1     , 
          "The TES location of (output) RecVertex->MCVertex table ") ;
      declareProperty 
        ( "OutputPV2Collision" , 
          m_output2   ,
          "The TES location of (output) RecVertex->GenCollision table ") ;
      //
      setProperty     ( "StatPrint"    , true ) ;
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
    /// TES address of primary vertices
    std::string  m_primaries   ; // TES address of primary vertices
    /// TES address of MC  vertices
    std::string  m_vertices    ; // TES address of MC vertices
    /// TES address of MC->Collision links
    std::string  m_collision   ; // TES address of MC->Collision links
    /// TES address of Track->MC links
    std::string  m_track2MC    ; // TES address of Track->MC links
    /// name of MCVertex->GenCollision tool
    std::string m_mc2collision ; // name of MCVeretx->GenCollision tool
    // ========================================================================
  private:
    // ========================================================================    
    /// name of PV->MC tool 
    std::string  m_pv2mcName ;                           // name of PV->MC tool 
    /// the tool itself 
    IPV2MC*      m_pv2mc     ;                               // the tool itself 
    // ========================================================================
  private:
    // ========================================================================    
    /// TES location of output relation table 
    std::string  m_output1   ; // TES location of output relation table 
    /// TES location of output relation table 
    std::string  m_output2   ; // TES location of output relation table 
    // ========================================================================
  };
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
namespace 
{
  // ==========================================================================
  /// the actual type of relation table in TES
  typedef LHCb::RelationWeighted2D
  <LHCb::RecVertex,LHCb::MCVertex,LHCb::PV2MCWeight>     Table1 ;
  /// the actual type of relation table in TES
  typedef LHCb::RelationWeighted2D
  <LHCb::RecVertex,LHCb::GenCollision,LHCb::PV2MCWeight> Table2 ;
  // ==========================================================================
  /// check the types 
  BOOST_STATIC_ASSERT(INHERITS(Table1,LHCb::PV2MC2D)) ;
  BOOST_STATIC_ASSERT(INHERITS(Table2,LHCb::PV2Collision2D)) ;    
  // ========================================================================== 
}
// ============================================================================
// execution of the algorithm
// ============================================================================
StatusCode LoKi::PV2MCAlg::execute() 
{ 
  
  // get the tool 
  Assert ( 0 != m_pv2mc , "IPV2MC* points to NULL!" ) ;
  
  const IPV2MC::PV2MC*        t1 = m_pv2mc-> pv2MC         () ;
  Assert ( 0 != t1 , "IPV2MC::PV2MC* pooint to NULL" ) ;
  Table1* table1 = new Table1 ( *t1 ) ;
  put ( table1 , m_output1 ) ;

  
  const IPV2MC::PV2Collision* t2 = m_pv2mc-> pv2Collision  () ;
  Assert ( 0 != t2 , "IPV2MC::PV2Collision* pooint to NULL" ) ;
  Table2* table2 = new Table2 ( *t2 ) ;
  put ( table2 , m_output2 ) ;
  
  // make a statistics 
  if ( statPrint() || msgLevel ( MSG::DEBUG ) ) 
  { 
    counter ( "#PV->MC" )           += table1->relations().size() ;
    counter ( "#PV->GenCollision" ) += table2->relations().size() ;
  }
  //
  return StatusCode::SUCCESS ;
} 
// ============================================================================
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LoKi,PV2MCAlg) ;
// ============================================================================
// The END
// ============================================================================

