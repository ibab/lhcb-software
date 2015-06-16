// Include files
#include <thread>

// boost
#include <boost/archive/text_iarchive.hpp>

// zeromq
#include "zmq/zmq.hpp"

// Gaudi
#include <GaudiKernel/IIncidentSvc.h>
#include <GaudiKernel/IAlgorithm.h>
#include <GaudiKernel/IAlgTool.h>
#include <GaudiKernel/SvcFactory.h>
#include <GaudiKernel/MsgStream.h>

// HLT
#include <Hlt2Monitoring/Chunk.h>
#include <Hlt2Monitoring/Histogram.h>
#include <Hlt2Monitoring/Serialize.h>

// local
#include "Hlt2AdderSvc.h"


namespace {

   using std::string;
   using std::to_string;
   using std::vector;
   using Monitoring::Chunk;
   using Monitoring::Histogram;

}

namespace Monitoring {
   /** Write a chunk of data to a std::ostream.
    */
   auto operator<<(std::ostream& os, const Chunk& chunk) -> std::ostream & {
      os << "Monitoring::Chunk [Run: " << chunk.runNumber << ", TCK: " << chunk.tck
         << ", HistId: " << chunk.histId << ", Start: " << chunk.start
         << "]\n                  [";
      std::copy(std::begin(chunk.data), std::end(chunk.data),
                std::ostream_iterator<BinContent>(os, " "));
      os << ']';
      return os;
   }

   /** Write histogram to a std::ostream.
    */
   auto operator<<(std::ostream& os, const Histogram& h) -> std::ostream& {
      os << "Monitoring::Histogram [Run: " << h.runNumber << ", TCK: " << h.tck
         << ", HistId: " << h.histId << "]";
      // For testing print something that depends on the data.
      os << "\n                      Integral = "
         << std::accumulate(std::begin(h.data), std::end(h.data), 0);
      return os;
   }
}


//-----------------------------------------------------------------------------
// Implementation file for class : Hlt2AdderSvc
//
// 2015-06-13 : Roel Aaij
//-----------------------------------------------------------------------------

// Factory for instantiation of service objects
DECLARE_SERVICE_FACTORY(Hlt2AdderSvc)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Hlt2AdderSvc::Hlt2AdderSvc(const string& name, ISvcLocator* loc)
   : base_class(name, loc),
   m_add{true},
   m_incidentSvc{nullptr},
   m_thread{nullptr},
   m_control{nullptr},
   m_context{nullptr}
{
   declareProperty("OutPort", m_outPort = 31338);
   declareProperty("FrontConnection", m_frontCon);
   declareProperty("BackConnection", m_backCon);
   declareProperty("PartitionName", m_partition);
   declareProperty("RunInPartitions", m_partitions = {"LHCb2"});

}
//=============================================================================
// Destructor
//=============================================================================
Hlt2AdderSvc::~Hlt2AdderSvc() {}

//=============================================================================

//===============================================================================
StatusCode Hlt2AdderSvc::initialize()
{
   StatusCode sc = Service::initialize();

   MsgStream msg(msgSvc(), name());
   msg << MSG::INFO << "Initialized ZeroMQ based Hlt2AdderSvc" << endmsg;
   sc = serviceLocator()->service("IncidentSvc", m_incidentSvc, true);
   if( !sc.isSuccess() ) {
      msg << MSG::FATAL << "IncidentSvc not found" << endmsg;
      return sc;
   }
   m_incidentSvc->addListener(this, "APP_RUNNING");
   m_incidentSvc->addListener(this, "APP_STOPPED");

   if (std::find(begin(m_partitions), end(m_partitions), m_partition) == end(m_partitions)) {
      MsgStream log(msgSvc(), name());
      log << MSG::INFO << "Running in partition: " << m_partition
          << ", not starting adder." << endmsg;
      m_add = false;
   } else if ((m_frontCon.empty() || m_backCon.empty())) {
      msg << MSG::WARNING << "Connections not correctly configured, "
             << "Hlt2 adder disabled" << endmsg;
      m_add = false;
   }

   return sc;
}

//===============================================================================
void Hlt2AdderSvc::handle(const Incident& /* incident */)
{

}

