// Include files
#include <thread>

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
   m_context{nullptr},
   m_output{nullptr},
   m_run{0},
   m_tck{0},
   m_startOfRun{0}
{
   declareProperty("OutputConnection", m_outCon = "ipc:///tmp/hlt2mon_0");
   declareProperty("HltDecReportsLocation", m_decRepLoc = "Hlt2/DecReports");
   declareProperty("UpdateInterval", m_updateInterval = 10.);
}

//===============================================================================
HltMonitorSvc::~HltMonitorSvc()
{
   for (const auto& counter : m_counters) {
      delete counter.second;
   }
}

//===============================================================================
StatusCode HltMonitorSvc::initialize()
{
   StatusCode sc = Service::initialize();

   MsgStream msg(msgSvc(), name());
   msg << MSG::INFO << "Initialized ZeroMQ based HltMonitorSvc" << endmsg;
   sc = serviceLocator()->service("IncidentSvc", m_incidentSvc, true);
   if(!sc.isSuccess()) {
      msg << MSG::FATAL << "IncidentSvc not found" << endmsg;
      return sc;
   }
   m_incidentSvc->addListener(this, IncidentType::RunChange);
   m_incidentSvc->addListener(this, IncidentType::BeginRun);

   sc = serviceLocator()->service("UpdateManagerSvc", m_updMgrSvc, true);
   if (!sc.isSuccess()) {
      msg << MSG::FATAL << "UpdateManagerSvc not found" << endmsg;
      return StatusCode::FAILURE;
   }
   m_updMgrSvc->registerCondition(this, "Conditions/Online/LHCb/RunParameters",
                                  &HltMonitorSvc::updateConditions, m_runPars);
   sc = m_updMgrSvc->update(this);
   if (!sc.isSuccess()) {
      msg << MSG::FATAL << "Could not update run parameter condition." << endmsg;
      return sc;
   }

   sc = serviceLocator()->service("EventDataSvc", m_evtSvc, true);
   if (!sc.isSuccess()) {
      msg << MSG::FATAL << "EventDataSvc not found" << endmsg;
   }
   return sc;
}

//===============================================================================
StatusCode HltMonitorSvc::start()
{
   auto sc = Service::start();
   if (!sc.isSuccess()) return sc;

   if (!m_context) {
      m_context = new zmq::context_t{1};
      m_output = new zmq::socket_t{*m_context, ZMQ_PUB};
      m_output->connect(m_outCon.c_str());
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
   MsgStream msg(msgSvc(), name());

   delete m_output;
   m_output = nullptr;
   delete m_context;
   m_context = nullptr;

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
   return item<RateCounter, decltype(m_counters)>(identifier, m_counters);
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
   return item<HltHistogram, decltype(m_histograms)>(identifier, m_histograms,
                                                     left, right, bins);
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
   MsgStream msg(msgSvc(), name());

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

   auto now = std::chrono::high_resolution_clock::now(); // System::currentTime( System::microSec );
   if ( m_latestUpdate.time_since_epoch().count() == 0 ||
        incident.type() == IncidentType::BeginRun ||
        incident.type() == "RunChange" )
      m_latestUpdate = now;
   using seconds = std::chrono::duration<double>;
   if (std::chrono::duration_cast<seconds>(std::chrono::high_resolution_clock::now() - m_latestUpdate).count()
       > m_updateInterval) {
      sendChunks();
      m_latestUpdate = now;
   }

   const RunChangeIncident* rci = dynamic_cast<const RunChangeIncident*>(&incident);
   if (rci) {
      if (m_run != rci->runNumber()) {
         MsgStream msg(msgSvc(), name());
         msg << MSG::DEBUG << "Change of run number detected " << m_run
             << " -> " << rci->runNumber() << endmsg;
      }
      m_run = rci->runNumber();

      // On a run change, always send the chunks
      sendChunks();
   }
}

//===============================================================================
void HltMonitorSvc::sendChunks()
{
   for (auto& entry : m_chunks) {
      // Don't send empty chunks.
      if (entry.second.data.empty()) continue;

      std::stringstream ss;
      boost::archive::text_oarchive oa{ss};
      oa << entry.second;
      auto s = ss.str();

      // Prepare message and send
      zmq::message_t msg(s.size());
      memcpy(static_cast<void*>(msg.data()), s.c_str(), s.size());
      m_output->send(msg);
      entry.second = Chunk{m_run, tck(), entry.second.histId};
   }
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
