// $Id: LoKiSvc.cpp,v 1.6 2006-11-09 17:01:06 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.6 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.5  2006/06/24 17:18:41  ibelyaev
//  reduce the printout
//
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Service.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ServiceLocatorHelper.h"
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/ILoKiSvc.h"
#include "LoKi/IReporter.h"
#include "LoKi/ErrorReport.h"
#include "LoKi/Welcome.h"
#include "LoKi/Exception.h"
#include "LoKi/Services.h"
// ============================================================================

// ============================================================================
/** @file
 *
 *  Implementation file for class LoKiSvc 
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
 *  @date 2001-01-23 
 */
// ============================================================================

// ============================================================================
/** @class LoKiSvc
 *  the simplest implementation of LoKi::ILoKiSvc interface 
 *  @see LoKi::ILoKiSvc 
 *  @date 2006-01-16 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 */
// ============================================================================
class LoKiSvc : 
  public virtual LoKi::ILoKiSvc , 
  public                Service 
{
  friend class SvcFactory<LoKiSvc> ;
public:
  
  /** get the pointer to service locator 
   *  @return ponter to Service Locator 
   *  @see LoKi::ILoKiSvc
   *  @see IScvLocator 
   */
  virtual ISvcLocator*          svcLoc() const 
  { return Service::serviceLocator() ; } ;
  
  /** get the pointer to Particle Property Service 
   *  @return pointer to Particle Property Service 
   *  @see LoKi::ILoKiSvc
   *  @see IParticlePropertySvc 
   */
  virtual IParticlePropertySvc* ppSvc() const 
  {
    if ( 0 != m_ppSvc ) { return m_ppSvc ; }
    // locate the service 
    StatusCode sc = service ( "ParticlePropertySvc" , m_ppSvc , true ) ;
    if       ( sc.isFailure() ) 
    { LOKI_EXCEPTION ( "LoKiSvc: 'PPSvc' could not be located" , sc ) ; }
    else if  ( 0 == m_ppSvc ) 
    { LOKI_EXCEPTION ( "LoKiSvc: IPPSvc* points to NULL"       , sc ) ; }
    //
    return m_ppSvc ;
  } ;
  /** get the pointer to Tool Service 
   *  @return pointer to Tool Service 
   *  @see LoKi::ILoKiSvc
   *  @see IToolSvc 
   */
  virtual IToolSvc*             toolSvc () const 
  {
    if ( 0 != m_toolSvc ) { return m_toolSvc ; }
    // locate the service 
    StatusCode sc = service ( "ToolSvc" , m_toolSvc , true ) ;
    if ( sc.isFailure() ) 
    { 
      m_toolSvc = 0 ;
      LOKI_EXCEPTION( "LoKiSvc: 'ToolSvc' could nto be located" , sc ) ; 
    }
    if ( 0 == m_toolSvc ) 
    { 
      LOKI_EXCEPTION( "LoKiSvc: IToolSvc* points to NULL"       , sc ) ; 
    }
    //
    return m_toolSvc ;
  } ;  
  /** get "good" error reporter
   *  @return pointer to Good error reporter
   *  @see LoKi::IReporter
   */
  virtual LoKi::IReporter*  reporter() const 
  {
    if ( 0 != m_reporter ) { return m_reporter ; }    // RETURN
    
    IToolSvc* svc = toolSvc()   ;
    if ( 0 == svc        ) { return 0 ; }             // RETURN 
    StatusCode sc = svc -> retrieveTool ( m_reporterName , 
                                          m_reporter     , 
                                          this           ) ;
    if ( sc.isFailure()  ) { return 0 ; }             // RETURN 
    if ( 0 == m_reporter ) { return 0 ; }             // RETURN  
    //
    return m_reporter ;
  } ;
public:
  /// Inform that a new incident has occured
  virtual void handle ( const Incident& ) {} ;
public:
  /** general service initialization
   *  @see IService 
   *  @return status code
   */
  virtual StatusCode initialize() 
  {
    StatusCode sc = Service::initialize () ;
    if ( sc.isFailure() ) { return sc ; }
    //
    { // welcome message 
      MsgStream log ( msgSvc() , name() ) ;
      log << MSG::ALWAYS << std::endl ;
      LoKi::Welcome::instance ().welcome( log.stream() ) ;
      log << endreq ;
    }
    //
    LoKi::Services& svc = LoKi::Services::instance() ;
    if ( 0 == svc.lokiSvc() ) { svc.setLoKi( this ) ; }
    //
    {
      IToolSvc* svc = toolSvc()   ;
      if ( 0 == svc        ) { return StatusCode::FAILURE ; }
      sc = svc -> retrieveTool ( m_reporterName , 
                                 m_reporter     , this ) ;
      if ( sc.isFailure()  ) { return StatusCode::FAILURE ; }
      if ( 0 == m_reporter ) { return StatusCode::FAILURE ; }
    }
    //
    LoKi::ErrorReport& rep = LoKi::ErrorReport::instance() ;
    if ( 0 == rep.reporter() && 0 != m_reporter ) 
    { rep.setReporter ( m_reporter ) ; }
    //
    return StatusCode::SUCCESS ;
  } ;  
  /** general service finalizetion 
   *  @see IService 
   *  @return status code
   */
  virtual StatusCode finalize () 
  {
    //
    LoKi::Services& svc = LoKi::Services::instance() ;
    svc.releaseAll() ;
    //
    if ( 0 != m_reporter && 0 != m_toolSvc ) 
    { m_toolSvc -> releaseTool ( m_reporter ) ; }
    m_reporter = 0 ;
    if ( 0 != m_toolSvc ) { m_toolSvc -> release() ; m_toolSvc = 0 ; }    
    if ( 0 != m_ppSvc   ) { m_ppSvc   -> release() ; m_ppSvc   = 0 ; }    
    //
    LoKi::ErrorReport& rep = LoKi::ErrorReport::instance() ;
    if ( 0 != rep.reporter() ) { rep.setReporter( 0 ) ; }
    //
    { // goodbye message 
      MsgStream log ( msgSvc() , name() ) ;
      log << MSG::ALWAYS << std::endl ;
      LoKi::Welcome::instance ().goodbye( log.stream() ) ;
      log << endreq ;
    }
    //
    return Service::finalize() ;
  } ;

  /** general service reinitialization
   *  @see IService 
   *  @return status code
   */
  virtual StatusCode reinitialize() 
  {
    StatusCode sc = Service::reinitialize () ;
    if ( sc.isFailure() ) { return sc ; }
    //
    LoKi::Services& svc = LoKi::Services::instance() ;
    svc.releaseAll();
    if ( 0 == svc.lokiSvc() ) { svc.setLoKi( this ) ; }
    //
    if ( 0 != m_toolSvc ) { m_toolSvc -> release() ; m_toolSvc = 0 ; }    
    if ( 0 != m_ppSvc   ) { m_ppSvc   -> release() ; m_ppSvc   = 0 ; }
    //
    { // welcome message 
      MsgStream log ( msgSvc() , name() ) ;
      log << MSG::ALWAYS << std::endl ;
      LoKi::Welcome::instance ().welcome ( log.stream() ) ;
      log << endreq ;
    }
    //
    return StatusCode::SUCCESS ;
  } ;
  
public: 
  
  /** Query interfaces of Interface
   *  @param iid ID of Interface to be retrieved
   *  @param ppI Pointer to Location for interface pointer
   *  @see IInterface 
   */
  virtual StatusCode queryInterface 
  ( const InterfaceID& iid, void** ppI ) 
  {
    if      ( 0 == ppI ) 
    { return StatusCode::FAILURE ; }
    else if ( LoKi::ILoKiSvc::interfaceID    () == iid  ) 
    { *ppI = static_cast<LoKi::ILoKiSvc*>    ( this ) ; }
    else if ( IIncidentListener::interfaceID () == iid  ) 
    { *ppI = static_cast<IIncidentListener*> ( this ) ; }
    else if ( IService::interfaceID          () == iid  ) 
    { *ppI = static_cast<IService*>          ( this ) ; }
    else 
    { return    Service::queryInterface ( iid , ppI ) ; }
    //
    addRef() ;
    //
    return StatusCode::SUCCESS ;
  } ;  
protected:
  /** standard constructor 
   *  @param name service instance name 
   *  @param pointer to the service locator 
   */
  LoKiSvc 
  ( const std::string& name , 
    ISvcLocator*       pSvc ) 
    : Service ( name , pSvc )
    //
    , m_ppSvc        ( 0 ) 
    , m_toolSvc      ( 0 ) 
    , m_reporter     ( 0 ) 
    , m_reporterName ( "LoKi::Reporter/ERROR")
    //
  { 
    declareProperty ( "Reporter" , m_reporterName ) ;
  } ;
  /// virtual and protected destructor 
  virtual ~LoKiSvc () 
  { 
    if ( 0 != m_reporter && 0 != m_toolSvc ) 
    { m_toolSvc -> releaseTool ( m_reporter ) ; }
    m_reporter = 0 ;
    if ( 0 != m_toolSvc ) { m_toolSvc -> release() ; m_toolSvc = 0 ; }    
    if ( 0 != m_ppSvc   ) { m_ppSvc   -> release() ; m_ppSvc   = 0 ; } 
  } ;  
private:
  // default constructor is disabled
  LoKiSvc () ;
  // copy constructor is disabled 
  LoKiSvc           ( const  LoKiSvc& ) ;
  // assignement operator is disabled 
  LoKiSvc& operator=( const  LoKiSvc& ) ;
private:
  //
  mutable IParticlePropertySvc* m_ppSvc        ;
  mutable IToolSvc*             m_toolSvc      ;
  mutable LoKi::IReporter*      m_reporter     ;
  std::string                   m_reporterName ;
};
// ============================================================================


// ============================================================================
// anonymous namespace to hide static factory
// ============================================================================
namespace 
{
  // ==========================================================================
  /** @var s_Factory
   *  concrete factory for instantiation of LoKiSvc instances 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-01-17
   */
  // ==========================================================================
  const SvcFactory<LoKiSvc> s_Factory;
} ;
// ============================================================================
/** @var LoKiSvcFactory
 *  abstract factory for instantiation of LoKiSvc instances 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-01-17
 */
// ============================================================================
const ISvcFactory&LoKiSvcFactory = s_Factory ;
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
