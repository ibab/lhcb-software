#include <limits>
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiAlg/IGenericTool.h"
#include "Kernel/IReferenceRate.h"
#include "LoKi/IHltFactory.h"
#include "boost/algorithm/string/join.hpp"


class HltReferenceRateSvc : public Service,
                    virtual public IReferenceRate,
                    virtual public IIncidentListener
  
{
public:
  HltReferenceRateSvc(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~HltReferenceRateSvc( ) ;

  /** Query interfaces (\see{IInterface})
      @param riid       ID of Interface to be retrieved
      @param ppvUnknown Pointer to Location for interface pointer
  */
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvUnknown);

  /// Initialize Service
  StatusCode initialize();

  /// Finalize  Service
  StatusCode finalize();

  /// Handle BeginEvent incident.
  void handle(const Incident&);

  /// Return the rate (reciproke period) of this clock 
  double rate() const { return m_rate ; }

  /// Return the current tick of this clock
  size_t tick() const { return m_tick ; }
  
private:
  MsgStream& info() const { return msg(MSG::INFO); }
  MsgStream& error() const { return msg(MSG::ERROR); }
  MsgStream& msg(MSG::Level level) const ;

  StatusCode decode();
  void updatePredicate( Property& /* p */ );
  void updatePreambulo ( Property& /* p */ );

  bool updateRequired() const { return m_predicate_updated || m_preambulo_updated; }

private:
  mutable std::auto_ptr<MsgStream> m_msg ;
  IIncidentSvc      *m_incidentSvc ;
  IDataProviderSvc  *m_evtSvc;
  IToolSvc          *m_toolSvc;
  LoKi::Types::ODIN_Cut* m_predicate;
  ToolHandle<IGenericTool>     m_decodeOdin;
  std::vector<std::string> m_preambulo_ ;             // the preambulo itself
  std::string m_preambulo ;                           // the preambulo itself
  std::string m_predicateDesc;
  std::string m_location;
  double            m_rate ;
  ulonglong         m_first,m_last;
  mutable size_t    m_tick ;
  bool m_predicate_updated;
  bool m_preambulo_updated;
};

/************************************************************************/

#include "GaudiKernel/SvcFactory.h" 
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "Event/ODIN.h"

DECLARE_SERVICE_FACTORY( HltReferenceRateSvc );


//=============================================================================
// IInterface implementation
//=============================================================================
StatusCode HltReferenceRateSvc::queryInterface(const InterfaceID& riid, void** ppvUnknown){
  if ( IReferenceRate::interfaceID().versionMatch(riid) ) {
    *ppvUnknown = (IReferenceRate*)this;
    addRef();
    return StatusCode::SUCCESS;
  }
  return Service::queryInterface(riid,ppvUnknown);
}

HltReferenceRateSvc::~HltReferenceRateSvc()
{
}

HltReferenceRateSvc::HltReferenceRateSvc( const std::string& name, 
                                          ISvcLocator* pSvcLocator) 
  : Service ( name , pSvcLocator )
  , m_incidentSvc(0)
  , m_evtSvc(0)
  , m_toolSvc(0)
  , m_predicate(0)
  , m_decodeOdin("ODINDecodeTool",this)
  , m_rate(0)
  , m_first( ~ulonglong(0) )
  , m_last(0)
  , m_tick(0)
  , m_predicate_updated(false)
  , m_preambulo_updated(false)
{
    // by default, we assume 50(beam-beam)+10(beam1)+10(beam2)+10(empty) Hz of lumi triggers
  declareProperty("ReferenceRate", m_rate = 80/*Gaudi::Units::Hz*/ ) ;
  declareProperty("ODINLocation", m_location = LHCb::ODINLocation::Default);
  declareProperty("ODINPredicate", m_predicateDesc = "ODIN_TRGTYP == LHCb.ODIN.LumiTrigger") 
                 -> declareUpdateHandler( & HltReferenceRateSvc::updatePredicate, this );
  declareProperty("Preambulo", m_preambulo_)
                 ->declareUpdateHandler(&HltReferenceRateSvc::updatePreambulo , this);
}

MsgStream& HltReferenceRateSvc::msg(MSG::Level level) const {
  if (m_msg.get()==0) m_msg.reset( new MsgStream( msgSvc(), name() ));
  *m_msg << level;
  return *m_msg;
}

