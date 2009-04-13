#include "GaudiKernel/Service.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "HltBase/IHltReferenceClockSvc.h"

class HltReferenceClockSvc : public Service,
			     virtual public IHltReferenceClockSvc,
			     virtual public IIncidentListener
  
{
public:
  HltReferenceClockSvc(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~HltReferenceClockSvc( ) ;
  StatusCode initialize();
  void handle(const Incident&);
  double rate() const { return m_rate ; }
  size_t tick() const { return m_tick ; }
  
private:
  MsgStream& info() const { return msg(MSG::INFO); }
  MsgStream& error() const { return msg(MSG::ERROR); }
  MsgStream& msg(MSG::Level level) const ;
private:
  int               m_triggerType ;
  double            m_rate ;
  mutable size_t    m_tick ;
  mutable std::auto_ptr<MsgStream> m_msg ;
  IIncidentSvc      *m_incidentSvc ;
  IDataProviderSvc  *m_evtSvc;
};

/************************************************************************/

#include "GaudiKernel/SvcFactory.h" 
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "Event/ODIN.h"

DECLARE_SERVICE_FACTORY( HltReferenceClockSvc );

HltReferenceClockSvc::~HltReferenceClockSvc()
{
}

HltReferenceClockSvc::HltReferenceClockSvc( const std::string& name, 
					  ISvcLocator* pSvcLocator) 
  : Service ( name , pSvcLocator ),
    m_triggerType(-1),
    m_rate(0),
    m_tick(0)
{
  declareProperty("ODINTriggerRate", m_rate = 2000/*Gaudi::Units::Hz*/ ) ;
  declareProperty("ODINTriggerType", m_triggerType = -1
		  /*LHCb::ODIN::RandomTrigger*/) ;
}

MsgStream& HltReferenceClockSvc::msg(MSG::Level level) const {
  if (m_msg.get()==0) m_msg.reset( new MsgStream( msgSvc(), name() ));
  *m_msg << level;
  return *m_msg;
}

StatusCode
HltReferenceClockSvc::initialize()
{
  StatusCode sc = Service::initialize() ;
  if (!service( "EventDataSvc", m_evtSvc).isSuccess()) return StatusCode::FAILURE;
  if( sc.isSuccess() ) {
    info() << "Reference rate is: " << m_rate << endreq ;
    m_tick = 0 ;
    if (!service( "IncidentSvc", m_incidentSvc).isSuccess()) 
      return StatusCode::FAILURE;
    m_incidentSvc->addListener(this,IncidentType::BeginEvent) ;
  }
}

void HltReferenceClockSvc::handle ( const Incident& incident )
{
  if ( IncidentType::BeginEvent == incident.type() ) {
    if( m_triggerType >= 0 ) {
      // get the ODIN bank
      SmartDataPtr<LHCb::ODIN> odin( m_evtSvc , LHCb::ODINLocation::Default );
      if (!odin) {
        error() << " Could not locate ODIN... " << endmsg;
        m_incidentSvc->fireIncident(Incident(name(),IncidentType::AbortEvent));
      } else {
	m_tick += 
	  int(odin->triggerType()) == m_triggerType &&
	  odin->bunchCrossingType() == LHCb::ODIN::BeamCrossing ? 1 : 0 ;
      }
    } else {
      m_tick += 1 ;
    }
  }
}
