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
#include <TKey.h>
#include <TH1D.h>
#include <TFile.h>
#include <TClass.h>
#include <TBufferFile.h>

// local
#include "Hlt2SaverSvc.h"
#include "Utilities.h"

namespace {
   using std::string;
   using std::to_string;
   using std::vector;
   using std::array;
   using std::pair;
   using std::make_pair;

   using Monitoring::Chunk;
   using Monitoring::Histogram;
   using Monitoring::entry_t;
   using Monitoring::histos_t;
   using Monitoring::byName;
   using Monitoring::byRun;

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
 : Hlt2MonBaseSvc(name, loc),
   m_stopSaving{false}
{
   declareProperty("DataConnection", m_dataCon);
   declareProperty("InfoConnection", m_infoCon);
   declareProperty("BaseDirectory", m_directory);
   declareProperty("SaveInterval", m_saveInterval = 60);
   declareProperty("NormalizeRateTo", m_normalize = "Hlt2RoutingBitsWriter/RoutingBit33");
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
      m_fileName = "Moore2Saver-%d-%s%s%sT%s%s%s.root";
   }

   if (m_runFileName.empty()) {
      m_runFileName = "Moore2Saver-run%d.root";
   }

   if (m_dataCon.empty()) {
      warning() << "Connections not correctly configured, "
                << "Hlt2 saver disabled" << endmsg;
      disable();
   }
   return sc;
}

//===============================================================================
void Hlt2SaverSvc::save()
{
   zmq::socket_t control{context(), ZMQ_PUB};
   control.connect(ctrlCon().c_str());

   while(!m_stopSaving) {
      int n = 0;
      while(n < m_saveInterval) {
         if (m_stopSaving)
            break;
         std::chrono::seconds one{1};
         std::this_thread::sleep_for(one);
         ++n;
      }
      if (!m_stopSaving)
         sendString(control, "SAVE");
   }

   int linger = 0;
   control.setsockopt(ZMQ_LINGER, &linger,sizeof(linger));
}

//===============================================================================
void Hlt2SaverSvc::function()
{
   zmq::socket_t data{context(), ZMQ_SUB};
   data.connect(m_dataCon.c_str());
   data.setsockopt(ZMQ_SUBSCRIBE, "", 0);
   info() << "Connected data socket to: " << m_dataCon << endmsg;

   // Clean up queue
   zmq::message_t msg;
   while (data.recv(&msg, ZMQ_DONTWAIT)) {
      msg.rebuild();
   }

   // Control socket
   zmq::socket_t control{context(), ZMQ_SUB};
   control.bind(ctrlCon().c_str());
   control.setsockopt(ZMQ_SUBSCRIBE, "", 0);

   // Start thread to trigger saving of histograms
   std::thread saveThread([this]{save();});

   //  Initialize poll set
   zmq::pollitem_t items [] = {
      { control, 0, ZMQ_POLLIN, 0 },
      { data, 0, ZMQ_POLLIN, 0 }
   };

   bool paused = false;

   while (true) {
      //  Process messages from both sockets
      zmq::message_t message;

      zmq::poll(&items[0], 2, -1);

      if (items[0].revents & ZMQ_POLLIN) {
         auto cmd = receiveString(control);
         if (cmd == "TERMINATE") {
            pruneHistograms();
            saveHistograms();
            int linger = 0;
            data.setsockopt(ZMQ_LINGER, &linger, sizeof(linger));
            control.setsockopt(ZMQ_LINGER, &linger,sizeof(linger));
            break;
         } else if (cmd == "PAUSE") {
            debug() << name() << " paused." << endmsg;
            paused = true;
         } else if (cmd == "RESUME") {
            debug() << name() << " resumed." << endmsg;
            paused = false;
         } else if (!paused && cmd == "SAVE") {
            pruneHistograms();
            saveHistograms();
         }
      }
      if (!paused && (items[1].revents & ZMQ_POLLIN)) {
         // Receive data
         auto key = receiveRunAndId(data);

         // Receive type
         auto type = receiveString(data);

         // Receive directory
         auto dir = receiveString(data);

         // Receive ROOT histogram
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
         histo->SetTitle(type.c_str());

         debug() << "Received ROOT histogram " << key.first << " " << key.second
                 << " " << dir << " " << histo->GetName() << endmsg;

         // For newly created or updated histograms, update their lastUpdate member.
         auto now = std::chrono::high_resolution_clock::now();

         // Find the right histogram, byName finds histograms with that name for all runs.
         auto& hbn = m_histos.get<byName>();
         auto range = boost::make_iterator_range(hbn.equal_range(dir + "/" + histo->GetName()));

         // Find the one that belongs to the run we care about. The range should be very small.
         auto it = std::find_if(begin(range), end(range), [&key](const entry_t& entry) {
               return entry.run == key.first;
            });

         // Either create a new histogram, or update the existing one.
         if (it == end(range)) {
            m_histos.insert({key.first, type, dir, histo.release(), now});
         } else {
            hbn.modify(it, [&now](entry_t& entry){entry.lastUpdate = now;});
            it->histo->Add(histo.get());
         }
      }
   }

   m_stopSaving = true;
   saveThread.join();

}

