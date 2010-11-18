// $Id$
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/IIncidentSvc.h"
// ============================================================================
// Kernel
// ============================================================================
#include "Kernel/IANNSvc.h"
#include "Kernel/IANSvc.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/ILoKiSvc.h"
// ============================================================================
// Local 
// ============================================================================
#include "Service.h"
// ============================================================================
/** @file
 *  Implemetation file for class Hlt::Service 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-03-18
 */
// ============================================================================
/* Standard constructor (protected) 
 *  @param name the service instance name 
 *  @param pSvc service locator 
 */
// ============================================================================
Hlt::Service::Service 
( const std::string& name ,                            // service instance name 
  ISvcLocator*       pSvc ) :                          //       Service Locator 
  ::Service ( name , pSvc )                            //        the base class 
  //
  , m_msgStream ()                                  // the local message stream  
  //
  , m_frozen    ( false )            // the global glag to disable registration 
//
  , m_lockers (   )                                   //     the current locker 
  , m_locked  (   )                                   //    the list of lockers 
//
  /// THE MAJOR STORAGE OF SELECTIONS, the map { id -> selection } 
  , m_selections ()                          // THE MAJOR STORAGE OF SELECTIONS 
  //
  , m_outputs    ()                      // structure for the output selections 
  , m_inputs     ()                      // structure for the input  selections 
  //
  , m_incSvc    ( 0 ) 
  , m_annSvc    ( 0 )
  //  
  , m_pedantic  ( true  ) 
  , m_anonymous ( false )
  , m_spy       ( false )
{
  // Perform the pedantic checks of everything
  declareProperty 
    ( "Pedantic"       , 
      m_pedantic       , 
      "Perform the pedantic checks of everything" ) ;
  // Allow anonymous access to selections 
  declareProperty 
    ( "AllowAnonymous" , 
      m_anonymous      , 
      "Allow anonymous access to selections" ) ;
  // Spy ? 
  declareProperty 
    ( "Spy" , m_spy , "Spy mode" ) ;
  
}
// ============================================================================
// produce the error message 
// ============================================================================
StatusCode Hlt::Service::Error 
( const std::string& message , 
  const StatusCode&  sc      ) const 
{
  MsgStream& msg = err() ;
  //
  msg << message  << "  StatusCode=" ;
  //
  if      ( sc.isSuccess     () )                 { msg << "SUCCESS"     ; }
  else if ( sc.isRecoverable () )                 { msg << "RECOVERABLE" ; }
  else if ( StatusCode::FAILURE == sc.getCode() ) { msg << "FAILURE"     ; }
  else                         { msg << "FAILUE(" << sc.getCode() << ")" ; }
  //
  return sc ;  
}
// ============================================================================
// produce the warning message 
// ============================================================================
StatusCode Hlt::Service::Warning
( const std::string& message , 
  const StatusCode&  sc      ) const 
{
  MsgStream& msg = warning () ;
  //
  msg << message  << "  StatusCode=" ;
  //
  if      ( sc.isSuccess     () )                 { msg << "SUCCESS"     ; }
  else if ( sc.isRecoverable () )                 { msg << "RECOVERABLE" ; }
  else if ( StatusCode::FAILURE == sc.getCode() ) { msg << "FAILURE"     ; }
  else                         { msg << "FAILUE(" << sc.getCode() << ")" ; }
  //
  return sc ;  
}
// ============================================================================
// throw the exception 
// ============================================================================
void Hlt::Service::Exception 
( const std::string& message , 
  const StatusCode&  sc      ) const 
{ throw GaudiException ( name() + " " + message , "Hlt::Service" , sc ) ; }
// ============================================================================
/*  query the interface 
 *  @see IInterface 
 *  @param iid the uniqye interface ID 
 *  @param ppi (OUTPUT) the interface 
 *  @return status code 
 */
// ============================================================================
StatusCode Hlt::Service::queryInterface
(  const InterfaceID& iid ,
   void**             ppi ) 
{
  // check the placeholder:
  if ( 0 == ppi ) { return StatusCode::FAILURE ; }
  // "switch"
  // Hlt Register ?
  if      ( Hlt::IRegister    ::interfaceID () == iid ) 
  { 
    *ppi =  static_cast<Hlt::IRegister*>    ( this )      ;
    addRef() ;
    return StatusCode::SUCCESS ;                                       // RETURN 
  }
  // Hlt Data ?
  else if ( Hlt::IData        ::interfaceID () == iid ) 
  { 
    *ppi =  static_cast<Hlt::IData*>        ( this )      ; 
    addRef() ;
    return StatusCode::SUCCESS ;                                       // RETURN 
  }
  // Hlt Inspector ?
  else if ( Hlt::IInspector   ::interfaceID () == iid ) 
  { 
    *ppi =  static_cast<Hlt::IInspector*>   ( this )      ;
    addRef() ;
    return StatusCode::SUCCESS ;                                       // RETURN 
  }
  // "Assigned Names & Numbers"?
  else if ( IANNSvc           ::interfaceID () == iid ) 
  {      return  annSvc() -> queryInterface ( iid , ppi ) ; }          // RETURN
  // "Assigned Names"?
  else if ( IANSvc            ::interfaceID () == iid ) 
  {      return  annSvc() -> queryInterface ( iid , ppi ) ; }          // RETURN
  //   LoKi ?
  else if ( LoKi::ILoKiSvc    ::interfaceID () == iid ) 
  {      return lokiSvc() -> queryInterface ( iid , ppi ) ; }          // RETURN
  // Incident listener?
  else if ( IIncidentListener ::interfaceID () == iid ) 
  { 
    *ppi =  static_cast<IIncidentListener*> ( this )      ;
    addRef() ;
    return StatusCode::SUCCESS ;                                      // RETURN 
  }
  
  // try basic interfaces form the base class 
  StatusCode sc = ::Service::queryInterface ( iid , ppi ) ;           // RETURN 
  if ( sc.isSuccess () && 0 != *ppi ) { return sc ; }
  
  // try to get something indirectly from LoKi service
  sc = lokiSvc () -> queryInterface ( iid , ppi ) ;
  if (  sc.isSuccess() && 0 != *ppi ) { return sc ; }                 // REUTRN
  
  // try basic interfaces form the base class 
  return ::Service::queryInterface ( iid , ppi ) ;                    // RETURN 
}
// ============================================================================
/*  standard initialization 
 *  @see IService 
 *  @return status code 
 */
