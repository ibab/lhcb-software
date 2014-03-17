#include "GaudiKernel/Auditor.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/VectorMap.h"
#include "GaudiKernel/HashMap.h"
#include "GaudiKernel/MsgStream.h"
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/ISequencerTimerTool.h"
// ============================================================================
#ifdef __ICC
// disable icc warning #654: overloaded virtual function "B::Y" is only partially overridden in class "C"
//   TODO: there is only a partial overload of IAuditor::before and IAuditor::after
#pragma warning(disable:654)
#endif
/** @class LHCbTimingAuditor
 *
 *  Simple auditor which uses SequencerTimerTool for *ALL*
 *  algorithms, including the algorithm from main Gaudi "TopAlg" list
 *  and algorithms managed by Data-On-Demand Service
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2007-01-31
 */
class LHCbTimingAuditor: public extends1<Auditor, IIncidentListener> {
public:

  virtual void before(StandardEventType evt, INamedInterface* alg);
  virtual void after(StandardEventType evt, INamedInterface* alg, const StatusCode &sc);

  using Auditor::before; // avoid hiding base-class methods
  virtual void before(CustomEventTypeRef evt, const std::string& name);
  using Auditor::after; // avoid hiding base-class methods
  virtual void after(CustomEventTypeRef evt, const std::string& name, const StatusCode &sc);

private:
  void i_beforeInitialize( INamedInterface* alg );
  void i_afterInitialize( INamedInterface* alg );
  void i_beforeFinalize( INamedInterface* alg );
  void i_beforeExecute( INamedInterface* alg );
  void i_afterExecute( INamedInterface* alg);

public:
  /// Inform that a new incident has occurred
  virtual void handle ( const Incident& ) ;

public:
  virtual StatusCode initialize () ;
  virtual StatusCode finalize   () ;

public:
  /// standard constructor
  LHCbTimingAuditor
  ( const std::string& name ,
    ISvcLocator*       pSvc )
    : base_class ( name , pSvc )
    //
    , m_toolSvc    ( 0 )
    , m_incSvc     ( 0 )
    //
    , m_timer      ( 0 )
    //
    , m_appMgr     ( 0 )
    //
    , m_map        (       )
    , m_indent     ( 0     )
    , m_inEvent    ( false )
    , m_goodForDOD ( false )
    , m_mapUser    (       )
    , m_histoSaved ( false )
  {
    declareProperty ( "OptimizedForDOD" , m_goodForDOD ) ;
    declareProperty ( "TimingTool" , m_timingToolName="LHCbSequencerTimerTool/TIMER" ) ;
  } ;
  /// virtual destructor
  virtual ~LHCbTimingAuditor() {}
private:
  // the default constructor is disabled
  LHCbTimingAuditor () ;
  // copy constructor is disabled
  LHCbTimingAuditor           ( const LHCbTimingAuditor& ) ;
  // assignement operator is disabled
  LHCbTimingAuditor& operator=( const LHCbTimingAuditor& ) ;
private:
  // tool service
  IToolSvc*            m_toolSvc ; ///< tool service
  // incident service
  IIncidentSvc*        m_incSvc  ; ///< incident service
  // the timer tool
  ISequencerTimerTool* m_timer   ; ///< the timer tool
  // ApplicationManager
  INamedInterface*     m_appMgr  ; ///< ApplicationManager
  //
  typedef GaudiUtils::VectorMap<const INamedInterface*,int>  Map ;
  Map                  m_map     ;
  // indentation level
  int                  m_indent  ; ///< indentation level
  // "in event"
  bool                 m_inEvent ; ///< "In event" flag
  // "optimized for Data-On-Demand Service"
  bool                 m_goodForDOD ; ///< "optimized for DOD"
  //
  typedef GaudiUtils::HashMap<std::string,int> MapUser ;
  MapUser              m_mapUser ; ///< map used to record user timing events

  // Whether the timing has been saved already
  bool                 m_histoSaved;
  
