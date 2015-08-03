// $Id$
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
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IAlgContextSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/IStatSvc.h"
#include "GaudiKernel/ICounterSvc.h"
#include "GaudiKernel/IChronoSvc.h"
#include "GaudiKernel/IUpdateManagerSvc.h"
#include "GaudiKernel/ServiceLocatorHelper.h"
// ============================================================================
// PartProp
// ============================================================================
#include "Kernel/IParticlePropertySvc.h"
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/ILoKiSvc.h"
#include "LoKi/IReporter.h"
#include "LoKi/ErrorReport.h"
#include "LoKi/Welcome.h"
#include "LoKi/Exception.h"
#include "LoKi/Services.h"
#include "LoKi/AuxFunBase.h"
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
  : public                Service
  , public virtual LoKi::ILoKiSvc 
  , public virtual IIncidentListener 
{
  // ==========================================================================
  // the friend factory for instantiation 
  friend class SvcFactory<LoKiSvc> ;
  // ==========================================================================
public:
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
  virtual unsigned long long event       () const { return m_event ; }
  // ==========================================================================
public:
  // ==========================================================================
  /** get the pointer to service locator 
   *  @return ponter to Service Locator 
   *  @see LoKi::ILoKiSvc
   *  @see IScvLocator 
   */
  ISvcLocator*          svcLoc() const 
  { return Service::serviceLocator() ; } 
  // ==========================================================================
  /** get the pointer to Particle Property Service 
   *  @return pointer to Particle Property Service 
   *  @see LoKi::ILoKiSvc
   *  @see IParticlePropertySvc 
   */
  LHCb::IParticlePropertySvc* ppSvc() const 
  {
    if ( 0 != m_ppSvc ) { return m_ppSvc ; }
    // locate the service 
    StatusCode sc = service ( "LHCb::ParticlePropertySvc" , m_ppSvc , true ) ;
    if       ( sc.isFailure() ) 
    {
      m_ppSvc = 0 ;
      LOKI_EXCEPTION ( "LoKiSvc: 'PPSvc' could not be located" , sc ) ; 
    }
    if  ( 0 == m_ppSvc ) 
    {
      LOKI_EXCEPTION ( "LoKiSvc: IPPSvc* points to NULL"       , sc ) ; 
    }
    //
    return m_ppSvc ;
  } 
  // ==========================================================================  
  /** get the pointer to Tool Service 
   *  @return pointer to Tool Service 
   *  @see LoKi::ILoKiSvc
   *  @see IToolSvc 
   */
  IToolSvc*             toolSvc () const 
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
  IAlgContextSvc* contextSvc () const 
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
  IIncidentSvc* incidentSvc () const 
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
  /** get the pointer to Histogram Service  
   *  @return pointer to Histogram Service 
   *  @see IIncidentSvc
   */
  IHistogramSvc* histoSvc () const 
  {
    if ( 0 != m_histoSvc ) { return m_histoSvc ; }
    // locate the service 
    StatusCode sc = service ( "HistogramDataSvc" , m_histoSvc , true ) ;
    if ( sc.isFailure() ) 
    { 
      m_histoSvc = 0 ;
      LOKI_EXCEPTION( "LoKiSvc: 'HistogramDataSvc' could not be located" , sc ) ; 
    }
    if ( 0 == m_histoSvc ) 
    { 
      LOKI_EXCEPTION( "LoKiSvc: IHistogramSvc* points to NULL"       , sc ) ; 
    }
    //
    return m_histoSvc ;
  }   
  // ==========================================================================
  /** get the pointer to Event Data Provider 
   *  @return pointer to Event Data Provider 
   *  @see IDataPrioviderSvc
   */
  IDataProviderSvc* evtSvc () const 
  {
    if ( 0 != m_evtSvc ) { return m_evtSvc ; }
    // locate the service 
    StatusCode sc = service ( "EventDataSvc" , m_evtSvc , true ) ;
    if ( sc.isFailure() ) 
    { 
      m_evtSvc = 0 ;
      LOKI_EXCEPTION( "LoKiSvc: 'EventDataSvc' could not be located" , sc ) ; 
    }
    if ( 0 == m_evtSvc ) 
    { 
      LOKI_EXCEPTION( "LoKiSvc: IDataProviderSvc* points to NULL"       , sc ) ; 
    }
    //
    return m_evtSvc ;
  }   
  // ==========================================================================
  /** get the pointer to Random Numbers Service 
   *  @return pointer to Random Numbers Service 
   *  @see IRndmGenSvc 
   */
  IRndmGenSvc* rndmSvc () const 
  {
    if ( 0 != m_rndmSvc ) { return m_rndmSvc ; }
    // locate the service 
    StatusCode sc = service ( "RndmGenSvc" , m_rndmSvc , true ) ;
    if ( sc.isFailure() ) 
    { 
      m_rndmSvc = 0 ;
      LOKI_EXCEPTION( "LoKiSvc: 'RndmGenSvc' could not be located" , sc ) ; 
    }
    if ( 0 == m_rndmSvc ) 
    { 
      LOKI_EXCEPTION( "LoKiSvc: IRndmGenSvc* points to NULL"       , sc ) ; 
    }
    //
    return m_rndmSvc ;
  }   
  // ==========================================================================
  /** get the pointer to Statistics 
   *  @return pointer to Statistics Service 
   *  @see IStatSvc 
   */
  IStatSvc* statSvc () const 
  {
    if ( 0 != m_statSvc ) { return m_statSvc ; }
    // locate the service 
    StatusCode sc = service ( "ChronoStatSvc" , m_statSvc , true ) ;
    if ( sc.isFailure() ) 
    { 
      m_statSvc = 0 ;
      LOKI_EXCEPTION( "LoKiSvc: 'ChronoStatSvc' could not be located" , sc ) ; 
    }
    if ( 0 == m_statSvc ) 
    { 
      LOKI_EXCEPTION( "LoKiSvc: IStatSvc* points to NULL"       , sc ) ; 
    }
    //
    return m_statSvc ;
  }   
  // ==========================================================================
  /** get the pointer to Counter service 
   *  @return pointer to Counter Service 
   *  @see ICounterSvc 
   */
  ICounterSvc* cntSvc () const 
  {
    if ( 0 != m_cntSvc ) { return m_cntSvc ; }
    // locate the service 
    StatusCode sc = service ( "CounterSvc" , m_cntSvc , true ) ;
    if ( sc.isFailure() ) 
    { 
      m_cntSvc = 0 ;
      LOKI_EXCEPTION( "LoKiSvc: 'CounterSvc' could not be located" , sc ) ; 
    }
    if ( 0 == m_cntSvc ) 
    { 
      LOKI_EXCEPTION( "LoKiSvc: ICounterSvc* points to NULL"       , sc ) ; 
    }
    //
    return m_cntSvc ;
  }   
  // ==========================================================================
  /** get the pointer to Chrono service 
   *  @return pointer to Chrono Service 
   *  @see IChronoSvc 
   */
  IChronoSvc* chronoSvc () const 
  {
    if ( 0 != m_chronoSvc ) { return m_chronoSvc ; }
    // locate the service 
    StatusCode sc = service ( "ChronoStatSvc" , m_chronoSvc , true ) ;
    if ( sc.isFailure() ) 
    { 
      m_chronoSvc = 0 ;
      LOKI_EXCEPTION( "LoKiSvc: 'ChronoStatSvc' could not be located" , sc ) ; 
    }
    if ( 0 == m_chronoSvc ) 
    { 
      LOKI_EXCEPTION( "LoKiSvc: IChronoSvc* points to NULL" , sc ) ; 
    }
    //
    return m_chronoSvc ;
  }   
  // ==========================================================================
  /** get the pointer to Update Manager Service  
   *  @return pointer to Update MAnager Service 
   *  @see IChronoSvc 
   */
  IUpdateManagerSvc* updateSvc () const 
  {
    if ( 0 != m_updateSvc ) { return m_updateSvc ; }
    // locate the service 
    StatusCode sc = service ( "UpdateManagerSvc", m_updateSvc , true ) ;
    if ( sc.isFailure() ) 
    { 
      m_updateSvc = 0 ;
      LOKI_EXCEPTION( "LoKiSvc: 'UpdateManagerSvc' could not be located" , sc ) ; 
    }
    if ( 0 == m_updateSvc ) 
    { 
      LOKI_EXCEPTION( "LoKiSvc: IUpdateManagerSvc* points to NULL" , sc ) ; 
    }
    //
    return m_updateSvc ;
  }   
  // ==========================================================================
public:
  // ==========================================================================
  /// Inform that a new incident has occured
  virtual void handle ( const Incident& inc ) 
  {
    if ( IncidentType::BeginEvent == inc.type() ) 
    {
      if   ( m_event < std::numeric_limits<unsigned long long>::max() )
      { ++m_event ; }
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
    //
    if (m_welcome) { // welcome message 
      MsgStream log ( msgSvc() , name() ) ;
      log << MSG::ALWAYS << std::endl ;
      LoKi::Welcome::instance ().welcome( log.stream() ) ;
      log << endmsg ;
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
      sc = svc -> retrieveTool 
        ( m_reporterName , m_reporter     , this ) ;
      if ( sc.isFailure()  ) { return StatusCode::FAILURE ; }
      if ( 0 == m_reporter ) { return StatusCode::FAILURE ; }
    }
    //
    LoKi::Services& svc = LoKi::Services::instance() ;
    if ( 0 == svc.lokiSvc() ) { svc.setLoKi ( this ) ; }
    //
    // validate AuxFunBase 
    if ( !LoKi::AuxFunBase::lokiSvc() ) 
    { LoKi::AuxFunBase::setLoKiSvc ( this ) ; }
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
    // Invalidate AuxFunBase 
    if ( LoKi::AuxFunBase::lokiSvc().same( this ) ) 
    { LoKi::AuxFunBase::setLoKiSvc ( 0 ) ; }
    //
    if ( 0 != m_toolSvc     ) { m_toolSvc     -> release() ; m_toolSvc     = 0 ; }    
    if ( 0 != m_ppSvc       ) { m_ppSvc       -> release() ; m_ppSvc       = 0 ; } 
    if ( 0 != m_contextSvc  ) { m_contextSvc  -> release() ; m_contextSvc  = 0 ; } 
    if ( 0 != m_incidentSvc ) { m_incidentSvc -> release() ; m_incidentSvc = 0 ; } 
    if ( 0 != m_histoSvc    ) { m_histoSvc    -> release() ; m_histoSvc    = 0 ; } 
    if ( 0 != m_evtSvc      ) { m_evtSvc      -> release() ; m_evtSvc      = 0 ; } 
    if ( 0 != m_rndmSvc     ) { m_rndmSvc     -> release() ; m_rndmSvc     = 0 ; } 
    if ( 0 != m_statSvc     ) { m_statSvc     -> release() ; m_statSvc     = 0 ; } 
    if ( 0 != m_cntSvc      ) { m_cntSvc      -> release() ; m_cntSvc      = 0 ; } 
    if ( 0 != m_chronoSvc   ) { m_chronoSvc   -> release() ; m_chronoSvc   = 0 ; } 
    if ( 0 != m_updateSvc   ) { m_updateSvc   -> release() ; m_updateSvc   = 0 ; } 
    //
    LoKi::ErrorReport& rep = LoKi::ErrorReport::instance() ;
    if ( 0 != rep.reporter() ) { rep.setReporter( 0 ).ignore() ; }
    //
    if (m_welcome) { // goodbye message 
      MsgStream log ( msgSvc() , name() ) ;
      log << MSG::ALWAYS << std::endl ;
      LoKi::Welcome::instance ().goodbye( log.stream() ) ;
      log << endmsg ;
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
    // Invalidate AuxFunBase 
    if ( LoKi::AuxFunBase::lokiSvc().same( this ) ) 
    { LoKi::AuxFunBase::setLoKiSvc ( 0 ) ; }
    //
    if ( 0 != m_toolSvc     ) { m_toolSvc     -> release() ; m_toolSvc     = 0 ; }    
    if ( 0 != m_ppSvc       ) { m_ppSvc       -> release() ; m_ppSvc       = 0 ; } 
    if ( 0 != m_contextSvc  ) { m_contextSvc  -> release() ; m_contextSvc  = 0 ; } 
    if ( 0 != m_incidentSvc ) { m_incidentSvc -> release() ; m_incidentSvc = 0 ; } 
    if ( 0 != m_histoSvc    ) { m_histoSvc    -> release() ; m_histoSvc    = 0 ; } 
    if ( 0 != m_evtSvc      ) { m_evtSvc      -> release() ; m_evtSvc      = 0 ; } 
    if ( 0 != m_rndmSvc     ) { m_rndmSvc     -> release() ; m_rndmSvc     = 0 ; } 
    if ( 0 != m_statSvc     ) { m_statSvc     -> release() ; m_statSvc     = 0 ; } 
    if ( 0 != m_cntSvc      ) { m_cntSvc      -> release() ; m_cntSvc      = 0 ; } 
    if ( 0 != m_chronoSvc   ) { m_chronoSvc   -> release() ; m_chronoSvc   = 0 ; } 
    if ( 0 != m_updateSvc   ) { m_updateSvc   -> release() ; m_updateSvc   = 0 ; } 
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
    // validate AuxFunBase 
    if ( !LoKi::AuxFunBase::lokiSvc() ) 
    { LoKi::AuxFunBase::setLoKiSvc ( this ) ; }
    //
    if (m_welcome) { // welcome message 
      MsgStream log ( msgSvc() , name() ) ;
      log << MSG::ALWAYS << std::endl ;
      LoKi::Welcome::instance ().welcome ( log.stream() ) ;
      log << endmsg ;
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
    if      ( 0 == ppI ) { return StatusCode::FAILURE ; }
    //
    *ppI = 0 ;
    if      ( LoKi::ILoKiSvc::interfaceID    () == iid  ) 
    { *ppI = static_cast<LoKi::ILoKiSvc*>    ( this ) ; }
    else if ( IIncidentListener::interfaceID () == iid  ) 
    { *ppI = static_cast<IIncidentListener*> ( this ) ; }
    //
    // dispatch to the concrete service: 
    //
    // Tool Service 
    else if ( IToolSvc::interfaceID             () == iid && 0 != toolSvc     () )
    { return toolSvc     ()     -> queryInterface ( iid , ppI ) ;}
    // Particle Property Service 
    else if ( LHCb::IParticlePropertySvc::interfaceID () == iid && 0 != ppSvc () ) 
    { return ppSvc       ()     -> queryInterface ( iid , ppI ) ; }
    // Algorithm  Context Service 
    else if ( IAlgContextSvc::interfaceID       () == iid && 0 != contextSvc  () ) 
    { return contextSvc  ()     -> queryInterface ( iid , ppI ) ; }
    // Incident Service 
    else if ( IIncidentSvc::interfaceID         () == iid && 0 != incidentSvc () ) 
    { return incidentSvc ()     -> queryInterface ( iid , ppI ) ; }
    // ServiceLocator  
    else if ( ISvcLocator::interfaceID          () == iid && 0 != svcLoc      () ) 
    { return svcLoc      ()     -> queryInterface ( iid , ppI ) ; }
    // IAlgManager 
    else if ( IAlgManager::interfaceID          () == iid && 0 != svcLoc      () ) 
    { return svcLoc      ()     -> queryInterface ( iid , ppI ) ; }
    // IHistogramSvc 
    else if ( IHistogramSvc::interfaceID        () == iid && 0 != histoSvc    () ) 
    { return histoSvc    ()     -> queryInterface ( iid , ppI ) ; }
    // IDataProviderSvc 
    else if ( IDataProviderSvc::interfaceID     () == iid && 0 != evtSvc      () ) 
    { return evtSvc      ()     -> queryInterface ( iid , ppI ) ; }
    // IRndmGenSvc 
    else if ( IRndmGenSvc::interfaceID          () == iid && 0 != rndmSvc     () ) 
    { return rndmSvc     ()     -> queryInterface ( iid , ppI ) ; }
    // IStatSvc 
    else if ( IStatSvc::interfaceID             () == iid && 0 != statSvc     () ) 
    { return statSvc     ()     -> queryInterface ( iid , ppI ) ; }
    // ICounterSvc 
    else if ( ICounterSvc::interfaceID          () == iid && 0 != cntSvc      () ) 
    { return cntSvc      ()     -> queryInterface ( iid , ppI ) ; }
    // IChronoSvc 
    else if ( IChronoSvc::interfaceID           () == iid && 0 != chronoSvc   () ) 
    { return chronoSvc   ()     -> queryInterface ( iid , ppI ) ; }
    // IUpdateManagerSvc 
    else if ( IUpdateManagerSvc::interfaceID    () == iid && 0 != updateSvc   () ) 
    { return updateSvc   ()     -> queryInterface ( iid , ppI ) ; }
    // a bit more fun with the reporter 
    else if ( LoKi::IReporter::interfaceID      () == iid && 0 != reporter    () ) 
    { return reporter    ()     -> queryInterface ( iid , ppI ) ; }
    // message service ? 
    else if ( IMessageSvc::interfaceID          () == iid && 0 != msgSvc      () ) 
    { return msgSvc      ()     -> queryInterface ( iid , ppI ) ; }
    // message service ?
    else 
    { return    Service::queryInterface ( iid , ppI ) ; }
    //
    addRef() ;
    //
    return StatusCode::SUCCESS ;
  } 
  // ==========================================================================
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
    , m_histoSvc     (  0 ) 
    , m_evtSvc       (  0 ) 
    , m_rndmSvc      (  0 ) 
    , m_statSvc      (  0 )
    , m_cntSvc       (  0 ) 
    , m_chronoSvc    (  0 ) 
    , m_updateSvc    (  0 ) 
    //
    //
    , m_reporter     (  0 )
    , m_reporterName ( "LoKi::Reporter/REPORT")
    , m_event        (  0 )
    , m_welcome      ( true )
    //
  {
    declareProperty 
      ( "Reporter" , m_reporterName , "The type/name of default Reporter tool") ;
    declareProperty 
      ( "Welcome" , m_welcome , "Show Welcome message") ;
  } 
  // ==========================================================================
  /// virtual and protected destructor
  virtual ~LoKiSvc () 
  { 
    if ( 0 != m_reporter && 0 != m_toolSvc ) 
    { m_toolSvc -> releaseTool ( m_reporter ) ; }
    m_reporter = 0 ;
    //
    if ( 0 != m_toolSvc     ) { m_toolSvc     -> release() ; m_toolSvc     = 0 ; }    
    if ( 0 != m_ppSvc       ) { m_ppSvc       -> release() ; m_ppSvc       = 0 ; } 
    if ( 0 != m_contextSvc  ) { m_contextSvc  -> release() ; m_contextSvc  = 0 ; } 
    if ( 0 != m_incidentSvc ) { m_incidentSvc -> release() ; m_incidentSvc = 0 ; } 
    if ( 0 != m_histoSvc    ) { m_histoSvc    -> release() ; m_histoSvc    = 0 ; } 
    if ( 0 != m_evtSvc      ) { m_evtSvc      -> release() ; m_evtSvc      = 0 ; } 
    if ( 0 != m_rndmSvc     ) { m_rndmSvc     -> release() ; m_rndmSvc     = 0 ; } 
    if ( 0 != m_statSvc     ) { m_statSvc     -> release() ; m_statSvc     = 0 ; } 
    if ( 0 != m_cntSvc      ) { m_cntSvc      -> release() ; m_cntSvc      = 0 ; } 
    if ( 0 != m_chronoSvc   ) { m_chronoSvc   -> release() ; m_chronoSvc   = 0 ; } 
    if ( 0 != m_updateSvc   ) { m_updateSvc   -> release() ; m_updateSvc   = 0 ; } 
  }   
  // ==========================================================================
private:
  // ==========================================================================
  /// default constructor is disabled
  LoKiSvc () ;                                        // no default constructor 
  /// copy constructor is disabled 
  LoKiSvc           ( const  LoKiSvc& ) ;                            // no copy 
  /// assignement operator is disabled 
  LoKiSvc& operator=( const  LoKiSvc& ) ;            // no assignement operator 
  // ==========================================================================
private:
  // ==========================================================================
  /// the particle property service 
  mutable LHCb::IParticlePropertySvc* m_ppSvc  ; // the particle property service 
  /// the tool service 
  mutable IToolSvc*             m_toolSvc      ;            // the tool service 
  /// the context service 
  mutable IAlgContextSvc*       m_contextSvc   ;         // the context service 
  /// the incident service 
  mutable IIncidentSvc*         m_incidentSvc  ;        // the incident service 
  /// the histogram service 
  mutable IHistogramSvc*        m_histoSvc     ;        //    histogram service 
  /// the event data service 
  mutable IDataProviderSvc*     m_evtSvc       ;        //   event data service 
  /// random numbers service 
  mutable IRndmGenSvc*          m_rndmSvc      ;        //       random numbers 
  /// statistics service 
  mutable IStatSvc*             m_statSvc      ;        //           statistics 
  /// counters  
  mutable ICounterSvc*          m_cntSvc       ;        //             counters
  /// chrono
  mutable IChronoSvc*           m_chronoSvc    ;        //               chrono
  /// update
  mutable IUpdateManagerSvc*    m_updateSvc    ;        //               update
  /// the default reporter 
  mutable LoKi::IReporter*      m_reporter     ;        // the default reporter 
  /// the name of the default reporter 
  std::string                   m_reporterName ;        // the name of reporter 
  /// the event marker 
  unsigned long long            m_event        ;            // the event marker
  /// print welcome message
  bool                          m_welcome      ;       // print welcome message 
  // ==========================================================================
};
// ============================================================================
DECLARE_SERVICE_FACTORY(LoKiSvc)
// ============================================================================
// The END 
// ============================================================================
