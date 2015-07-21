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
#include <GaudiKernel/IDataProviderSvc.h>
#include <GaudiKernel/IAlgorithm.h>
#include <GaudiKernel/IAlgTool.h>
#include <GaudiKernel/SvcFactory.h>
#include <GaudiKernel/MsgStream.h>

// HLT
#include <Kernel/RateCounter.h>
#include <Hlt2Monitoring/Chunk.h>
#include <Hlt2Monitoring/Serialize.h>

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
   : Service(name, loc),
   m_zmqSvc{nullptr},
   m_updMgrSvc{nullptr},
   m_infoOut{nullptr}
{
   declareProperty("InfoConnection", m_infoCon = "ipc:///tmp/hlt2MonInfo_0");
   declareProperty("LumiSettingsCondition", m_lumiCond = "Conditions/Online/LHCb/Lumi/LumiSettings");
   declareProperty("StartTimeCondition", m_runCond = "Conditions/Online/LHCb/RunParameters");
}

//===============================================================================
HltInfoSvc::~HltInfoSvc()
{
}

//===============================================================================
StatusCode HltInfoSvc::initialize()
{
   StatusCode sc = Service::initialize();

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

   // Register conditions
   auto register_ = [this](const std::string& cond) {
      m_conditions[cond] = nullptr;
      m_updMgrSvc->registerCondition(this, cond, &HltInfoSvc::updateConditions,
                                     m_conditions[cond]);
   };

   register_(m_lumiCond);
   register_(m_runCond);

   sc = m_updMgrSvc->update(this);
   if (!sc.isSuccess()) {
      fatal() << "Could not update conditions." << endmsg;
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

   return Service::finalize();
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
   auto check = [this](const std::string& cond, std::string par) -> StatusCode {
      if (m_conditions[cond] == 0) {
         error() << "Could not obtain Condition " << cond << " from conditions DB" << endmsg;
        return StatusCode::FAILURE;
      }
      if (!m_conditions[cond]->exists(par)) {
         error() << "Condition does not contain " << par << endmsg;
         return StatusCode::FAILURE;
      }
      return StatusCode::SUCCESS;
   };

   // Lumi parameters
   auto sc = check(m_lumiCond, "LumiPars");
   if (!sc.isSuccess()) return sc;
   m_info.lumiPars = m_conditions[m_lumiCond]->paramVect<double>("LumiPars"); // seconds

   // Start time
   sc = check(m_runCond, "RunStartTime");
   if (!sc.isSuccess()) return sc;
   m_info.start = m_conditions[m_runCond]->param<int>("RunStartTime"); // seconds

   // Run number
   sc = check(m_runCond, "RunNumber");
   if (!sc.isSuccess()) return sc;
   m_info.run = m_conditions[m_runCond]->param<int>("RunNumber");

   // Send new info
   sendInfo();

   return StatusCode::SUCCESS;
}
