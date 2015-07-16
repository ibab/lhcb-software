// Include files
#include <memory>
#include <thread>
#include <chrono>
#include <ctime>
#include <set>

// boost
#include <boost/archive/text_iarchive.hpp>
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
#include <TF1.h>
#include <TFile.h>
#include <TClass.h>
#include <TBufferFile.h>

// local
#include "Hlt2SaverSvc.h"

namespace {
   using std::string;
   using std::to_string;
   using std::vector;
   using std::array;
   using std::pair;

   using Monitoring::Chunk;
   using Monitoring::Histogram;
   using boost::lexical_cast;
   namespace ba = boost::algorithm;
   namespace fs = boost::filesystem;

   const std::string s_directory = "/hist/Savesets";
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
   declareProperty("BaseDirectory", m_directory);
   declareProperty("SaveInterval", m_saveInterval = 60.);
   declareProperty("NormaliseRateTo", m_normalize = "Hlt2RoutingBitsWriter/RoutingBit33");
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

   if (m_fileName.empty()) {
      m_fileName = "Hlt2Saver-%d-%s%s%sT%s%s%s.root";
   }

   if (m_dataCon.empty()) {
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
         auto cmd = receiveString(control);
         if (cmd == "TERMINATE") {
            int linger = 0;
            data.setsockopt(ZMQ_LINGER, &linger, sizeof(linger));
            control.setsockopt(ZMQ_LINGER, &linger,sizeof(linger));
            break;
         } else if (cmd == "PAUSE") {
            paused = true;
         } else if (cmd == "RESUME") {
            paused = false;
         }
      }
      if (!paused && (items[1].revents & ZMQ_POLLIN)) {
         // Receive data
         auto key = receiveRunAndId(data);

         // Start time
         int start{0};
         message.rebuild(sizeof(start));
         data.recv(&message);
         memcpy(&start, message.data(), sizeof(start));
         m_startTimes[key.first] = start;

         // Receive type
         auto type = receiveString(data);

         // Receive directory
         auto dir = receiveString(data);

         // Receive ROOT histogram
         message.rebuild();
         data.recv(&message);

         // ROOT uses char buffers, ZeroMQ uses size_t
         auto factor = sizeof( size_t ) / sizeof( char );

         // Create a buffer and insert the message
         TBufferFile buffer(TBuffer::kRead);
         buffer.SetBuffer(message.data(), factor * message.size(), kFALSE);

         // Grab the object from the buffer
         auto cl = TClass::GetClass(typeid(TH1D));
         std::unique_ptr<TH1D> histo{static_cast<TH1D*>(buffer.ReadObject(cl))};
         histo->SetDirectory(nullptr);

         debug() << "Received ROOT histogram " << key.first << " " << key.second
                 << " " << dir << " " << histo->GetName() << endmsg;

         auto it = m_histos.get<byKey>().find(key);
         if (it == end(m_histos.get<byKey>())) {
            m_histos.insert({key, type, dir, histo.release()});
         } else {
            it->histo->Add(histo.get());
         }
      }

      // Write ROOT histograms to file
      auto now = std::chrono::high_resolution_clock::now();
      if (std::chrono::duration_cast<std::chrono::seconds>(now - lastUpdate).count() >
          m_saveInterval) {
         saveHistograms();
         lastUpdate = now;
      }
   }
}

