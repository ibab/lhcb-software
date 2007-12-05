// $Id: LoKiSvc.cpp,v 1.12 2007-12-05 09:37:45 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <climits>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Service.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IAlgContextSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentSvc.h"
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
/*  @class LoKiSvc
 *  the simplest implementation of LoKi::ILoKiSvc interface 
 *  @see LoKi::ILoKiSvc 
 *  @date 2006-01-16 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 */
// ============================================================================
class LoKiSvc 
  : public virtual LoKi::ILoKiSvc 
  , public                Service 
{
  friend class SvcFactory<LoKiSvc> ;
public:
  // ==========================================================================
  /** get the pointer to service locator 
   *  @return ponter to Service Locator 
   *  @see LoKi::ILoKiSvc
   *  @see IScvLocator 
   */
  virtual ISvcLocator*          svcLoc() const 
  { return Service::serviceLocator() ; } 
  // ==========================================================================  
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
  } 
  // ==========================================================================
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
  } 
  // ==========================================================================
  /** get the pointer to Algorithm Context Service 
   *  @return pointer to Algorithm Context Service 
   *  @see LoKi::ILoKiSvc
   *  @see IToolSvc 
   */
  virtual IAlgContextSvc* contextSvc () const 
  {
    if ( 0 != m_contextSvc ) { return m_contextSvc ; }
    // locate the service 
    StatusCode sc = service ( "AlgContextSvc" , m_contextSvc , true ) ;
    if ( sc.isFailure() ) 
    { 
      m_contextSvc = 0 ;
      LOKI_EXCEPTION( "LoKiSvc: 'AlgContextSvc' could not be located" , sc ) ; 
    }
    if ( 0 == m_contextSvc ) 
    { 
      LOKI_EXCEPTION( "LoKiSvc: IAlgContextSvc* points to NULL"       , sc ) ; 
    }
    //
    return m_contextSvc ;
  }   
  // ==========================================================================
  /** get the pointer to Incident Service 
   *  @return pointer to Incident Service 
   *  @see IIncidentSvc
   */
  virtual IIncidentSvc* incidentSvc () const 
  {
    if ( 0 != m_incidentSvc ) { return m_incidentSvc ; }
    // locate the service 
    StatusCode sc = service ( "IncidentSvc" , m_incidentSvc , true ) ;
    if ( sc.isFailure() ) 
    { 
      m_incidentSvc = 0 ;
      LOKI_EXCEPTION( "LoKiSvc: 'IncidentSvc' could not be located" , sc ) ; 
    }
    if ( 0 == m_incidentSvc ) 
    { 
      LOKI_EXCEPTION( "LoKiSvc: IIncicentSvc* points to NULL"       , sc ) ; 
    }
    //
    return m_incidentSvc ;
  }   
  // ==========================================================================
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
  } 
  // ==========================================================================
  /** The the sequential event number 
   *  (needed for the proper synchronizations checks) 
   *  @return the sequential event number 
   */
  virtual long                 event       () const { return m_event ; }
public:
  // ==========================================================================
  /// Inform that a new incident has occured
  virtual void handle ( const Incident& inc ) 
  {
    if ( IncidentType::BeginEvent == inc.type() ) 
    {
      if   ( m_event < std::numeric_limits<long>::max() ) { ++m_event ; }
      else { m_event = 1 ; }
    }    
  } 
  // ==========================================================================