//===============================================================================
void Hlt2SaverSvc::pruneHistograms()
{
   auto now = std::chrono::high_resolution_clock::now();

   std::vector<histos_t::iterator> toRemove;

   for (auto it = begin(m_histos), last = end(m_histos); it != last; ++it) {
      // Prune histograms for which we have not gotten an update in this save interval.
      // This is always the case for histograms that were loaded from the previous save set.
      auto diff = std::chrono::duration_cast<std::chrono::seconds>(now - it->lastUpdate).count();
      if (diff > 2 * m_saveInterval) {
         debug() << "Pruning " << it->name() << " with diff " << diff << endmsg;
         toRemove.push_back(it);
      }
   }

   for (auto it : toRemove) {
      m_histos.erase(it);
   }

}

//===============================================================================
void Hlt2SaverSvc::saveHistograms() const
{
   // Store histograms per run and all runs.
   std::set<Monitoring::RunNumber> runs;
   using normalizers_t = std::unordered_map<Monitoring::RunNumber, pair<string, TH1D*> >;
   normalizers_t normalizers;

   // Get list of runs for which we have updates
   for (const auto& entry : m_histos) {
      runs.insert(entry.run);
   }


   // Load saved histograms for runs where we have updates.
   std::set<Monitoring::RunNumber> skip;
   for (auto run : runs) {
      auto info = runInfo(run);


      if (info) {
         bool exists{false};
         fs::path file;
         std::tie(file, exists) = filename(run, false);
         if (exists) {
            Monitoring::loadSavedHistograms(m_histos, file, run, "_Rate");
         }
      } else {
         warning() << "No run info for run " << run << " not saving its histograms."
                   << endmsg;
         skip.insert(run);
      }
   }

   // Find rate normalization histograms
   for (const auto& entry : m_histos) {
      auto histo = entry.histo.get();
      auto dir = entry.dir;
      // If this is a rate histogram and a normalizer, save it.
      if ((entry.type == Monitoring::s_Rate) && (m_normalize == (dir + "/" + histo->GetName()))) {
         normalizers.emplace(entry.run, make_pair(dir, histo));
      }
   }

   // Write ROOT histograms to file
   // Loop over runs
   for (const auto run : runs) {
      if (skip.count(run)) {
         continue;
      }
      auto outPath = fs::unique_path("/tmp/HltSaver-%%%%-%%%%-%%%%-%%%%.root");
      TFile outFile(outPath.string().c_str(), "new");

      debug() << "Saving histograms for run " << run << " to " << outPath.string() << endmsg;

      // Get the normalization histogram, including the already saved bit.
      auto normIt = normalizers.find(run);
      TH1D* norm{nullptr};
      std::unique_ptr<TH1D> totalNorm;
      if (normIt != end(normalizers)) {
         norm = normIt->second.second;
         totalNorm = std::unique_ptr<TH1D>{static_cast<TH1D*>(norm->Clone())};
         totalNorm->SetDirectory(nullptr);
      }

      // Scale to the full lumi rate
      auto info = runInfo(run);
      if (totalNorm && info) {
         double scale = 1000 * std::accumulate(begin(info->lumiPars), end(info->lumiPars), 0.);
         totalNorm->Scale(1. / scale);
      } else {
         totalNorm.reset();
      }

      // Loop over histograms for that run
      for (const auto& entry : boost::make_iterator_range(m_histos.get<byRun>().equal_range(run))) {
         auto histo = entry.histo.get();
         auto dir = entry.dir;
         auto outDir = static_cast<TDirectoryFile*>(outFile.Get(dir.c_str()));
         if (!outDir) {
            outFile.mkdir(dir.c_str());
            outDir = static_cast<TDirectoryFile*>(outFile.Get(dir.c_str()));
         }
         if (!outDir) {
            warning() << "Could not create directory in SaveSet file "
                      << outPath.string() << " " << dir << endmsg;
            continue;
         }

         // Normalize
         if (totalNorm.get() && entry.type == Monitoring::s_Rate) {
            auto normName = string{histo->GetName()} + "_Rate";
            auto clone = static_cast<TH1D*>(histo->Clone(normName.c_str()));
            std::unique_ptr<TH1D> normHisto{clone};
            // Normalize to our normalization histogram
            normHisto->Divide(totalNorm.get());
            // Write normalized histogram to file
            outDir->WriteTObject(normHisto.get(), normName.c_str(), "overwrite");
            debug() << "Saved " << entry.dir << "/" << normName << endmsg;
         }

         // Write normal histogram to file
         outDir->WriteTObject(histo, histo->GetName());
         debug() << "Saved " << entry.dir << "/" << histo->GetName() << endmsg;

         // Reset all histograms that are not the normalization histogram
         if (histo != norm) {
            histo->Reset("ICESM");
         }
      }

      outFile.Close();

      // Lambda to replace previous file with new file
      auto replace = [outPath, run, this](bool byRun) {
         // Does the source file exist?
         bool exists{false};
         fs::path file;
         std::tie(file, exists) = filename(run, byRun);

         // filesystem error code
         boost::system::error_code ec;

         // Rename temporary file to output file
         if (exists) {
            bool success = fs::remove(file, ec);
            success &= !ec;
            if (!success) {
               warning() << "Could not remove file " << file.string() << endmsg;
            }
         }

         // Copy to the saveset
         fs::copy_file(outPath, file, ec);
         if (ec) {
            warning() << "Could not copy file " << outPath << " to "
                      << file.string() << endmsg;
         } else {
            this->info() << "Saved histograms for run " << run << " to " << file.string()
                         << "."  << endmsg;
         }
      };

      // Replace regular file
      replace(false);
      // Replace by run file
      replace(true);

      // Copying done, remove temporary file
      boost::system::error_code ec;
      bool success = fs::remove(outPath, ec);
      success &= !ec;
      if (!success) {
         warning() << "Could not remove file " << outPath.string() << endmsg;
      }

      // All other histograms for this run are saved, reset the normalization histogram.
      if (norm) {
         norm->Reset("ICESM");
      }
   }
}