//===============================================================================
void Hlt2SaverSvc::saveHistograms() const
{
   // Store histograms per run and all runs.
   std::set<Monitoring::RunNumber> runs;
   using normalizers_t = std::unordered_map<Monitoring::RunNumber, pair<string, TH1D*> >;
   normalizers_t normalizers;

   for (const auto& entry : m_histos) {
      runs.insert(entry.run());

      auto histo = entry.histo.get();
      auto dir = entry.dir;
      // If this is a rate histogram, normalize it.
      if ((entry.type == Monitoring::s_Rate) && (m_normalize == (dir + "/" + histo->GetName()))) {
         normalizers.emplace(entry.run(), make_pair(dir, histo));
      }
   }

   // Loop over runs
   for (const auto run : runs) {
      auto ti = timeInfo(run);
      fs::path directory;
      if (m_directory.empty()) {
         directory = fs::path{s_directory} / ti[0] / m_partition / "Moore2Saver" / ti[1] / ti[2];
      } else {
         directory = fs::path{m_directory};
      }
      if (!fs::exists(directory)) {
         if (!fs::create_directories(directory)) {
            warning() << "Failed to create directory " << directory
                      << ". Skipping run " << run << "." << endmsg;
            continue;
         }
      }
      auto filename = boost::format(m_fileName) % run % ti[0] % ti[1] % ti[2] % ti[3] % ti[4] % ti[5];

      auto file = fs::path(directory) / fs::path(filename.str());
      if (fs::exists(file)) {
         debug() << "Updating histograms for run " << run << " in " << file.string() << endmsg;
      } else {
         debug() << "Saving histograms for run " << run << " to " << file.string() << endmsg;
      }

      std::unique_ptr<TFile> inFile;
      if (fs::exists(file)) {
         inFile = std::unique_ptr<TFile>{new TFile(file.string().c_str(), "read")};
      }
      TFile outFile("/tmp/Hlt2Saver.root", "new");

      // Get the normalization histogram, including the already saved bit.
      auto normIt = normalizers.find(run);
      TH1D* norm{nullptr};
      std::unique_ptr<TH1D> totalNorm;
      if (normIt != end(normalizers)) {
         norm = normIt->second.second;
         auto path = normIt->second.first + "/" + norm->GetName();
         std::unique_ptr<TH1D> saved;
         if (inFile) {
            saved = std::unique_ptr<TH1D>{static_cast<TH1D*>(inFile->Get(path.c_str()))};
            if (saved) {
               saved->SetDirectory(nullptr);
               totalNorm = std::unique_ptr<TH1D>{static_cast<TH1D*>(saved->Clone())};
               totalNorm->Add(norm);
            }
         } else {
            totalNorm = std::unique_ptr<TH1D>{static_cast<TH1D*>(norm->Clone())};
         }
         totalNorm->SetDirectory(nullptr);
      }

      // Scale to the full lumi rate
      auto runInfo = getRunInfo(run);
      if (totalNorm.get() && runInfo) {
         double scale = 1000 * std::accumulate(begin(runInfo->lumiPars), end(runInfo->lumiPars), 0.);
         totalNorm->Scale(1. / scale);
      } else {
         totalNorm.reset();
      }

      // Loop over histograms for that run
      for (const auto& entry : boost::make_iterator_range(m_histos.get<byRun>().equal_range(run))) {
         auto histo = entry.histo.get();
         auto dir = entry.dir;
         auto inDir = static_cast<TDirectoryFile*>(inFile->Get(dir.c_str()));
         outFile.mkdir(dir.c_str());
         auto outDir = static_cast<TDirectoryFile*>(outFile.Get(dir.c_str()));
         if (!outDir) {
            warning() << "Could not create directory in SaveSet file "
                      << filename << " " << dir << endmsg;
            continue;
         }

         std::unique_ptr<TH1D> outHisto;
         if (inDir) {
            outHisto = std::unique_ptr<TH1D>{static_cast<TH1D*>(inDir->Get(histo->GetName()))};
            if (outHisto) outHisto->SetDirectory(nullptr);
         }

         if (!outHisto) {
            // No previous histogram present
            outHisto = std::unique_ptr<TH1D>{static_cast<TH1D*>(histo->Clone())};
            outHisto->SetDirectory(nullptr);
         } else {
            // Previously saved histogram present
            outHisto->Add(histo);
         }

         // Normalize
         if (totalNorm.get() && entry.type == Monitoring::s_Rate) {
            auto normName = string{outHisto->GetName()} + "_Rate";
            auto clone = static_cast<TH1D*>(outHisto->Clone(normName.c_str()));
            std::unique_ptr<TH1D> normHisto{clone};
            // Normalize to our normalization histogram
            normHisto->Divide(totalNorm.get());
            // Write normalized histogram to file
            outDir->WriteTObject(normHisto.get(), normName.c_str(), "overwrite");
            debug() << "Saved " << entry.dir << "/" << normName << endmsg;
         }

         // Write normal histogram to file
         outDir->WriteTObject(outHisto.get(), outHisto->GetName());
         debug() << "Saved " << entry.dir << "/" << outHisto->GetName() << endmsg;

         // Reset all hisograms that are not the normalization histogram
         if (histo != norm) {
            histo->Reset("ICESM");
         }
      }

      // Close input and output files
      if (inFile) {
         inFile->Close();
         inFile.reset();
      }
      outFile.Close();

      // Rename temporary file to output file
      auto outPath = fs::path{outFile.GetName()};
      fs::remove(file);
      fs::copy_file(outPath, file);
      fs::remove(outPath);

      // All other histograms for this run are saved, reset the normalization histogram.
      if (norm) {
         norm->Reset("ICESM");
      }
   }
}

//===============================================================================
std::array<std::string, 6> Hlt2SaverSvc::timeInfo(unsigned int run) const
{
   auto it = m_startTimes.find(run);
   if (it == end(m_startTimes)) {
      return {"", "", "", "", "", ""};
   }
   using clock_t = std::chrono::high_resolution_clock;
   using tp_t = std::chrono::time_point<clock_t>;
   const clock_t::duration startTime = std::chrono::seconds(it->second);
   std::time_t t = clock_t::to_time_t(tp_t{startTime});
   std::tm* start = gmtime(&t);

   auto fmt = [](int t)->string { return (boost::format("%02d") % t).str(); };
   return {to_string(start->tm_year + 1900),
           fmt(start->tm_mon + 1), fmt(start->tm_mday),
           fmt(start->tm_hour), fmt(start->tm_min), fmt(start->tm_sec)};
}

//===============================================================================
boost::optional<Monitoring::RunInfo>
Hlt2SaverSvc::getRunInfo(Monitoring::RunNumber run) const
{
   boost::optional<Monitoring::RunInfo> r;
   auto it = m_runInfo.find(run);
   if (it != end(m_runInfo)) {
      r = it->second;
   } else {
      zmq::socket_t inf{context(), ZMQ_REQ};
      inf.connect(m_infoCon.c_str());
      int timeo = 1000;
      inf.setsockopt(ZMQ_RCVTIMEO, &timeo, sizeof(timeo));

      // Send request for info
      sendString(inf, Monitoring::s_RunInfo, ZMQ_SNDMORE);
      sendMessage(inf, run);

      vector<string> reply;
      bool more = true;
      while (more) {
         reply.push_back(receiveString(inf, &more));
      }
      if (reply[0] == Monitoring::s_Known) {
         Monitoring::RunInfo runInfo;
         std::stringstream ss{reply[1]};
         {
            boost::archive::text_iarchive ia{ss};
            ia >> runInfo;
         }
         m_runInfo.emplace(run, runInfo);
         r = runInfo;
      }

      int linger = 0;
      inf.setsockopt(ZMQ_LINGER, &linger, sizeof(linger));
   }
   return r;
}
