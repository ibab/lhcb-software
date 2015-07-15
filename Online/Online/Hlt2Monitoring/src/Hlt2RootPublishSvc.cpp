// Include files
#include <memory>
#include <thread>
#include <chrono>
#include <ctime>
#include <set>

// boost
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/algorithm/string/find.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>

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

// ROOT
#include <TH1D.h>
#include <TFile.h>
#include <TClass.h>
#include <TBufferFile.h>

// local
#include "Hlt2RootPublishSvc.h"

namespace {
   using std::string;
   using std::to_string;
   using std::vector;
   using std::array;
   using Monitoring::Chunk;
   using Monitoring::Histogram;
   using boost::lexical_cast;
   namespace ba = boost::algorithm;
   namespace fs = boost::filesystem;

   const std::string s_directory = "/hist/Savesets";
}

//-----------------------------------------------------------------------------
// Implementation file for class : Hlt2RootPublishSvc
//
// 2015-06-13 : Roel Aaij
//-----------------------------------------------------------------------------

// Factory for instantiation of service objects
DECLARE_SERVICE_FACTORY(Hlt2RootPublishSvc)

//=============================================================================
Hlt2RootPublishSvc::Hlt2RootPublishSvc(const string& name, ISvcLocator* loc)
 : Hlt2MonBaseSvc(name, loc)
{
   declareProperty("InfoConnection", m_infoCon);
   declareProperty("PublishInterval", m_publishInterval = 60.);
   declareProperty("RateStart", m_rateStart = 0.);
   declareProperty("RunDuration", m_runDuration = 3600.);
   declareProperty("RateInterval", m_rateInterval = 20.);
}

//=============================================================================
Hlt2RootPublishSvc::~Hlt2RootPublishSvc()
{
   // Delete histograms that were written
   for (const auto& entry : m_histos) {
      delete entry.second.second;
   }
}

//===============================================================================
StatusCode Hlt2RootPublishSvc::initialize()
{
   StatusCode sc = Hlt2MonBaseSvc::initialize();
   if (!sc.isSuccess()) return sc;

   if (m_frontCon.empty() || m_backCon.empty() || m_infoCon.empty()) {
      warning() << "Connections not correctly configured, "
                << "Hlt2 ROOT publisher disabled" << endmsg;
      m_enabled = false;
   }

   return sc;
}