  //name of the timing tool to be used
  std::string m_timingToolName;


} ;
// ============================================================================
/// factory:
// ============================================================================
DECLARE_COMPONENT(LHCbTimingAuditor)
// ============================================================================
StatusCode LHCbTimingAuditor::initialize ()
{
  StatusCode sc = Auditor::initialize() ;
  if ( sc.isFailure() ) { return sc ; }                  // RETURN

  MsgStream log ( msgSvc() , name() ) ;

  // get tool service
  if ( 0 == m_toolSvc )
  {
    sc = Auditor::service ( "ToolSvc" , m_toolSvc ) ;
    if ( sc.isFailure() )
    {
      log << "Could not retrieve 'ToolSvc' " << sc << endmsg ;
      return sc ;                                        // RETURN
    }
    if ( 0 == m_timer )
    {
      sc = m_toolSvc->retrieveTool
        ( m_timingToolName , m_timer , this  , true ) ;
      if ( sc.isFailure() )
      {
        log << MSG::ERROR
            << "Could not retrieve ISequencerTimerTool" << endmsg ;
        return sc ;
      }
    }
  }
  // get incident service
  if ( 0 == m_incSvc )
  {
    sc = Auditor::service ( "IncidentSvc" , m_incSvc ) ;
    if ( sc.isFailure() )
    {
      log << MSG::ERROR
          << "Could not retrieve 'IncidentSvc'" << sc << endmsg ;
      return sc ;
    }
    m_incSvc -> addListener ( this , IncidentType::BeginEvent ) ;
    m_incSvc -> addListener ( this , IncidentType::EndEvent   ) ;
  }
  // get the application manager
  if ( 0 == m_appMgr )
  {
    sc = Auditor::service ( "ApplicationMgr" , m_appMgr ) ;
    if ( sc.isFailure() )
    {
      log << MSG::ERROR
          << "Could not retrieve 'ApplicationMgr'" << sc << endmsg ;
      return sc ;
    }
    if ( m_map.end() == m_map.find( m_appMgr ) )
    {
      int timer = m_timer->addTimer( "EVENT LOOP" ) ;
      m_map.insert ( m_appMgr , timer ) ;
    }
  }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
StatusCode LHCbTimingAuditor::finalize   ()
{
  if ( 0 != m_incSvc )
  {
    m_incSvc -> removeListener ( this , IncidentType::BeginEvent ) ;
    m_incSvc -> removeListener ( this , IncidentType::EndEvent   ) ;
    m_incSvc -> release () ;
    m_incSvc = 0 ;
  }
  if ( 0 != m_toolSvc )
  {
    // the 2 following line are commented out: it is
    // is a temporary hack which prevent a crash due to a problem in
    // the reference counting
    //     if ( 0 != m_timer )
    //     { m_toolSvc -> releaseTool ( m_timer ) . ignore() ; m_timer = 0 ; }
    m_toolSvc -> release () ;
    m_toolSvc = 0 ;
  }
  if ( 0 != m_appMgr ) { m_appMgr -> release () ;  m_appMgr = 0 ; }
  // clear the map
  m_map.clear() ;
  // finalize the base class
  return Auditor::finalize () ;
}
// ============================================================================
void LHCbTimingAuditor::before(StandardEventType evt, INamedInterface *alg)
{
  switch (evt) {
  case IAuditor::Initialize : i_beforeInitialize( alg ); break;
  case IAuditor::Execute    : i_beforeExecute( alg );    break;
  case IAuditor::Finalize   : i_beforeFinalize( alg );   break;
  default: break;
  }
}
// ============================================================================
void LHCbTimingAuditor::after(StandardEventType evt, INamedInterface *alg, const StatusCode &)
{
  switch (evt) {
  case IAuditor::Initialize : i_afterInitialize( alg ); break;
  case IAuditor::Execute    : i_afterExecute( alg ); break;
  default: break;
  }
}
// ============================================================================
void LHCbTimingAuditor::i_beforeFinalize( INamedInterface* /*alg*/ )
{
  if (!m_histoSaved)
  {
    m_timer->saveHistograms();
    m_histoSaved = true;
  }
}

// ============================================================================
void LHCbTimingAuditor::i_beforeInitialize( INamedInterface* alg )
{
  if ( m_goodForDOD ) { return ; }
  //
  if ( 0 == alg ) { return ; }
  Map::iterator found = m_map.find( alg ) ;
  if ( m_map.end() != found ) { return ; }
  ++m_indent ;
  std::string nick = alg->name() ;
  if ( 0 < m_indent ) { nick = std::string ( m_indent , ' ') + nick ; }
  if ( m_inEvent )
  {
    nick[0] = '*' ;
    MsgStream log( msgSvc() , name() ) ;
    log << MSG::DEBUG
        << "Insert non-structural component '"
        << alg->name() << "' of type '"
        << System::typeinfoName(typeid(*alg)) << "' at level "
        << m_indent << endmsg ;
  }
  int timer = m_timer->addTimer( nick ) ;
  m_map.insert ( alg , timer ) ;
  m_timer->start( timer ) ;
}
// ============================================================================
void LHCbTimingAuditor::i_afterInitialize( INamedInterface* alg )
{
  if ( m_goodForDOD ) { return ; }
  if ( 0 == alg     ) { return ; }
  --m_indent ;
}
// ============================================================================
void LHCbTimingAuditor::i_beforeExecute( INamedInterface* alg )
{
  if ( 0 == alg ) { return ; }
  ++m_indent ;
  Map::iterator found = m_map.find( alg ) ;
  if ( m_map.end() == found )
  {
    MsgStream log( msgSvc() , name() ) ;
    log << MSG::DEBUG
        << "Insert non-structural component '"
        << alg->name() << "' of type '"
        << System::typeinfoName(typeid(*alg)) << "' at level "
        << m_indent << endmsg ;
    std::string nick = alg->name() ;
    if ( 0 < m_indent  ) { nick = std::string ( m_indent , ' ') + nick ; }
    if ( !m_goodForDOD ) { nick[0]='*' ;}
    int timer = m_timer->addTimer( nick ) ;
    m_map.insert ( alg , timer ) ;
    m_timer->start( timer ) ;
    return ;
  }
  m_timer->start( found->second ) ;
}
// ============================================================================
void LHCbTimingAuditor::i_afterExecute( INamedInterface* alg )
{
  if ( 0 == alg ) { return ; }
  Map::iterator found = m_map.find( alg ) ;
  if ( m_map.end() == found ) { return ; }
  m_timer->stop( found->second ) ;
  --m_indent ;
}
// ============================================================================
void LHCbTimingAuditor::before(CustomEventTypeRef evt, const std::string& name)
{
  // Ignore obvious mistakes
  if ( name.empty() && evt.empty() ) { return; }

  // look for the user timer in the map
  int timer = 0;
  std::string nick = name + ":" + evt;
  MapUser::iterator found = m_mapUser.find( nick );

  if ( m_mapUser.end() == found ) {
    // add a new timer if not yet available
    timer = m_timer->addTimer( nick ) ;
    m_mapUser[nick] = timer;
  }
  else {
    timer = found->second;
  }

  m_timer->start( timer );
}
// ============================================================================
void LHCbTimingAuditor::after(CustomEventTypeRef evt, const std::string& name, const StatusCode &)
{
  // Ignore obvious mistakes
  if ( name.empty() && evt.empty() ) { return; }

  // look for the user timer in the map
  std::string nick = name + ":" + evt;
  MapUser::iterator found = m_mapUser.find( nick );

  // We cannot do much if the timer is not available
  if ( m_mapUser.end() == found ) {
    MsgStream log(msgSvc(), this->name());
    log << MSG::WARNING << "Trying to stop the measure  of the timing for '"
                        << nick << "' but it was never started. Check the code"
                        << endmsg;
    return;
  }
  m_timer->stop( found->second );
}
// ============================================================================
void LHCbTimingAuditor::handle ( const Incident& i )
{
  if      ( IncidentType::BeginEvent == i.type () )
  {
    m_timer -> start ( m_map[ m_appMgr ] ) ;
    ++m_indent ;
    m_inEvent = true ;
  }
  else if ( IncidentType::EndEvent   == i.type () )
  {
    m_timer -> stop  ( m_map[ m_appMgr ] ) ;
    --m_indent ;
    m_inEvent = false ;
  }
}
// ============================================================================
// The END
// ============================================================================
