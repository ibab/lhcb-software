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
  , m_msgStream {}                                  // the local message stream
  //
  , m_frozen    { false }            // the global glag to disable registration
//
  , m_lockers {   }                                   //     the current locker
  , m_locked  {   }                                   //    the list of lockers
//
  /// THE MAJOR STORAGE OF SELECTIONS, the map { id -> selection }
  , m_selections {}                          // THE MAJOR STORAGE OF SELECTIONS
  //
  , m_outputs    {}                      // structure for the output selections
  , m_inputs     {}                      // structure for the input  selections
  //
  , m_incSvc    { nullptr }
  , m_annSvc    { nullptr }
//
  , m_pedantic  { true  }
  , m_anonymous { false }
  , m_spy       { false }
//
  , m_cntMap    { }
  , m_cntEvent  { false }
  , m_statPrint { false }
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
    ( "Spy"       , m_spy , "Spy mode" ) ;
  // PrintStatistics
  declareProperty
    ( "StatPrint" , m_statPrint , "Print Statistics" )
    -> declareUpdateHandler
    ( &Hlt::Service::printStatHandler, this  ) ;
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
  msg << endmsg ;
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
  msg << endmsg ;
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
  if ( !ppi ) { return StatusCode::FAILURE ; }
  // "switch"
  // Hlt Register ?
  if      ( Hlt::IRegister    ::interfaceID () == iid )
  {
    *ppi =  static_cast<Hlt::IRegister*>    ( this )      ;
    addRef() ;
    return StatusCode::SUCCESS ;                                       // RETURN
  }
  // Hlt Data ?
  if ( Hlt::IData        ::interfaceID () == iid )
  {
    *ppi =  static_cast<Hlt::IData*>        ( this )      ;
    addRef() ;
    return StatusCode::SUCCESS ;                                       // RETURN
  }
  // Hlt Inspector ?
  if ( Hlt::IInspector   ::interfaceID () == iid )
  {
    *ppi =  static_cast<Hlt::IInspector*>   ( this )      ;
    addRef() ;
    return StatusCode::SUCCESS ;                                       // RETURN
  }
  // "Assigned Names & Numbers"?
  if ( IANNSvc           ::interfaceID () == iid )
  {      return  annSvc() -> queryInterface ( iid , ppi ) ; }          // RETURN
  // "Assigned Names"?
  if ( IANSvc            ::interfaceID () == iid )
  {      return  annSvc() -> queryInterface ( iid , ppi ) ; }          // RETURN
  //   LoKi ?
  if ( LoKi::ILoKiSvc    ::interfaceID () == iid )
  {      return lokiSvc() -> queryInterface ( iid , ppi ) ; }          // RETURN
  // Incident listener?
  if ( IIncidentListener ::interfaceID () == iid )
  {
    *ppi =  static_cast<IIncidentListener*> ( this )      ;
    addRef() ;
    return StatusCode::SUCCESS ;                                      // RETURN
  }

  // try basic interfaces form the base class
  StatusCode sc = ::Service::queryInterface ( iid , ppi ) ;           // RETURN
  if ( sc.isSuccess () && *ppi ) { return sc ; }

  // try to get something indirectly from LoKi service
  sc = lokiSvc () -> queryInterface ( iid , ppi ) ;
  if (  sc.isSuccess() && *ppi ) { return sc ; }                      // RETURN

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
  // check Incident Service & subscribe the incident
  incSvc () -> addListener ( this , IncidentType::BeginEvent ) ;
  incSvc () -> addListener ( this , IncidentType::EndEvent   ) ;
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
  IIncidentSvc* svc { nullptr };
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
  IANNSvc* svc { nullptr } ;
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
  LoKi::ILoKiSvc* svc { nullptr } ;
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
  //
  if  ( m_statPrint ) { printStat ( MSG::ALWAYS ) ; }
  //
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
void Hlt::Service::handle ( const Incident& inc )
{
  if ( !m_frozen )
  {
    MsgStream& msg = msgStream ( MSG::INFO ) ;
    msg <<  "The Service is frozen! No further registrations are allowed!" <<endmsg ;
    if ( !m_lockers.empty() )
    { Error ("Service is going to be frozen, but it is locked!") ; }
    //
    m_frozen = true ;
  }
  //
  if      ( IncidentType::BeginEvent == inc.type() ) { m_cntEvent = true ; }
  else if ( IncidentType::  EndEvent == inc.type() )
  {
    // ======================================================
    // clear all selections at end of the event
    for ( auto&  isel : m_selections )
    {
      Hlt::Selection* sel = isel.second ;
      if ( sel )
      {
        //
        if ( m_cntEvent )
        {
          CntMap::mapped_type& cnts = m_cntMap[ sel->id() ] ;
          StatEntity&          rate = cnts.first  ;
          StatEntity&          size = cnts.second ;
          //
          rate += sel -> decision() ;
          if ( !sel->empty() ) { size += sel -> size () ; }
        }
        //
        sel -> clean () ;
      }
    }
    //
    m_cntEvent = false ;
  }
  //
}
// ============================================================================
// Handler for statistics printout
// ============================================================================
void Hlt::Service::printStatHandler  ( Property& /* theProp */ )
{
  // no action if not yet initialized
  if ( this -> FSMState() < Gaudi::StateMachine::INITIALIZED ) { return ; }
  if ( m_statPrint    ) { this -> printStat ( MSG::ALWAYS ) ; }
}
// ============================================================================
// Print statistics
// ============================================================================
namespace
{
  // ==========================================================================
  // the header row for counters printout
  const std::string s_header  =
    " |    Counter                                      |     #     |    sum     | mean/eff^* | rms/err^*  |     min     |     max     |" ;
  // format for regular statistical printout rows
  const std::string s_format1 =
    " | %|-48.48s|%|50t||%|10d| |%|11.7g| |%|#11.5g| |%|#11.5g| |%|#12.5g| |%|#12.5g| |" ;
  // format for "efficiency" statistical printout rows
  const std::string s_format2 =
    " |*%|-48.48s|%|50t||%|10d| |%|11.5g| |(%|#9.6g| +- %|-#9.6g|)%%|   -------   |   -------   |" ;
  const std::string s_line    = " +" + std::string(128,'-') + '+';
  // ==========================================================================
} //                                                 end of anonymous namespace
// ============================================================================
std::size_t Hlt::Service::printStat( const MSG::Level level ) const
{
  // print statistics
  if ( m_cntMap.empty() ) { return 0 ; }
  MsgStream& msg = msgStream ( level ) ;
  //
  msg << "Number of counters : "  << m_cntMap.size() ;
  //
  // Size-counter
  //
  msg << std::endl << s_line << std::endl << s_header << std::endl << s_line ;
  for ( const auto &entry : m_cntMap )
  {
    msg << std::endl
        << Gaudi::Utils::formatAsTableRow
      ( entry .  first                ,
        entry .  second.second        ,
        false                         ,
        s_format1                     , s_format2 ) ;
  }
  //
  // Rate counters
  //
  msg << std::endl << s_line << std::endl << s_header << std::endl << s_line ;
  for ( auto& entry : m_cntMap )
  {
    msg << std::endl
        << Gaudi::Utils::formatAsTableRow
      ( "acc: " + entry .  first.str() ,
        entry .  second.first          ,
        true                           ,
        s_format1                      , s_format2 ) ;
  }
  //
  msg << std::endl << s_line << endmsg ;
  //
  return m_cntMap.size() ;
}
// ============================================================================
// the factory
// ============================================================================
typedef Hlt::Service _Hlt_Service ;
DECLARE_SERVICE_FACTORY(_Hlt_Service)
// ============================================================================
// The END
// ============================================================================