//===============================================================================
void Hlt2RootPublishSvc::function()
{
   zmq::socket_t front{context(), ZMQ_SUB};
   front.connect(m_frontCon.c_str());
   front.setsockopt(ZMQ_SUBSCRIBE, "", 0);
   info() << "Connected front socket to: " << m_frontCon << endmsg;

   zmq::socket_t inf{context(), ZMQ_REQ};
   inf.connect(m_infoCon.c_str());
   int timeo = 1000;
   inf.setsockopt(ZMQ_RCVTIMEO, &timeo, sizeof(timeo));
   info() << "Connected info socket to: " << m_infoCon << endmsg;

   zmq::socket_t back{context(), ZMQ_PUB};
   back.bind(m_backCon.c_str());
   info() << "Bound back socket to: " << m_backCon << endmsg;

   // Control socket
   zmq::socket_t control{context(), ZMQ_SUB};
   control.connect(ctrlCon().c_str());
   control.setsockopt(ZMQ_SUBSCRIBE, "", 0);

   //  Initialize poll set
   zmq::pollitem_t items [] = {
      { control, 0, ZMQ_POLLIN, 0 },
      { front, 0, ZMQ_POLLIN, 0 }
   };

   bool paused = false;
   auto lastUpdate = std::chrono::high_resolution_clock::now();

   while (true) {
      //  Process messages from both sockets
      zmq::message_t message;

      zmq::poll(&items[0], 2, -1);

      if (items[0].revents & ZMQ_POLLIN) {
         auto cmd = receiveString(control);
         if (cmd == "TERMINATE") {
            int linger = 0;
            front.setsockopt(ZMQ_LINGER, &linger, sizeof(linger));
            inf.setsockopt(ZMQ_LINGER, &linger,sizeof(linger));
            control.setsockopt(ZMQ_LINGER, &linger,sizeof(linger));
            break;
         } else if (cmd == "PAUSE") {
            paused = true;
         } else if (cmd == "RESUME") {
            paused = false;
         }
      }
      if (!paused && (items[1].revents & ZMQ_POLLIN)) {
         // Deserialize
         Monitoring::Histogram histo;
         std::stringstream ss{receiveString(front)};
         {
            boost::archive::text_iarchive ia{ss};
            ia >> histo;
         }
         histoKey_t key{histo.runNumber, histo.histId};

         auto addHisto = [this](histoKey_t key, Gaudi::Histo1DDef def) {
            auto result = ba::find_last(def.title(), "/");
            string dir{begin(def.title()), begin(result)};
            string title{end(result), end(def.title())};
            auto histo = new TH1D(title.c_str(), title.c_str(), def.bins(),
                                  def.lowEdge(), def.highEdge());
            histo->Sumw2();
            m_histos.emplace(key, make_pair(dir, histo));
            return make_pair(dir, title);
         };

         if (!m_defs.count(key) && !m_rates.count(key)) {
            // We don't know about this histo,
            // request histo info from MonInfoSvc

            // send request
            sendString(inf, Monitoring::s_HistoInfo, ZMQ_SNDMORE);
            sendMessage(inf, key.first, ZMQ_SNDMORE);
            sendMessage(inf, key.second);

            // Wait for reply
            size_t err = 0;
            vector<string> reply;
            bool more = true;
            while (more) {
               reply.push_back(receiveString(inf, &more));
            }

            // If we get an error or the histogram is not known, continue.
            if (err || reply[0] != Monitoring::s_Known) {
               debug() << "Histo not known: " << key.first << " " << key.second << endmsg;
               continue;
            }

            m_startTimes[key.first] = boost::lexical_cast<int>(reply[1]);

            if (reply[2] == Monitoring::s_Rate) {
               int nBins = boost::numeric_cast<int>(m_runDuration / m_rateInterval);
               auto r = addHisto(key, Gaudi::Histo1DDef(reply[3], m_rateStart,
                                                        m_rateStart + m_rateInterval * nBins, nBins));
               m_rates[key] = r;
               debug() << "Added Rate histo: " << r.first << " " << r.second << " "
                       << m_startTimes[key.first] << endmsg;
            } else if (reply[2] == Monitoring::s_Histo1D) {
               // Deserialize Histo1DDef
               Gaudi::Histo1DDef def;
               std::stringstream is{reply[3]};
               {
                  boost::archive::text_iarchive ia{is};
                  ia >> def;
               }

               // Add histo and known def
               auto r = addHisto(key, def);
               m_defs[key] = make_pair(r.first, def);
               debug() << "Added 1D histo: " << r.first << " " << r.second << " "
                       << m_startTimes[key.first] << endmsg;
            }
         }

         if (m_defs.count(key) || m_rates.count(key)) {
            // It's a regular or rate histo that we know about, add the
            auto it = m_histos.find(key);
            if (it == end(m_histos)) {
               warning() << "Should know about histogram " << key.first << " " << key.second
                         << ", but no corresponding ROOT histogram can be found: ignoring it."
                         << endmsg;
            } else {
               TH1D* rHisto = it->second.second;
               for (size_t i = 0; i < histo.data.size(); ++i) {
                  auto binContent = rHisto->GetBinContent(i);
                  rHisto->SetBinContent(i, binContent + histo.data[i]);
               }
            }
         }
      }

      // Publish histograms
      auto now = std::chrono::high_resolution_clock::now();
      if (std::chrono::duration_cast<std::chrono::seconds>(now - lastUpdate).count() >
          m_publishInterval) {
         publishHistograms(back);
      }
   }
}

//===============================================================================
void Hlt2RootPublishSvc::publishHistograms(zmq::socket_t& socket) const
{
   auto deleteBuffer = []( void* data, void* /* hint */ ) -> void {
      delete [] (char*)data;
   };

   // Loop over histograms
   for (const auto& entry : m_histos) {
      auto histo = entry.second.second;

      // Skip empty histograms
      if (!histo->GetEntries()) continue;

      auto dir = entry.second.first;
      debug() << "Publishing " << entry.first.first << " " << entry.first.second
              << " " << entry.second.first << " " << histo->GetName() << endmsg;

      Monitoring::RunNumber run = entry.first.first;
      sendMessage(socket, run, ZMQ_SNDMORE);
      sendMessage(socket, entry.first.second, ZMQ_SNDMORE);
      auto it = m_startTimes.find(run);
      int startTime{0};
      if (it != end(m_startTimes)) {
         startTime = it->second;
      } else {
         warning() << "Cannot find start time for run " << run << endmsg;
      }
      sendMessage(socket, startTime, ZMQ_SNDMORE);

      string type;
      if (m_rates.count(entry.first)) {
         type = Monitoring::s_Rate;
      } else if (m_defs.count(entry.first)) {
         type = Monitoring::s_Histo1D;
      } else {
         warning() << "Histogram " << run << " " << entry.first.second
                 << " is of unknown type." << endmsg;
         continue;
      }
      sendString(socket, type, ZMQ_SNDMORE);
      sendString(socket, dir, ZMQ_SNDMORE);

      // Serialize histogram
      TBufferFile buffer(TBuffer::kWrite);
      buffer.WriteObject(histo);

      // Create message and detach buffer
      zmq::message_t message(buffer.Buffer(), buffer.Length(), deleteBuffer);
      buffer.DetachBuffer();

      // Send message
      socket.send(message);

      // Reset histos to limit amount sent over the network.
      histo->Reset("ICESM");
   }
}
