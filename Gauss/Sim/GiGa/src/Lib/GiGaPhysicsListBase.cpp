// $Id: GiGaPhysicsListBase.cpp,v 1.1 2003-04-06 18:49:48 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// GiGa
#include "GiGa/GiGaPhysicsListBase.h"
#include "GiGa/GiGaUtil.h"
// ============================================================================

// ============================================================================
/** @file 
 * 
 *  Implementation file for class : GiGaPhysicsListBase
 * 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2003-04-06
 */
// ============================================================================


// ============================================================================
namespace GiGaPhysicsListBaseLocal
{
#ifdef GIGA_DEBUG
  /** @var   s_Counter
   *  static instance counter 
   */
  static GiGaUtil::InstanceCounter<GiGaPhysicsListBase> s_Counter ;
#endif   
};
// ============================================================================


// ============================================================================
/** standard constructor
 *  @see GiGaBase 
 *  @see AlgTool 
 *  @param type type of the object (?)
 *  @param name name of the object
 *  @param parent  pointer to parent object
 */
// ============================================================================
GiGaPhysicsListBase::GiGaPhysicsListBase
( const std::string& type   , 
  const std::string& name   , 
  const IInterface*  parent ) 
  : GiGaBase ( type , name , parent )
  , m_defaultCut        (  2 * mm ) 
  , m_cutForGamma       ( -1 * km ) 
  , m_cutForElectron    ( -1 * km ) 
  , m_cutForPositron    ( -1 * km ) 
  , m_cutForProton      ( -1 * km ) 
  , m_cutForAntiProton  ( -1 * km ) 
  , m_cutForNeutron     ( -1 * km ) 
  , m_cutForAntiNeutron ( -1 * km ) 
{
  // 
  declareInterface<IGiGaPhysicsList> ( this ) ;
  //
  declareProperty( "Cut"               , m_defaultCut        ) ;
  declareProperty( "CutForGamma"       , m_cutForGamma       ) ;
  declareProperty( "CutForElectron"    , m_cutForElectron    ) ;
  declareProperty( "CutForPositron"    , m_cutForPositron    ) ;
  declareProperty( "CutForProton"      , m_cutForProton      ) ;
  declareProperty( "CutForAntiProton"  , m_cutForAntiProton  ) ;
  declareProperty( "CutForNeutron"     , m_cutForNeutron     ) ;
  declareProperty( "CutForAntiENutron" , m_cutForAntiNeutron ) ;
  //
#ifdef GIGA_DEBUG
  GiGaPhysicsListBaseLocal::s_Counter.increment () ;
#endif
  //
};
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
GiGaPhysicsListBase::~GiGaPhysicsListBase()
{
  //
#ifdef GIGA_DEBUG
  GiGaPhysicsListBaseLocal::s_Counter.decrement () ;
#endif
  //
};
// ============================================================================

// ============================================================================
/** initialize the object 
 *  @see GiGaBase
 *  @see  AlgTool
 *  @see IAlgTool
 *  @return status code 
 */
// ============================================================================
StatusCode   GiGaPhysicsListBase::initialize     () 
{
  // initialize the base
  StatusCode sc = GiGaBase::initialize () ;
  if( sc.isFailure() ) { return Error ( "GiGaBase is not initialized" , sc ) ; }
  
  // readjust the cuts 
  if( cutForGamma       () <= 0 ) { m_cutForGamma       = defaultCut () ; }
  if( cutForElectron    () <= 0 ) { m_cutForElectron    = defaultCut () ; }
  if( cutForPositron    () <= 0 ) { m_cutForPositron    = defaultCut () ; }
  if( cutForProton      () <= 0 ) { m_cutForProton      = defaultCut () ; }
  if( cutForAntiProton  () <= 0 ) { m_cutForAntiProton  = defaultCut () ; }
  if( cutForNeutron     () <= 0 ) { m_cutForNeutron     = defaultCut () ; }
  if( cutForAntiNeutron () <= 0 ) { m_cutForAntiNeutron = defaultCut () ; }
  //
  MsgStream log( msgSvc () , name () ) ;
  log << MSG::DEBUG 
      << " Production cuts *COULD* be set separately for following particles: " 
      << endreq ;
  log << MSG::DEBUG 
      << " \tGamma      : \t" << cutForGamma       () / mm << " mm" << endreq 
      << " \tElectron   : \t" << cutForElectron    () / mm << " mm" << endreq 
      << " \tPositron   : \t" << cutForPositron    () / mm << " mm" << endreq 
      << " \tProton     : \t" << cutForProton      () / mm << " mm" << endreq 
      << " \tAntiProton : \t" << cutForAntiProton  () / mm << " mm" << endreq 
      << " \tNeutron    : \t" << cutForNeutron     () / mm << " mm" << endreq 
      << " \tAntiNeutron: \t" << cutForAntiNeutron () / mm << " mm" << endreq
      << " \tDefault    : \t" << defaultCut        () / mm << " mm" << endreq ;
  //
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
/** finalize the object 
 *  @see GiGaBase
 *  @see  AlgTool
 *  @see IAlgTool
 *  @return status code 
 */
// ============================================================================
StatusCode   GiGaPhysicsListBase::finalize () { return GiGaBase::finalize () ; }
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