//===============================================================================
StatusCode Hlt2AdderSvc::start()
{
   auto sc = Service::start();
   if (!sc.isSuccess()) return sc;

   if (!m_add) {
      return sc;
   } else if (m_thread) {
      zmq_send(*m_control, "RESUME", 6, 0);
      return sc;
   }

   if (!m_context) {
      m_context = new zmq::context_t{1};
   }

   m_thread = new std::thread([this](void) -> void {
         MsgStream log(msgSvc(), name());

         zmq::socket_t front{*m_context, ZMQ_SUB};
         front.connect(m_frontCon.c_str());
         front.setsockopt(ZMQ_SUBSCRIBE, "", 0);
         log << MSG::INFO << "Connected frontend to: " << m_frontCon << endmsg;

         zmq::socket_t back{*m_context, ZMQ_PUB};
         back.bind(m_backCon.c_str());
         log << MSG::INFO << "Bound backend to: " << m_backCon << endmsg;

         // WORKING: control socket and zmq_poll to stop. Check restart logic.
         zmq::socket_t control{*m_context, ZMQ_PAIR};
         control.connect("inproc://control");

         //  Initialize poll set
         zmq::pollitem_t items [] = {
            { front, 0, ZMQ_POLLIN, 0 },
            { control, 0, ZMQ_POLLIN, 0 }
         };

         auto start = std::chrono::high_resolution_clock::now();

         int interval{10};
         bool paused = false;

         while (true) {
            //  Process messages from both sockets
            zmq::message_t message;

            if (!paused) zmq::poll (&items [0], 2, -1);

            if (paused || (items[1].revents & ZMQ_POLLIN)) {
               control.recv(&message);
               std::string command{static_cast<char*>(message.data())};
               if (command == "TERMINATE") {
                  break;
               } else if (command == "PAUSE") {
                  front.close();
                  paused = true;
               } else if (paused && command == "RESUME") {
                  front.connect(m_frontCon.c_str());
                  front.setsockopt(ZMQ_SUBSCRIBE, "", 0);
                  paused = false;
               }
            }
            if (!paused && (items[0].revents & ZMQ_POLLIN)) {
               front.recv(&message);

               // Deserialize
               std::stringstream ss{static_cast<char*>(message.data())};
               boost::archive::text_iarchive ia{ss};
               Monitoring::Chunk c;
               ia >> c;

               // Add to internal store
               if (!m_histograms.count(c.histId)) {
                  m_histograms[c.histId] =
                     Monitoring::Histogram{c.runNumber, c.tck, c.histId};
               }
               m_histograms[c.histId].addChunk(c);

               // If not displayed for interval seconds, show all current histograms
               auto now = std::chrono::high_resolution_clock::now();
               if (std::chrono::duration_cast<std::chrono::seconds>(now - start).count() >
                   interval) {
                  for (const auto& h : m_histograms) {
                     std::cout << h.second << std::endl;
                  }
                  start = now;
               }
            }
         }
      });

   m_control = new zmq::socket_t{*m_context, ZMQ_PAIR};
   m_control->bind("inproc://control");

   MsgStream msg(msgSvc(), name());
   msg << MSG::INFO << "Hlt2AdderSvc started." << endmsg;

   return sc;
}


//===============================================================================
StatusCode Hlt2AdderSvc::stop()
{
   if (m_control) {
      zmq_send(*m_control, "PAUSE", 5, 0);
   }
   return StatusCode::SUCCESS;
}

//===============================================================================
StatusCode Hlt2AdderSvc::finalize()
{
   MsgStream msg(msgSvc(), name());

   // terminate the proxy
   if (m_thread) {
      zmq_send(*m_control, "TERMINATE", 9, 0);
      m_thread->join();
      delete m_thread;
      m_thread = nullptr;
      delete m_control;
      m_control = nullptr;
   }
   delete m_context;
   m_context = nullptr;

   if (m_incidentSvc) {
      m_incidentSvc->removeListener(this);
      m_incidentSvc->release();
      m_incidentSvc = 0;
   }
   return Service::finalize();
}



int main() {
}
