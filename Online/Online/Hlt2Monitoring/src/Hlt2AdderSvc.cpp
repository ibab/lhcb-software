// Include files
#include <thread>

// boost
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

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

//-----------------------------------------------------------------------------
// Implementation file for class : Hlt2AdderSvc
//
// 2015-06-13 : Roel Aaij
//-----------------------------------------------------------------------------

// Factory for instantiation of service objects
DECLARE_SERVICE_FACTORY(Hlt2AdderSvc)

//=============================================================================
Hlt2AdderSvc::Hlt2AdderSvc(const string& name, ISvcLocator* loc)
   : Hlt2MonBaseSvc(name, loc)
{
   declareProperty("SendInterval", m_sendInterval = 10.);
}

//=============================================================================
Hlt2AdderSvc::~Hlt2AdderSvc()
{

}

//===============================================================================
StatusCode Hlt2AdderSvc::initialize()
{
   StatusCode sc = Hlt2MonBaseSvc::initialize();
   if (!sc.isSuccess()) return sc;

   if ((m_frontCon.empty() || m_backCon.empty())) {
      warning() << "Connections not correctly configured, "
                << "Hlt2 adder disabled" << endmsg;
      m_enabled = false;
   }
   return sc;
}

//===============================================================================
void Hlt2AdderSvc::function()
{
   zmq::socket_t front{context(), ZMQ_SUB};
   front.connect(m_frontCon.c_str());
   front.setsockopt(ZMQ_SUBSCRIBE, "", 0);
   info() << "Connected frontend to: " << m_frontCon << endmsg;

   zmq::socket_t back{context(), ZMQ_PUB};
   back.bind(m_backCon.c_str());
   info() << "Bound backend to: " << m_backCon << endmsg;

   // WORKING: control socket and zmq_poll to stop. Check restart logic.
   zmq::socket_t control{context(), ZMQ_SUB};
   control.connect(ctrlCon().c_str());
   control.setsockopt(ZMQ_SUBSCRIBE, "", 0);

   //  Initialize poll set
   zmq::pollitem_t items [] = {
      { control, 0, ZMQ_POLLIN, 0 },
      { front, 0, ZMQ_POLLIN, 0 }
   };

   bool paused = false;

   while (true) {
      //  Process messages from both sockets
      zmq::message_t message;

      if (!paused) zmq::poll (&items [0], 2, -1);

      if (paused || (items[0].revents & ZMQ_POLLIN)) {
         control.recv(&message);
         if (message.size() == 9 && memcmp(message.data(), "TERMINATE", 9) == 0) {
            int linger = 0;
            front.setsockopt(ZMQ_LINGER, &linger, sizeof(linger));
            back.setsockopt(ZMQ_LINGER, &linger,sizeof(linger));
            control.setsockopt(ZMQ_LINGER, &linger,sizeof(linger));
            break;
         } else if (message.size() == 5 && memcmp(message.data(), "PAUSE", 5) == 0) {
            paused = true;
         } else if (message.size() == 6 && memcmp(message.data(), "RESUME", 6) == 0) {
            paused = false;
         }
      }
      if (!paused && (items[1].revents & ZMQ_POLLIN)) {
         front.recv(&message);

         // Deserialize
         std::stringstream ss{static_cast<char*>(message.data())};
         boost::archive::text_iarchive ia{ss};
         Monitoring::Chunk c;
         ia >> c;

         key_t key{c.runNumber, c.histId};
         auto now = std::chrono::high_resolution_clock::now();
         auto it = m_histograms.find(key);
         // Add to internal store
         if (it == end(m_histograms)) {
            it = m_histograms.emplace(key, Monitoring::Histogram{c.runNumber, c.tck, c.histId}).first;
            m_updates[key] = now;
         }
         it->second.addChunk(c);

         // If not sent for interval seconds, send all histograms
         if (std::chrono::duration_cast<std::chrono::seconds>(now - m_updates[key]).count() >
             m_sendInterval) {

            // Serialize
            std::stringstream ss;
            boost::archive::text_oarchive oa{ss};
            oa << it->second;
            auto s = ss.str();

            // Send message
            zmq::message_t msg(s.size());
            memcpy(static_cast<void*>(msg.data()), s.c_str(), s.size());
            back.send(msg);
            m_updates[key] = now;

            // Zero out histogram
            it->second.data = std::vector<Monitoring::BinContent>(it->second.data.size(), 0);
         }
      }
   }
}