//===============================================================================
array<string, 6> Hlt2SaverSvc::timeInfo(unsigned int run) const
{
   auto info = runInfo(run);
   if (!info) {
      return {"", "", "", "", "", ""};
   }

   using clock_t = std::chrono::high_resolution_clock;
   using tp_t = std::chrono::time_point<clock_t>;
   const clock_t::duration startTime = std::chrono::seconds(info->start);
   std::time_t t = clock_t::to_time_t(tp_t{startTime});
   std::tm* start = gmtime(&t);

   auto fmt = [](int t)->string { return (boost::format("%02d") % t).str(); };
   return {to_string(start->tm_year + 1900),
           fmt(start->tm_mon + 1), fmt(start->tm_mday),
           fmt(start->tm_hour), fmt(start->tm_min), fmt(start->tm_sec)};
}

//===============================================================================
boost::optional<Monitoring::RunInfo>
Hlt2SaverSvc::runInfo(Monitoring::RunNumber run) const
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

//===============================================================================
std::pair<fs::path, bool> Hlt2SaverSvc::filename(Monitoring::RunNumber run, bool byRun) const
{
   auto ti = timeInfo(run);
   fs::path directory;
   if (m_directory.empty()) {
      if (byRun) {
         string first = to_string((run / 10000) * 10000);
         string second = to_string((run / 1000) * 1000);
         directory = fs::path{s_directory} / "ByRun" / "Moore2Saver" / first / second;
      } else {
         directory = fs::path{s_directory} / ti[0] / m_partition / "Moore2Saver" / ti[1] / ti[2];
      }
   } else {
      if (byRun) {
         directory = fs::path{m_directory} / "ByRun";
      } else {
         directory = fs::path{m_directory};
      }
   }
   if (!fs::exists(directory)) {
      boost::system::error_code ec;
      bool success = fs::create_directories(directory, ec);
      success &= !ec;
      if (!success) {
         warning() << "Failed to create directory " << directory << endmsg;
         return make_pair(directory, false);
      }
   }
   boost::format fn;
   if (byRun) {
      fn = boost::format(m_runFileName) % run;
   } else {
      fn = boost::format(m_fileName) % run % ti[0] % ti[1] % ti[2] % ti[3] % ti[4] % ti[5];
   }
   auto file = fs::path(directory) / fs::path(fn.str());

   bool good = true;
   boost::system::error_code ec;
   good = fs::exists(file, ec);
   good &= !ec;

   return make_pair(file, good);
}
