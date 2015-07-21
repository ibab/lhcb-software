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
#include "HltMonitorSvc.h"

namespace {
   using std::string;
   using std::to_string;
   using std::vector;
   using Monitoring::Chunk;
}

// Factory for instantiation of service objects
DECLARE_COMPONENT(HltMonitorSvc)

//===============================================================================
HltMonitorSvc::HltMonitorSvc(const string& name, ISvcLocator* loc)
   : base_class(name, loc),
   m_zmqSvc{nullptr},
   m_incidentSvc{nullptr},
   m_updMgrSvc{nullptr},
   m_evtSvc{nullptr},
   m_runPars{nullptr},
   m_dataOut{nullptr},
   m_infoOut{nullptr},
   m_run{0},
   m_tck{0},
   m_startOfRun{0}
{
   declareProperty("DataConnection", m_dataCon = "ipc:///tmp/hlt2MonData_0");
   declareProperty("InfoConnection", m_infoCon = "ipc:///tmp/hlt2MonInfo_0");
   declareProperty("HltDecReportsLocation", m_decRepLoc = "Hlt2/DecReports");
   declareProperty("UpdateInterval", m_updateInterval = 10.);
}

//===============================================================================
HltMonitorSvc::~HltMonitorSvc()
{
   for (const auto& counter : m_counters) {
      delete counter.second;
   }
   for (const auto& histo : m_histograms) {
      delete histo.second;
   }
}

//===============================================================================
StatusCode HltMonitorSvc::initialize()
{
   StatusCode sc = Service::initialize();

   // IncidentSvc
   info() << "Initialized ZeroMQ based HltMonitorSvc" << endmsg;
   sc = serviceLocator()->service("IncidentSvc", m_incidentSvc, true);
   if(!sc.isSuccess()) {
      fatal() << "IncidentSvc not found" << endmsg;
      return sc;
   }
   m_incidentSvc->addListener(this, IncidentType::RunChange);
   m_incidentSvc->addListener(this, IncidentType::BeginEvent);

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
   m_updMgrSvc->registerCondition(this, "Conditions/Online/LHCb/RunParameters",
                                  &HltMonitorSvc::updateConditions, m_runPars);
   sc = m_updMgrSvc->update(this);
   if (!sc.isSuccess()) {
      fatal() << "Could not update run parameter condition." << endmsg;
      return sc;
   }

   sc = serviceLocator()->service("EventDataSvc", m_evtSvc, true);
   if (!sc.isSuccess()) {
      fatal() << "EventDataSvc not found" << endmsg;
   }
   return sc;
}

//===============================================================================
StatusCode HltMonitorSvc::start()
{
   auto sc = Service::start();
   if (!sc.isSuccess()) return sc;

   // Data output connection
   if (!m_dataOut) {
      m_dataOut = new zmq::socket_t{m_zmqSvc->context(), ZMQ_PUB};
      m_dataOut->connect(m_dataCon.c_str());
   }

   // Info output connection
   if (!m_infoOut) {
      m_infoOut = new zmq::socket_t{m_zmqSvc->context(), ZMQ_PUB};
      m_infoOut->connect(m_infoCon.c_str());
   }

   return sc;
}

//===============================================================================
StatusCode HltMonitorSvc::stop()
{
   return StatusCode::SUCCESS;
}

//===============================================================================
StatusCode HltMonitorSvc::finalize()
{
   // Clean up
   for (auto socket : {&m_dataOut, &m_infoOut}) {
      int linger = 0;
      (*socket)->setsockopt(ZMQ_LINGER, &linger, sizeof(linger));
      delete *socket;
      *socket = nullptr;
   }

   if (m_incidentSvc) {
      m_incidentSvc->removeListener(this);
      m_incidentSvc->release();
      m_incidentSvc = 0;
   }
   return Service::finalize();
}

//===============================================================================
RateCounter& HltMonitorSvc::rateCounter(const std::string& identifier) const
{
   auto& r = item<RateCounter, decltype(m_counters)>(identifier, m_counters);

   // Add info message
   addInfo(r.id().__hash__(), Monitoring::s_Rate, r.id().str());
   return r;
}

