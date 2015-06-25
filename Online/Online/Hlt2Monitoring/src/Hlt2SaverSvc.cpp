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
#include "Hlt2SaverSvc.h"

namespace {

   using std::string;
   using std::to_string;
   using std::vector;
   using Monitoring::Chunk;
   using Monitoring::Histogram;

}

//-----------------------------------------------------------------------------
// Implementation file for class : Hlt2SaverSvc
//
// 2015-06-13 : Roel Aaij
//-----------------------------------------------------------------------------

// Factory for instantiation of service objects
DECLARE_SERVICE_FACTORY(Hlt2SaverSvc)

//=============================================================================
Hlt2SaverSvc::Hlt2SaverSvc(const string& name, ISvcLocator* loc)
   : Hlt2MonBaseSvc(name, loc)
{
   declareProperty("DataConnection", m_dataCon);
   declareProperty("InfoConnection", m_infoCon);
   declareProperty("Directory", m_directory);
   declareProperty("SaveInterval", m_saveInterval = 60.);
   declareProperty("RateStart", m_saveInterval = 0.);
   declareProperty("RunDuration", m_runDuration = 3600.);
   declareProperty("RateInterval", m_rateInterval = 20.);
}

//=============================================================================
Hlt2SaverSvc::~Hlt2SaverSvc()
{

}

//===============================================================================
StatusCode Hlt2SaverSvc::initialize()
{
   StatusCode sc = Hlt2MonBaseSvc::initialize();
   if (!sc.isSuccess()) return sc;

   if (m_dataCon.empty() || m_infoCon.empty()) {
      warning() << "Connections not correctly configured, "
                << "Hlt2 saver disabled" << endmsg;
      m_enabled = false;
   }
   return sc;
}

//===============================================================================
void Hlt2SaverSvc::function()
{
   zmq::socket_t data{context(), ZMQ_SUB};
   data.connect(m_dataCon.c_str());
   data.setsockopt(ZMQ_SUBSCRIBE, "", 0);
   info() << "Connected data socket to: " << m_dataCon << endmsg;

   zmq::socket_t inf{context(), ZMQ_PUB};
   inf.connect(m_infoCon.c_str());
   int timeo = 1000;
   inf.setsockopt(ZMQ_RCVTIMEO, &timeo, sizeof(timeo));
   info() << "Bound backend to: " << m_infoCon << endmsg;

   // WORKING: control socket and zmq_poll to stop. Check restart logic.
   zmq::socket_t control{context(), ZMQ_SUB};
   control.connect(ctrlCon().c_str());
   control.setsockopt(ZMQ_SUBSCRIBE, "", 0);

   //  Initialize poll set
   zmq::pollitem_t items [] = {
      { control, 0, ZMQ_POLLIN, 0 },
      { data, 0, ZMQ_POLLIN, 0 }
   };

   bool paused = false;
   auto lastUpdate = std::chrono::high_resolution_clock::now();

   while (true) {
      //  Process messages from both sockets
      zmq::message_t message;

      zmq::poll(&items[0], 2, -1);

      if (items[0].revents & ZMQ_POLLIN) {
         control.recv(&message);
         if (message.size() == 9 && memcmp(message.data(), "TERMINATE", 9) == 0) {
            int linger = 0;
            data.setsockopt(ZMQ_LINGER, &linger, sizeof(linger));
            inf.setsockopt(ZMQ_LINGER, &linger,sizeof(linger));
            control.setsockopt(ZMQ_LINGER, &linger,sizeof(linger));
            break;
         } else if (message.size() == 5 && memcmp(message.data(), "PAUSE", 5) == 0) {
            paused = true;
         } else if (message.size() == 6 && memcmp(message.data(), "RESUME", 6) == 0) {
            paused = false;
         }
      }
      if (!paused && (items[1].revents & ZMQ_POLLIN)) {
         // Receive data
         data.recv(&message);

         // Deserialize
         std::stringstream ss{static_cast<char*>(message.data())};
         boost::archive::text_iarchive ia{ss};
         Monitoring::Histogram histo;
         ia >> histo;

         histoKey_t key{histo.runNumber, histo.histId};
         if (!m_defs.count(key) && !m_rates.count(key)) {
            // We don't know about this histo,
            // request histo info from MonInfoSvc

            // send request
            zmq::message_t req{sizeof(Monitoring::RunNumber)};
            memcpy(req.data(), &key.first, sizeof(Monitoring::RunNumber));
            inf.send(req, ZMQ_SNDMORE);
            req.rebuild(sizeof(Monitoring::HistId));
            memcpy(req.data(), &key.second, sizeof(Monitoring::HistId));
            inf.send(req);

            // Wait for reply
            zmq::message_t rep;
            size_t err = 0;
            vector<string> reply;
            do {
               rep.rebuild();
               err = inf.recv(&rep);
               if (err) break;
               reply.push_back(string{static_cast<char*>(rep.data())});
            } while(rep.more());

            if (err || reply[0] != Monitoring::s_Known)
               continue;

            if (reply[1] == Monitoring::s_Rate) {
               m_rates[key] = reply[2];
               // TODO: Create ROOT rate histo from properties
            } else if (reply[1] == Monitoring::s_Histo1D) {
               std::stringstream is{reply[2]};
               boost::archive::text_iarchive ia{is};
               Gaudi::Histo1DDef def;
               ia >> def;
               m_defs[key] = def;
               // TODO: Create ROOT histo from def
            }
         }

         if (m_defs.count(key)) {
            // It's a regular histo that we know about
            // TODO: Add received histo to ROOT histo
         } else if (m_rates.count(key)) {
            // It's a rate histo that we know about
            // TODO: Add received histo to ROOT rate histo
         }
      }

      // TODO: Write ROOT histograms to file
      auto now = std::chrono::high_resolution_clock::now();
      if (std::chrono::duration_cast<std::chrono::seconds>(now - lastUpdate).count() >
          m_saveInterval) {

      }
   }
}