public:
  // ==========================================================================
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
    // RESET the sequential event number 
    m_event = 0  ;
    //
    {
      /// subscribe the incident:
      IIncidentSvc* isvc = incidentSvc() ;
      if ( 0 == isvc  ) { return StatusCode::FAILURE ; }  
      isvc -> addListener ( this , IncidentType::BeginEvent ) ; 
    }
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
    LoKi::Services& svc = LoKi::Services::instance() ;
    if ( 0 == svc.lokiSvc() ) { svc.setLoKi ( this ) ; }
    //
    LoKi::ErrorReport& rep = LoKi::ErrorReport::instance() ;
    if ( 0 == rep.reporter() && 0 != m_reporter ) 
    { rep.setReporter ( m_reporter ) ; }
    //
    return StatusCode::SUCCESS ;
  }   
  // ==========================================================================
  /** general service finalization 
   *  @see IService 
   *  @return status code
   */
  virtual StatusCode finalize () 
  {
    //
    { // static services 
      LoKi::Services& svc = LoKi::Services::instance() ;
      svc.releaseAll().ignore()  ;
    }
    //
    if ( 0 != m_reporter && 0 != m_toolSvc ) 
    { m_toolSvc -> releaseTool ( m_reporter ).ignore() ; }
    //
    m_reporter = 0 ;
    //
    { 
      /// unsubscribe the incident:
      IIncidentSvc* isvc = incidentSvc() ;
      if ( 0 == isvc  ) { return StatusCode::FAILURE ; }     // RETURN 
      isvc -> removeListener ( this ) ;
    }
    //
    if ( 0 != m_toolSvc     ) { m_toolSvc     -> release() ; m_toolSvc     = 0 ; }    
    if ( 0 != m_ppSvc       ) { m_ppSvc       -> release() ; m_ppSvc       = 0 ; }    
    if ( 0 != m_contextSvc  ) { m_contextSvc  -> release() ; m_contextSvc  = 0 ; }    
    if ( 0 != m_incidentSvc ) { m_incidentSvc -> release() ; m_incidentSvc = 0 ; } 
    //
    LoKi::ErrorReport& rep = LoKi::ErrorReport::instance() ;
    if ( 0 != rep.reporter() ) { rep.setReporter( 0 ).ignore() ; }
    //
    { // goodbye message 
      MsgStream log ( msgSvc() , name() ) ;
      log << MSG::ALWAYS << std::endl ;
      LoKi::Welcome::instance ().goodbye( log.stream() ) ;
      log << endreq ;
    }
    //
    return Service::finalize() ;
  } 
  // ==========================================================================
  /** general service reinitialization
   *  @see IService 
   *  @return status code
   */
  virtual StatusCode reinitialize() 
  {
    StatusCode sc = Service::reinitialize () ;
    if ( sc.isFailure() ) { return sc ; }
    //
    {
      /// unsubscribe the incident:
      IIncidentSvc* isvc = incidentSvc() ;
      if ( 0 == isvc  ) { return StatusCode::FAILURE ; }     // RETURN 
      isvc -> removeListener ( this ) ; 
    }
    //
    if ( 0 != m_toolSvc      ) { m_toolSvc     -> release() ; m_toolSvc     = 0 ; }    
    if ( 0 != m_ppSvc        ) { m_ppSvc       -> release() ; m_ppSvc       = 0 ; }
    if ( 0 != m_contextSvc   ) { m_contextSvc  -> release() ; m_contextSvc  = 0 ; }
    if ( 0 != m_incidentSvc  ) { m_incidentSvc -> release() ; m_incidentSvc = 0 ; }
    //
    {
      /// subscribe the incident:
      IIncidentSvc* isvc = incidentSvc() ;
      if ( 0 == isvc  ) { return StatusCode::FAILURE ; }           // RETURN 
      isvc -> addListener ( this , IncidentType::BeginEvent ) ; 
    }
    //
    { // static services:
      LoKi::Services& svc = LoKi::Services::instance() ;
      svc.releaseAll();
      if ( 0 == svc.lokiSvc() ) { svc.setLoKi( this ) ; }
    }
    //
    { // welcome message 
      MsgStream log ( msgSvc() , name() ) ;
      log << MSG::ALWAYS << std::endl ;
      LoKi::Welcome::instance ().welcome ( log.stream() ) ;
      log << endreq ;
    }
    //
    return StatusCode::SUCCESS ;
  } 
  // ==========================================================================  
public:
  // ==========================================================================
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
  // ==========================================================================
protected:  
protected:
  // ==========================================================================
  /** standard constructor 
   *  @param name service instance name 
   *  @param pSvc pointer to the service locator 
   */
  LoKiSvc 
  ( const std::string& name , 
    ISvcLocator*       pSvc ) 
    : Service ( name , pSvc )
    //
    , m_ppSvc        (  0 ) 
    , m_toolSvc      (  0 ) 
    , m_contextSvc   (  0 ) 
    , m_incidentSvc  (  0 ) 
    , m_reporter     (  0 )
    , m_reporterName ( "LoKi::Reporter/ERROR")
    , m_event        ( -1 )
    //
  { 
    declareProperty ( "Reporter" , m_reporterName ) ;
  } 
  // ==========================================================================
  /// virtual and protected destructor
  virtual ~LoKiSvc () 
  { 
    if ( 0 != m_reporter && 0 != m_toolSvc ) 
    { m_toolSvc -> releaseTool ( m_reporter ) ; }
    m_reporter = 0 ;
    if ( 0 != m_toolSvc     ) { m_toolSvc     -> release() ; m_toolSvc     = 0 ; }    
    if ( 0 != m_ppSvc       ) { m_ppSvc       -> release() ; m_ppSvc       = 0 ; } 
    if ( 0 != m_contextSvc  ) { m_contextSvc  -> release() ; m_contextSvc  = 0 ; } 
    if ( 0 != m_incidentSvc ) { m_incidentSvc -> release() ; m_incidentSvc = 0 ; } 
  }   
  // ==========================================================================
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
  mutable IAlgContextSvc*       m_contextSvc   ;
  mutable IIncidentSvc*         m_incidentSvc  ;
  mutable LoKi::IReporter*      m_reporter     ;
  std::string                   m_reporterName ;
  long                          m_event        ;
};
// ============================================================================

// ============================================================================
DECLARE_SERVICE_FACTORY(LoKiSvc)
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