//===============================================================================
vector<string> HltMonitorSvc::counters() const
{
   vector<string> names;
   for (const auto& counter : m_counters) {
      names.push_back(counter.first.str());
   }
   return names;
}

//===============================================================================
void HltMonitorSvc::count(const Gaudi::StringKey& key, double t)
{
   // The clock that provides the start of run time might be a bit ahead of the
   // event clock, which would result in negative numbers here. Since that should (flw)
   // only happen at the start of run, we ignore those events.
   auto diff = t - m_startOfRun;
   if (diff < 0) {
      return;
   }
   auto bin = boost::numeric_cast<size_t>(diff);

   // Create a new chunk if we don't have one for this identifier
   auto it = m_chunks.find(key);
   if (it == end(m_chunks)) {
      it = m_chunks.emplace(key, Chunk{m_run, tck(), key.__hash__()}).first;
   }

   // Count
   if (!it->second.data.count(bin)) {
      it->second.data[bin] = 1;
   } else {
      it->second.data[bin] += 1;
   }
}

//===============================================================================
HltHistogram& HltMonitorSvc::histogram(const std::string& identifier,
                                       double left, double right,
                                       size_t bins) const
{
   auto& r = item<HltHistogram, decltype(m_histograms)>(identifier, m_histograms,
                                                                left, right, bins);

   // Serialize the Histo1DDef
   std::stringstream ss;
   {
      boost::archive::text_oarchive oa{ss};
      oa << r.def();
   }
   string s = ss.str();

   // Add info message
   addInfo(r.id().__hash__(), Monitoring::s_Histo1D, s);
   return r;
}

//===============================================================================
vector<string> HltMonitorSvc::histograms() const
{
   vector<string> names;
   for (const auto& entry : m_histograms) {
      names.push_back(entry.first.str());
   }
   return names;
}

//===============================================================================
void HltMonitorSvc::fill(const Gaudi::StringKey& key, size_t bin)
{
   // Create a new chunk if we don't have one for this identifier
   auto it = m_chunks.find(key);
   if (it == end(m_chunks)) {
      it = m_chunks.emplace(key, Chunk{m_run, tck(), key.__hash__()}).first;
   }

   // Count
   if (!it->second.data.count(bin)) {
      it->second.data[bin] = 1;
   } else {
      it->second.data[bin] += 1;
   }
}

//===============================================================================
void HltMonitorSvc::handle(const Incident& incident)
{
   const RunChangeIncident* rci = dynamic_cast<const RunChangeIncident*>(&incident);
   if (rci && m_run != rci->runNumber()) {
      debug() << "Change of run number detected " << m_run
              << " -> " << rci->runNumber() << endmsg;
      m_run = rci->runNumber();

      // Send all info messages again, with the new run.
      for (const auto& entry : m_infoMessages) {
         m_toSend.insert(entry.first);
      }

      // On a run change, always send the chunks
      sendChunks();
   }

   auto now = std::chrono::high_resolution_clock::now(); // System::currentTime( System::microSec );
   if ( m_latestUpdate.time_since_epoch().count() == 0 ||
        incident.type() == "RunChange" )
      m_latestUpdate = now;
   using seconds = std::chrono::duration<double>;
   if (std::chrono::duration_cast<seconds>(std::chrono::high_resolution_clock::now() - m_latestUpdate).count()
       > m_updateInterval) {
      sendChunks();
      m_latestUpdate = now;
   } else {
      sendChunks(false);
   }
}