// ============================================================================
StatusCode Hlt::Service::initialize () 
{
  // initialize the base 
  StatusCode sc = ::Service::initialize () ;
  if ( sc.isFailure() ) { return StatusCode::FAILURE ; }              // RETURN
  // check LoKi Service 
  lokiSvc() ;
  // check Assigned Numbers & Names service 
  annSvc () ;
  // check Inciden tService & subscribe the incident 
  incSvc () -> addListener ( this , IncidentType::BeginEvent ) ;
  incSvc () -> addListener ( this , IncidentType::BeginRun   ) ;
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// access to incident service 
// ============================================================================
IIncidentSvc* Hlt::Service::incSvc() const 
{
  if ( m_incSvc.validPointer() ) { return m_incSvc ; }
  IIncidentSvc* svc  = 0 ;
  StatusCode sc = service ( "IncidentSvc" , svc , true ) ;
  Assert ( sc.isSuccess ()         , "Unable to locate IncidentSvc" , sc ) ;
  m_incSvc = svc ;
  Assert ( m_incSvc.validPointer() , "IncidentSvc points to NULL"        ) ;
  return m_incSvc ;
}
// ============================================================================
// access to "Assigned Numbers and Names" service 
// ============================================================================
IANNSvc* Hlt::Service::annSvc() const 
{
  if ( m_annSvc.validPointer()  ) { return m_annSvc ; }
  IANNSvc* svc = 0 ;
  StatusCode sc = service ( "HltANNSvc" , svc , true ) ;
  Assert ( sc.isSuccess ()         , "Unable to locate HltANNSvc" , sc ) ;
  m_annSvc = svc ;
  Assert ( m_annSvc.validPointer() , "IANNSvc  points to NULL"         ) ;
  return m_annSvc ;
}
// ============================================================================
// access to "Assigned Numbers and Names" service 
// ============================================================================
LoKi::ILoKiSvc* Hlt::Service::lokiSvc() const 
{
  if ( m_lokiSvc.validPointer()  ) { return m_lokiSvc ; }
  LoKi::ILoKiSvc* svc = 0 ;
  StatusCode sc = service ( "LoKiSvc" , svc , true ) ;
  Assert ( sc.isSuccess ()         , "Unable to locate LoKiSvc" , sc ) ;
  m_lokiSvc = svc ;
  Assert ( m_lokiSvc.validPointer() , "ILoKiSvc  points to NULL"     ) ;
  return m_lokiSvc ;
}
// ============================================================================
/*  standard finalization
 *  @see IService 
 *  @return status code 
 */
// ============================================================================
StatusCode Hlt::Service::finalize () 
{
  // @TODO Hlt::Service::finalize: print here some statistics 
  // clear everything:
  m_inputs     . clear   () ;
  m_outputs    . clear   () ;
  m_selections . clear   () ;
  m_locked     . clear   () ;
  m_lockers    . clear   () ;
  //
  // unsubscribe the incidents 
  if ( m_incSvc.validPointer() ) { m_incSvc -> removeListener ( this ) ; }
  // release the services 
  m_incSvc  . release () ;
  m_annSvc  . release () ;
  m_lokiSvc . release () ;
  // finalize the base class 
  return ::Service::finalize () ;
}  
// ============================================================================
// handle the incidents
// ============================================================================
void Hlt::Service::handle ( const Incident& /* inc */ ) 
{
  if ( !m_frozen ) 
  {
    Warning ( "The Service is frozen! No further registrations are allowed!") ;
    if ( !m_lockers.empty() ) 
    { Error ("Service is going to be frozen, but it is locked!") ; }
    //
    m_frozen = true ;
  } 
  // clear all selections 
  for ( SelMap::iterator isel = m_selections.begin() ; 
        m_selections.end() != isel ; ++isel ) 
  {
    Hlt::Selection* sel = isel->second ;
    if ( 0 != sel ) { sel -> clean () ; }
  } 
}
// ============================================================================


// ============================================================================
// the factory
// ============================================================================
typedef Hlt::Service _Hlt_Service ;
DECLARE_SERVICE_FACTORY(_Hlt_Service)
// ============================================================================
// The END 
// ============================================================================
