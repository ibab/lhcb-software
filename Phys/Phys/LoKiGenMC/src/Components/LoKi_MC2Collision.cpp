
// $Id: LoKi_MC2Collision.cpp,v 1.5 2008-06-02 09:54:46 ibelyaev Exp $
// ============================================================================
// Include files 
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
// ============================================================================
// Event 
// ============================================================================
#include "Event/MCVertex.h"
#include "Event/GenCollision.h"
// ============================================================================
// LoKiCore
// ============================================================================
#include "LoKi/ILoKiSvc.h"
// ============================================================================
// Kernel ?  
// ============================================================================
#include "Kernel/IMC2Collision.h"
#include "Kernel/MC2Collision.h"
// ============================================================================
/** @file
 *
 *  Implementation file for class : LoKi_MC2Collision
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
/** @class LoKi_MC2Collision
 *  The most simplest implementation of IMC2Collision interface
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23 
 */
// ============================================================================
class LoKi_MC2Collision 
  : public virtual IMC2Collision     
  , public virtual IIncidentListener 
  , public         GaudiTool  
{
  // friend factory for instantiation
  friend class ToolFactory<LoKi_MC2Collision> ;
  //
public:
  // ==========================================================================
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
    //
    if ( 0 != m_table ) { return m_table ; }
    m_table = get<Table>( m_location ) ;
    if ( 0 == m_table ) 
    {
      Error ( "Could not load input data from '" + m_location + "'" ) ;
    }
    return m_table ;
  } ;
  // ==========================================================================
public:
  // ==========================================================================
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
  };
  // ==========================================================================
  /// handle new incident 
  virtual void handle ( const Incident& ) { m_table = 0 ; }
  // ==========================================================================
protected:
  // ==========================================================================  
  /** standart constructor 
   *  @param type tool type 
   *  @param name tool name 
   *  @param parent parent object
   */
  LoKi_MC2Collision 
  ( const std::string& type   , 
    const std::string& name   , 
    const IInterface*  parent )
    : GaudiTool ( type, name , parent )
    ///
    , m_table    ( 0                                   ) 
    , m_location ( LHCb::MC2CollisionLocation::Default ) 
    ///
  {
    declareInterface<IIncidentListener> ( this ) ; 
    declareInterface<IMC2Collision>     ( this ) ; 
    // 
    declareProperty  ( "Location"    , m_location  ) ;
  };           
  // ==========================================================================  
  /// destructor (virual and protected)
  virtual ~LoKi_MC2Collision () {} ;
  // ==========================================================================  
private:
  // default constructor  is disabled 
  LoKi_MC2Collision ()  ;
  // copy constructor     is disabled 
  LoKi_MC2Collision 
  ( const LoKi_MC2Collision& ) ;
  // assignement operator is disabled 
  LoKi_MC2Collision& operator=
  ( const LoKi_MC2Collision& ) ;
  //
private:
  // the actual interface class (not exported) 
  typedef const LHCb::MCVertex2Collision    Table ;
  mutable Table*                          m_table ;
private:
  // table location 
  std::string    m_location  ;
} ;
// ============================================================================
DECLARE_TOOL_FACTORY( LoKi_MC2Collision ) 
// ============================================================================
// The END
// ============================================================================