//===============================================================================
void HltMonitorSvc::sendInfo()
{
   if (!m_run)
      return;

   for (auto msgId : m_toSend) {
      const auto& it = m_infoMessages.find(msgId);
      if (it == end(m_infoMessages)) {
         warning() << "No info message for id " << msgId << endmsg;
         continue;
      }

      auto& infoMsg = it->second;

      // First type of info
      auto type = Monitoring::s_HistoInfo;
      zmq::message_t msg(type.length());
      memcpy(static_cast<void*>(msg.data()), type.c_str(), type.length());
      m_infoOut->send(msg, ZMQ_SNDMORE);

      // Then run number
      msg.rebuild(sizeof(Monitoring::RunNumber));
      memcpy(msg.data(), &m_run, sizeof(Monitoring::RunNumber));
      m_infoOut->send(msg, ZMQ_SNDMORE);

      // Then HistId
      msg.rebuild(sizeof(Monitoring::HistId));
      memcpy(msg.data(), &infoMsg.id, msg.size());
      m_infoOut->send(msg, ZMQ_SNDMORE);

      // The type of histogram
      msg.rebuild(infoMsg.type.length());
      memcpy(static_cast<void*>(msg.data()), infoMsg.type.c_str(), msg.size());
      m_infoOut->send(msg, ZMQ_SNDMORE);

      // The infomation
      msg.rebuild(infoMsg.info.length());
      memcpy(static_cast<void*>(msg.data()), infoMsg.info.c_str(), msg.size());
      m_infoOut->send(msg);
   }

   // Clear set of messages to send
   m_toSend.clear();
}

//===============================================================================
void HltMonitorSvc::sendChunks(bool all)
{
   if (UNLIKELY(all && msgLevel(MSG::DEBUG))) {
      info() << "Sending all histogram chunks." << endmsg;
   }

   sendInfo();

   for (auto& entry : m_chunks) {
      // Don't send empty chunks and if all is false, don't send small chunks.
      if (entry.second.data.empty() ||
          (!all && (entry.second.data.size() < boost::numeric_cast<size_t>(m_updateInterval)))) {
         continue;
      }

      if (UNLIKELY(!all && msgLevel(MSG::DEBUG))) {
         info() << "Sending histogram chunk " << entry.second.runNumber
                << " " << entry.second.histId << endmsg;
      }

      // serialize
      std::stringstream ss;
      {
         boost::archive::text_oarchive oa{ss};
         oa << entry.second;
      }
      string s = ss.str();

      // Prepare message and send
      zmq::message_t msg(s.size() + 1);
      memcpy(static_cast<void*>(msg.data()), s.c_str(), s.size() + 1);
      m_dataOut->send(msg);
      entry.second = Chunk{m_run, tck(), entry.second.histId};
   }
}

//===============================================================================
void HltMonitorSvc::addInfo(Monitoring::HistId id, const std::string& type,
                            const std::string& inf) const
{
   debug() << "Adding info: " << id << " " << type << " " << inf << endmsg;

   auto msgId = m_infoMessages.size();
   m_infoMessages.emplace(msgId, InfoMessage{id, type, inf});
   m_toSend.insert(msgId);
}

//===============================================================================
unsigned int HltMonitorSvc::tck() const
{
   if (UNLIKELY(!m_tck)) {
      // Get the newly created HltDecReports to obtain the TCK
      DataObject* obj{nullptr};
      StatusCode sc = m_evtSvc->retrieveObject(m_decRepLoc, obj);
      if (!sc.isSuccess()) {
         throw GaudiException(name() + " Could not retrieve LHCb::HltDecReports from " + m_decRepLoc, "",
                              StatusCode::FAILURE);
      }
      const LHCb::HltDecReports* decReps = static_cast<const LHCb::HltDecReports*>(obj);
      m_tck = decReps->configuredTCK();
   }
   return m_tck;
}

//===============================================================================
StatusCode HltMonitorSvc::updateConditions()
{
    if (m_runPars == 0) {
        error() << "Could not obtain Condition for run parameters from conditions DB" << endmsg;
        return StatusCode::FAILURE;
    }
    if (!m_runPars->exists("RunStartTime")) {
        error() << "Condition does not contain RunStartTime " << endmsg;
        return StatusCode::FAILURE;
    }
    m_startOfRun = m_runPars->param<int>("RunStartTime"); // seconds
    return StatusCode::SUCCESS;
}
