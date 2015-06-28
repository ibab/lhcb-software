// Include files
#include <thread>
#include <set>

// boost
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/algorithm/string/find.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/numeric/conversion/cast.hpp>
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

// local
#include "Hlt2SaverSvc.h"

namespace {

   using std::string;
   using std::to_string;
   using std::vector;
   using Monitoring::Chunk;
   using Monitoring::Histogram;
   namespace ba = boost::algorithm;
   namespace fs = boost::filesystem;
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
   declareProperty("Filename", m_fileName);
   declareProperty("SaveInterval", m_saveInterval = 60.);
   declareProperty("RateStart", m_saveInterval = 0.);
   declareProperty("RunDuration", m_runDuration = 3600.);
   declareProperty("RateInterval", m_rateInterval = 20.);
}

//=============================================================================
Hlt2SaverSvc::~Hlt2SaverSvc()
{
   // Delete histograms that were written
   for (const auto& entry : m_histos) {
      delete entry.second.second;
   }
}

//===============================================================================
StatusCode Hlt2SaverSvc::initialize()
{
   StatusCode sc = Hlt2MonBaseSvc::initialize();
   if (!sc.isSuccess()) return sc;

   if (m_fileName.empty()) {
      m_fileName = "Hlt2Saver-%d.root";
   }

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

   // Control socket
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

            // If we get an error or the histogram is not known, continue.
            if (err || reply[0] != Monitoring::s_Known)
               continue;

            auto addHisto = [this](histoKey_t key, Gaudi::Histo1DDef def) {
               auto result = ba::find_last(def.title(), "/");
               string dir{begin(def.title()), begin(result)};
               string title{begin(result), end(def.title())};
               auto histo = new TH1D(title.c_str(), title.c_str(), def.bins(),
                                     def.lowEdge(), def.highEdge());
               histo->Sumw2();
               m_histos[key] = make_pair(dir, histo);
               return make_pair(dir, title);
            };

            if (reply[1] == Monitoring::s_Rate) {
               int nBins = boost::numeric_cast<int>(m_runDuration / m_rateInterval);
               m_rates[key] = addHisto(key, Gaudi::Histo1DDef(reply[2], nBins, m_rateStart,
                                                              m_rateStart + m_rateInterval * nBins));
            } else if (reply[1] == Monitoring::s_Histo1D) {
               // Deserialize Histo1DDef
               std::stringstream is{reply[2]};
               boost::archive::text_iarchive ia{is};
               Gaudi::Histo1DDef def;
               ia >> def;

               // Add histo and known def
               m_defs[key] = make_pair(addHisto(key, def).first, def);
            }
         }

         if (m_defs.count(key) || m_rates.count(key)) {
            // It's a regular or rate histo that we know about, add the
            auto rHisto = m_histos[key].second;
            for (size_t i = 0; i < histo.data.size(); ++i) {
               auto binContent = rHisto->GetBinContent(i);
               rHisto->SetBinContent(i, binContent + histo.data[i]);
            }
         }
      }

      // TODO: Write ROOT histograms to file
      auto now = std::chrono::high_resolution_clock::now();
      if (std::chrono::duration_cast<std::chrono::seconds>(now - lastUpdate).count() >
          m_saveInterval) {
         saveHistograms();
      }
   }
}

//===============================================================================
void Hlt2SaverSvc::saveHistograms() const
{
   // Store histograms per run and all runs.
   std::set<Monitoring::RunNumber> runs;
   boost::unordered_multimap<Monitoring::RunNumber, std::pair<std::string, TH1D*>> histosPerRun;
   for (const auto& entry : m_histos) {
      histosPerRun.emplace(make_pair(entry.first.first, entry.second));
      runs.insert(entry.first.first);
   }

   // Loop over runs
   for (const auto run : runs) {
      auto filename = boost::format(m_fileName) % run;
      auto file = fs::path(m_directory) / fs::path(filename.str());
      TFile rootFile(file.string().c_str(), (fs::exists(file) ? "UPDATE" : "NEW"));

      // Loop over histograms for that run
      for (const auto& entry : boost::make_iterator_range(histosPerRun.equal_range(run))) {
         auto histo = entry.second.second;
         auto dir = entry.second.first;
         auto rDir = static_cast<TDirectoryFile*>(rootFile.Get(dir.c_str()));
         if (!rDir) {
            rootFile.mkdir(dir.c_str());
            rDir = static_cast<TDirectoryFile*>(rootFile.Get(dir.c_str()));
         }
         auto savedHisto = static_cast<TH1D*>(rDir->Get(histo->GetName()));
         if (!savedHisto) {
            // No previous histogram present
            savedHisto = histo;
         } else {
            // Previously saved histogram present
            savedHisto->Add(histo);
         }
         // Write histogram to file
         rDir->WriteTObject(savedHisto, savedHisto->GetName(), "overwrite");
      }
   }

   // Delete histograms that were written
   for (const auto& entry : m_histos) {
      delete entry.second.second;
   }
   // Clear store
   m_histos.clear();
}
