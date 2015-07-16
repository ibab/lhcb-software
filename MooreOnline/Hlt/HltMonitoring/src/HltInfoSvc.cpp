// Include files
#include <thread>
#include <functional>

// boost
#include <boost/regex.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/numeric/conversion/cast.hpp>

// zeromq
#include "zmq/zmq.hpp"

// Gaudi
#include <GaudiKernel/IIncidentSvc.h>
#include <GaudiKernel/IDataProviderSvc.h>
#include <GaudiKernel/IAlgorithm.h>
#include <GaudiKernel/IAlgTool.h>
#include <GaudiKernel/SvcFactory.h>
#include <GaudiKernel/MsgStream.h>

// HLT
#include <Kernel/RateCounter.h>
#include <Hlt2Monitoring/Chunk.h>
#include <Hlt2Monitoring/Serialize.h>

// Det
#include <DetDesc/RunChangeIncident.h>

// Event
#include <Event/HltDecReports.h>

// Local
#include "HltInfoSvc.h"

namespace {
   using std::string;
   using std::to_string;
   using std::vector;
   using Monitoring::Chunk;
}

// Factory for instantiation of service objects
DECLARE_COMPONENT(HltInfoSvc)

//===============================================================================
HltInfoSvc::HltInfoSvc(const string& name, ISvcLocator* loc)
   : base_class(name, loc),
   m_zmqSvc{nullptr},
   m_incidentSvc{nullptr},
   m_updMgrSvc{nullptr},
   m_infoOut{nullptr},
   m_run{0}
{
   declareProperty("InfoConnection", m_infoCon = "ipc:///tmp/hlt2MonInfo_0");
   declareProperty("LumiSettingsCondition", m_lumiCond = "Conditions/Online/LHCb/Lumi/LumiSettings");
}

//===============================================================================
HltInfoSvc::~HltInfoSvc()
{
}

//===============================================================================
StatusCode HltInfoSvc::initialize()
{
   StatusCode sc = Service::initialize();

   // IncidentSvc
   info() << "Initialized ZeroMQ based HltInfoSvc" << endmsg;
   sc = serviceLocator()->service("IncidentSvc", m_incidentSvc, true);
   if(!sc.isSuccess()) {
      fatal() << "IncidentSvc not found" << endmsg;
      return sc;
   }
   m_incidentSvc->addListener(this, IncidentType::RunChange);

   // ZQM service
   sc = serviceLocator()->service("ZeroMQSvc", m_zmqSvc, true);
   if( !sc.isSuccess() ) {
      fatal() << "ZeroMQSvc not found" << endmsg;
      return sc;
   }

   // UpdateManagerSvc
   sc = serviceLocator()->service("UpdateManagerSvc", m_updMgrSvc, true);
   if (!sc.isSuccess()) {
      fatal() << "UpdateManagerSvc not found" << endmsg;
      return StatusCode::FAILURE;
   }

   m_conditions[m_lumiCond] = nullptr;
   m_updMgrSvc->registerCondition(this, m_lumiCond, &HltInfoSvc::updateConditions,
                                  m_conditions[m_lumiCond]);
   sc = m_updMgrSvc->update(this);
   if (!sc.isSuccess()) {
      fatal() << "Could not update lumi settings condition." << endmsg;
   }
   return sc;
}

//===============================================================================
StatusCode HltInfoSvc::start()
{
   auto sc = Service::start();
   if (!sc.isSuccess()) return sc;

   // Info output connection
   if (!m_infoOut) {
      m_infoOut = new zmq::socket_t{m_zmqSvc->context(), ZMQ_PUB};
      m_infoOut->connect(m_infoCon.c_str());
   }

   return sc;
}

//===============================================================================
StatusCode HltInfoSvc::stop()
{
   return StatusCode::SUCCESS;
}

//===============================================================================
StatusCode HltInfoSvc::finalize()
{
   // Clean up
   int linger = 0;
   m_infoOut->setsockopt(ZMQ_LINGER, &linger, sizeof(linger));
   delete m_infoOut;
   m_infoOut = nullptr;

   if (m_incidentSvc) {
      m_incidentSvc->removeListener(this);
      m_incidentSvc->release();
      m_incidentSvc = 0;
   }
   return Service::finalize();
}

//===============================================================================
void HltInfoSvc::handle(const Incident& incident)
{
   const RunChangeIncident* rci = dynamic_cast<const RunChangeIncident*>(&incident);
   if (rci) {
      if (m_run != rci->runNumber()) {
         debug() << "Change of run number detected " << m_run
                 << " -> " << rci->runNumber() << endmsg;
      }
      m_run = rci->runNumber();
      m_info.run = m_run;
      sendInfo();
   }
}

//===============================================================================
void HltInfoSvc::sendInfo()
{
   if (FSMState() < Gaudi::StateMachine::RUNNING) {
      return;
   }
   debug() << "Sending run info." << endmsg;

   // First type of info
   auto it = Monitoring::s_RunInfo;
   zmq::message_t msg(it.length());
   memcpy(static_cast<void*>(msg.data()), it.c_str(), it.length());
   m_infoOut->send(msg, ZMQ_SNDMORE);

   std::stringstream ss;
   {
      boost::archive::text_oarchive oa{ss};
      oa << m_info;
   }
   string info = ss.str();

   // Prepare message and send
   msg.rebuild(info.size());
   memcpy(static_cast<void*>(msg.data()), info.c_str(), info.length());
   m_infoOut->send(msg);
}

//===============================================================================
StatusCode HltInfoSvc::updateConditions()
{
    if (m_conditions[m_lumiCond] == 0) {
        error() << "Could not obtain Condition for run parameters from conditions DB" << endmsg;
        return StatusCode::FAILURE;
    }
    if (!m_conditions[m_lumiCond]->exists("LumiPars")) {
        error() << "Condition does not contain LumiPars " << endmsg;
        return StatusCode::FAILURE;
    }
    m_info.lumiPars = m_conditions[m_lumiCond]->paramVect<double>("LumiPars"); // seconds
    return StatusCode::SUCCESS;
}
