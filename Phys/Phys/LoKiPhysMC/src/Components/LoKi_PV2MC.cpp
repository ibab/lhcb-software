// $Id: LoKi_PV2MC.cpp,v 1.8 2008-06-02 09:59:31 ibelyaev Exp $
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
#include "Relations/IRelationWeighted.h"
#include "Relations/IRelationWeighted2D.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/RecVertex.h"
#include "Event/MCVertex.h"
#include "Event/GenCollision.h"
// ============================================================================
// LoKiCore
// ============================================================================
#include "LoKi/ILoKiSvc.h"
// ============================================================================
// Kernel? 
// ============================================================================
#include "Kernel/PV2MC.h"
#include "Kernel/IPV2MC.h"
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
 *  @date 2006-03-19 
 */
// ============================================================================
/** @class LoKi_PV2MC
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
class LoKi_PV2MC
  : public virtual IPV2MC            ,
    public virtual IIncidentListener ,
    public         GaudiTool  
{
  // friend factory for instantiation
  friend class ToolFactory<LoKi_PV2MC> ;
public:
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
  virtual const PV2MC* pv2MC() const 
  {
    //
    if ( 0 != m_table1 ) { return m_table1->direct() ; }
    m_table1 = get<Table1>( m_location1 ) ;
    if ( 0 == m_table1 ) 
    {
      Error ( "Could not load input data from '" + m_location1 + "'" ) ;
      return 0 ;
    }
    return m_table1->direct() ;
    //
  } 
  /** return a relation table for 
   *  (primary)LHCb::MCVertex --> LHCb::RecVertex relations
   *
   *  @see LHCb::MC2PV
   *  @see LHCb::RecVertex
   *  @see LHCb::MCVertex
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-03-19
   */
  virtual const MC2PV* mc2PV() const 
  {
    //
    if ( 0 != m_table1 ) { return m_table1->inverse() ; }
    m_table1 = get<Table1>( m_location1 ) ;
    if ( 0 == m_table1 ) 
    {
      Error ( "Could not load input data from '" + m_location1 + "'" ) ;
      return 0 ;
    }
    return m_table1->inverse() ;
    //
  }
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
   *      // chi2 of th editance in between MC and RC vertex:
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
  virtual const PV2Collision* pv2Collision() const 
  {
    //
    if ( 0 != m_table2 ) { return m_table2->direct() ; }
    m_table2 = get<Table2>( m_location1 ) ;
    if ( 0 == m_table2 ) 
    {
      Error ( "Could not load input data from '" + m_location2 + "'" ) ;
      return 0 ;
    }
    return m_table2->direct() ;
    //
  } 
  /** return a relation table for 
   *  LHCb::GenCollision --> LHCb::RecVertex relations
   *
   *  @see LHCb::Collision2PV
   *  @see LHCb::RecVertex
   *  @see LHCb::GenCollision
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-03-19
   */
  virtual const Collision2PV* collision2PV() const 
  {
    //
    if ( 0 != m_table2 ) { return m_table2->inverse() ; }
    m_table2 = get<Table2>( m_location2 ) ;
    if ( 0 == m_table2 ) 
    {
      Error ( "Could not load input data from '" + m_location2 + "'" ) ;
      return 0 ;
    }
    return m_table2->inverse() ;
    //
  }
  //
public:  
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
  /// handle new incident 
  virtual void handle ( const Incident& ) 
  { m_table1 = 0 ; m_table2 = 0 ; }
  // ==========================================================================
protected:
  // ==========================================================================
  /** standart constructor 
   *  @param type tool type 
   *  @param name tool name 
   *  @param parent parent object
   */
  LoKi_PV2MC 
  ( const std::string& type   , 
    const std::string& name   , 
    const IInterface*  parent )
    : GaudiTool ( type, name , parent )
    ///
    , m_table1    ( 0 ) 
    , m_table2    ( 0 ) 
    , m_location1 ( LHCb::PV2MCLocation        ::Default ) 
    , m_location2 ( LHCb::PV2CollisionLocation ::Default ) 
    ///
  {
    declareInterface<IIncidentListener> ( this ) ; 
    declareInterface<IPV2MC>            ( this ) ; 
    // 
    declareProperty  ( "LocationPV2PC"         , m_location1  ) ;
    declareProperty  ( "LocationPV2collision"  , m_location2  ) ;
  }
  /// destructor (virual and protected)
  virtual ~LoKi_PV2MC () {} 
  // ==========================================================================
private:
  // ==========================================================================
  // default constructor  is disabled 
  LoKi_PV2MC ()  ;
  // copy constructor     is disabled 
  LoKi_PV2MC 
  ( const LoKi_PV2MC& ) ;
  // assignement operator is disabled 
  LoKi_PV2MC& operator=
  ( const LoKi_PV2MC& ) ;
  // ==========================================================================
private:
  // ==========================================================================
  // the actual interface class (not exported) 
  typedef const LHCb::Relations_::PV2MC2D        Table1;
  typedef const LHCb::Relations_::PV2Collision2D Table2;
  // the relation tables 
  mutable Table1* m_table1    ;
  mutable Table2* m_table2    ;
  // ========================================================================== 
private:
  // ==========================================================================
  // table location 
  std::string    m_location1 ;
  std::string    m_location2 ;
  // ==========================================================================
};
// ============================================================================
DECLARE_TOOL_FACTORY( LoKi_PV2MC ) ;
// ============================================================================
// The END
// ============================================================================

