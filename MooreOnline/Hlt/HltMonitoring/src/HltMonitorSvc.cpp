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
   declareProperty("ChunkOverlap", m_chunkOverlap = 1);
   declareProperty("UpdateInterval", m_updateInterval = 10);
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
void HltMonitorSvc::count(const Gaudi::StringKey& key, double t)
{
   // The clock that provides the start of run time might be a bit ahead of the
   // event clock, which would result in negative numbers here. Since that should (flw)
   // only happen at the start of run, we ignore those events.
   auto diff = t - m_startOfRun;
   if (diff < 0) {
      return;
   }

   // if (UNLIKELY(!m_tck)) {
   //    // Get the newly created HltDecReports to obtain the TCK
   //    DataObject* obj{nullptr};
   //    StatusCode sc = m_evtSvc->retrieveObject(m_decRepLoc, obj);
   //    if (!sc.isSuccess()) {
   //       throw GaudiException(name() + " Could not retrieve LHCb::HltDecReports from " + m_decRepLoc, "",
   //                            StatusCode::FAILURE);
   //    }
   //    const LHCb::HltDecReports* decReps = static_cast<const LHCb::HltDecReports*>(obj);
   //    m_tck = decReps->configuredTCK();
   // }

   MsgStream msg(msgSvc(), name());

   size_t bin = 0;
   size_t chunkSize{boost::numeric_cast<size_t>(m_updateInterval)};

   // Create a new chunk if we don't have one for this identifier
   auto it = m_chunks.find(key);
   if (it == end(m_chunks)) {
      it = m_chunks.emplace(key, Chunk{m_run, m_tck, key.__hash__(),
                                       0, chunkSize}).first;
      bin = boost::numeric_cast<size_t>(diff);
   }

   // If we get an early event, OR in case of an event outside of the chunk, send the old
   // chunk and create a new one.
   if ((diff < it->second.start)
       || (boost::numeric_cast<size_t>(t - it->second.start) > it->second.data.size())) {
      sendChunk(it->second);
      auto start = boost::numeric_cast<size_t>(diff);
      if (start > m_chunkOverlap) {
         start -= m_chunkOverlap;
      }
      it->second = Chunk{m_run, m_tck, it->second.histId, start, chunkSize};
      bin = boost::numeric_cast<size_t>(diff - start);
   }

   // Count
   it->second.data[bin] += 1;
}

//===============================================================================
RateCounter& HltMonitorSvc::rateCounter(const std::string& identifier) const
{
   Gaudi::StringKey key{identifier};
   auto it = m_counters.find(key);
   if (it == m_counters.end()) {
      auto rc = new RateCounter(const_cast<HltMonitorSvc* const>(this), key);
      auto r = m_counters.emplace(key, rc);
      return *(r.first->second);
   } else {
      return *(it->second);
   }
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
void HltMonitorSvc::handle(const Incident& incident)
{
   MsgStream msg(msgSvc(), name());


   const RunChangeIncident* rci = dynamic_cast<const RunChangeIncident*>(&incident);
   if (rci) {
      if (m_run != rci->runNumber()) {
         msg << MSG::DEBUG << "Change of run number detected " << m_run
             << " -> " << rci->runNumber() << endmsg;
      }
      m_run = rci->runNumber();

      // On a run change, always send the chunks
      for (auto& entry : m_chunks) {
         sendChunk(entry.second);
         size_t size{boost::numeric_cast<size_t>(m_updateInterval + m_chunkOverlap)};
         entry.second = Chunk{m_run, m_tck, entry.second.histId, 0, size};
      }
   }
}

//===============================================================================
void HltMonitorSvc::sendChunk(const Chunk& chunk)
{
   std::stringstream ss;
   boost::archive::text_oarchive oa{ss};
   oa << chunk;
   auto s = ss.str();

   // Prepare message and send
   zmq::message_t msg(s.size());
   memcpy(static_cast<void*>(msg.data()), s.c_str(), s.size());
   m_output->send(msg);
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