StatusCode HltReferenceRateSvc::decode() {
    delete m_predicate;
    m_predicate = 0;
    LoKi::Types::ODIN_Cut cut( LoKi::BasicFunctors<const LHCb::ODIN*>::BooleanConstant( false ) );
    if (!m_predicateDesc.empty()) { 
        LoKi::Hybrid::IHltFactory* _factory(0);
        StatusCode sc = m_toolSvc->retrieveTool("LoKi::Hybrid::HltFactory",_factory,this) ;
        if (sc.isFailure()) return sc;
        sc = _factory->get( m_predicateDesc, cut, m_preambulo );
        m_toolSvc->releaseTool(_factory) ;
        if (sc.isFailure()) return sc;
    }
    m_predicate = cut.clone();
    m_predicate_updated = false ;
    m_preambulo_updated = false ;
    info() << "Reference rate for predicate " <<  *m_predicate <<  " is set to " << m_rate << endreq ;
    return StatusCode::SUCCESS;
}

StatusCode
HltReferenceRateSvc::initialize()
{
  StatusCode sc = Service::initialize() ;
  if( !sc.isSuccess() ) return sc;
  sc = service("ToolSvc",m_toolSvc);
  if( !sc.isSuccess() ) return sc;
  sc = service("EventDataSvc",m_evtSvc);
  if( !sc.isSuccess() ) return sc;
  sc = m_decodeOdin.retrieve();
  if( !sc.isSuccess()) return sc;
  sc = service( "IncidentSvc", m_incidentSvc);
  if( !sc.isSuccess() ) return sc;
  // insert at low priority, so that ODIN is available when we get invoked..
  long prio = std::numeric_limits<long>::min();
  m_incidentSvc->addListener(this,IncidentType::BeginEvent, prio ) ;

  sc = decode();
  if( !sc.isSuccess() ) return sc;

  m_tick = 0 ;

  return sc;
}

StatusCode
HltReferenceRateSvc::finalize()
{
  if (m_first<m_last) {
      Gaudi::Time first( m_first*1000);
      Gaudi::Time last (  m_last*1000);
      Gaudi::TimeSpan interval( last-first) ;
      info() << "Got " << m_tick << " ticks " 
             << " in "<<  interval.seconds() << " s."
             << " during [ " << first.format(true,"%F %T") << "," << last.format(true,"%F %T")  << endmsg;
      if (interval.seconds()>0) {
          info() << "This corresponds to an average rate of " << m_tick/interval.seconds() << " Hz."
                 << ", compared to an assumed 'set' rate of " << m_rate << endmsg;
      }
  } 
  StatusCode sc = Service::finalize() ;
  m_toolSvc->release();
  m_evtSvc->release();
  m_incidentSvc->release();
  return sc;
}

void HltReferenceRateSvc::handle ( const Incident& /*incident*/ )
{
  if (updateRequired() ) {
       StatusCode sc = decode();
       if (sc.isFailure()) throw GaudiException( "Error from HltReferenceRateSvc::decode()","HltReferenceRateSvc failure" , sc ) ;
  }
  if ( m_predicate == 0 ) {
      ++m_tick;
      return;
  }
  // get the ODIN bank
  SmartDataPtr<LHCb::ODIN> odin( m_evtSvc , m_location );
  if (!odin) m_decodeOdin->execute();
  if (!odin) {
        error() << " Could not obtain ODIN...  requesting AbortEvent" << endmsg;
        m_incidentSvc->fireIncident(Incident(name(),IncidentType::AbortEvent));
        return;
  } 
  if ( (*m_predicate)(odin) ) ++m_tick;
  ulonglong gps = odin->gpsTime();
  if ( gps < m_first ) m_first = gps;
  if ( gps > m_last  ) m_last = gps;
}

//=============================================================================
// update handlers
//=============================================================================
void HltReferenceRateSvc::updatePredicate ( Property& /* p */ )    
{
  /// mark as "to-be-updated"
  m_predicate_updated = true ;
  // no action if not yet initialized
  if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
  // postpone the action
  if ( !m_preambulo_updated ) { return ; }
  // perform the actual immediate decoding
  StatusCode sc = decode  () ;
  if (sc.isFailure()) throw GaudiException( "Error from HltReferenceRateSvc::decode()","HltReferenceRateSvc failure" , sc ) ;
}

void HltReferenceRateSvc::updatePreambulo ( Property& /* p */ )
{
  // concatenate the preambulo:
  m_preambulo = boost::algorithm::join( m_preambulo_ , "\n" );
  /// mark as "to-be-updated"
  m_preambulo_updated = true ;
  // no further action if not yet initialized
  if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
  // postpone the action
  if ( !m_predicate_updated ) { return ; }

  // perform the actual immediate decoding
  StatusCode sc = decode  () ;
  if (sc.isFailure()) throw GaudiException( "Error from HltReferenceRateSvc::decode()","HltReferenceRateSvc failure" , sc ) ;
}

