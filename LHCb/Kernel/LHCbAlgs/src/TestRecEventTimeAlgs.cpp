#include "GaudiKernel/Service.h"
#include "GaudiKernel/IEventTimeDecoder.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "Event/RecHeader.h"

namespace LHCbAlgsTest {

class CreateFakeRecHeader: public extends1<Service, IIncidentListener> {
public:
  CreateFakeRecHeader(const std::string& name, ISvcLocator *pSvcLocator) :
    base_class(name, pSvcLocator), m_timeDecoder(0)
  {
    declareProperty("EventTimeDecoder", m_timeDecoderName = "FakeEventTime",
                    "Tool to use to generate the event times for the fake RecHeader");
    declareProperty("RecHeaderLocation",
                    m_recHeaderLoc = LHCb::RecHeaderLocation::Default,
                    "Alternative location for RecHeader.");
  }
  virtual ~CreateFakeRecHeader()
  {
  }

  virtual StatusCode initialize()
  {
    StatusCode sc = base_class::initialize();
    if (sc.isFailure())
      return sc;

   SmartIF<IToolSvc> toolSvc(serviceLocator()->service("ToolSvc"));
   if (!toolSvc) return StatusCode::FAILURE;

   sc = toolSvc->retrieveTool(m_timeDecoderName, m_timeDecoder, this);
   if (!sc.isSuccess()) return sc;

   SmartIF<IIncidentSvc> incidentSvc(serviceLocator()->service("IncidentSvc"));
   if (!incidentSvc) return StatusCode::FAILURE;

   incidentSvc->addListener(this, IncidentType::BeginEvent);

   m_dataProvider = serviceLocator()->service("EventDataSvc");

   return StatusCode::SUCCESS;
  }

  virtual void handle(const Incident&)
  {
    Gaudi::Time t = m_timeDecoder->getTime();
    info() << "Adding RecHeader with event time " << t << endmsg;
    LHCb::RecHeader *h = new LHCb::RecHeader();
    h->setGpsTime(t.ns()/1000);
    m_dataProvider->registerObject(m_recHeaderLoc, h);
  }

private:
  /// Type of the IEventTimeDecoder instance.
  std::string m_timeDecoderName;

  /// IEventTimeDecoder instance
  IEventTimeDecoder *m_timeDecoder;

  /// Location of the RecHeader in the transient store.
  std::string m_recHeaderLoc;

  /// Pointer to the event transient store.
  SmartIF<IDataProviderSvc> m_dataProvider;
};

}

DECLARE_NAMESPACE_SERVICE_FACTORY( LHCbAlgsTest, CreateFakeRecHeader )
