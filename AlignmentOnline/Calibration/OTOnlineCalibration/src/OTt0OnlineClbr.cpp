// Include files
// STD and STL
#include <algorithm>
#include <iostream>
#include <fstream>
#include <chrono>
#include <memory>
#include <thread>
#include <chrono>
#include <vector>
#include <string>
#include <map>
#include <set>

// Boost Regex, cast and FileSystem
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>

// Boost Qi, Spirit and Phoenix
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>

// from Gaudi
#include <GaudiKernel/AlgFactory.h>
#include <DetDesc/Condition.h>

//to publish online
#include <GaudiKernel/MsgStream.h>
#include <GaudiKernel/ServiceLocatorHelper.h>
#include <GaudiKernel/IPublishSvc.h>

// local
#include "OTt0OnlineClbr.h"

//-----------------------------------------------------------------------------
// Implementation file for class : OTt0OnlineClbr
//
// 2015-01-08 : Lucia Grillo
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( OTt0OnlineClbr )

namespace {
   namespace fs = boost::filesystem;
   using namespace LHCb;
   using std::unique_ptr;
   using std::endl;
   using std::cout;
   using std::pair;
   using std::vector;
   using std::string;
   using std::ofstream;
   using std::to_string;
   using std::make_pair;
   
   const std::string EOR = "EOR";
}

//=============================================================================
OTt0OnlineClbr::OTt0OnlineClbr( const string& name, ISvcLocator* pSvcLocator)
   : AnalysisTask ( name , pSvcLocator ),
     m_pubString{},
     m_pubStatus{},
     m_pPublishSvc{nullptr}
{

   //currently broken: doesnt do anything
   declareProperty("SaveFits", m_saveFits = false, " Save fitted gaussian (default false,)");
   declareProperty("UseMean", m_useMean = false,
                   "GetMean instead of fit the distributions ");

   //Current OT decision: run online the global t0 calibration ONLY and update when needed that constant ONLY
   declareProperty("ReadInitialT0FromDB", m_readFromDB = false,
                   "Reads Global t0 from db. at the moment is an option beacuse of implementation ");

   declareProperty("Threshold", m_threshold = 0.1,
                   "if the global t0 exceedes this threshold, we want to update it");
   declareProperty("MaxDifference", m_maxDifference = 1.,
                   "if the global t0 exceedes this threshold, don't to update it and indicate a problem.");
   declareProperty("MinEntries", m_minEntries = 50000,
                   "Minimum number of required entries to perform the calibration.");

   declareProperty("XMLFilePath", m_xmlFilePath = "/group/online/alignment/OT/Calib" );
   declareProperty("CondStructure", m_CondStructure = "Conditions/OT/Calibration/" );
   declareProperty("PublishSleep", m_pubSleep = 1);
   declareProperty("MaxTimeDiff", m_maxTimeDiff = 3600);
   declareProperty("RunOnline",  m_RunOnline  = true );
   declareProperty("Histograms",  m_histogramNames =
         {"OT/OTTrackMonitor/station1/drifttimeresidualgood",
          "OT/OTTrackMonitor/station2/drifttimeresidualgood",
          "OT/OTTrackMonitor/station3/drifttimeresidualgood"});

   //inputs for testing
   declareProperty("InputFileName"   ,  m_InputFileName  = "clbr_hists_109.root" );
   declareProperty("InputFilePath"   ,  m_InputFilePath  = "/afs/cern.ch/work/l/lgrillo/OT_CALIBRATION_files/" );

}

//=============================================================================
OTt0OnlineClbr::~OTt0OnlineClbr()
{

}

//=============================================================================
StatusCode OTt0OnlineClbr::initialize()
{
   // Init things for this task
   // must be run here
   if (!fs::exists(m_xmlFilePath)) {
      fs::create_directories(m_xmlFilePath);
   }

   StatusCode sc = AnalysisTask::initialize(); // must be executed first
   if ( sc.isFailure() ) return sc;  // error printed already by AnalysisTask

   if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

   sc = serviceLocator()->service("LHCb::PublishSvc", m_pPublishSvc, false);
   if ( sc.isFailure() ) return sc;
   m_pPublishSvc->declarePubItem("OT/T0", m_pubString);
   m_pPublishSvc->declarePubItem("OT/T0Status", m_pubStatus);

   return sc;
}

//=============================================================================
StatusCode OTt0OnlineClbr::start()
{
   if ( msgLevel(MSG::DEBUG) ) debug() << "==> Start" << endmsg;
   auto v = latestVersion();
   if (v == 0 || m_readFromDB) {
      // booststrap from DB
      double t0 = readCondDB();
      writeXML(v + 1, 0, t0);
      publish({0}, v + 1, "good");
   } else {
      // Read last xml
      auto xmlFile = xmlFileName(v);
      pair<unsigned int, double> r = readXML(xmlFile);
      m_calibratedRuns.emplace(r.first);
      // Parse the file to get the previous run number.
      publish({r.first}, v, "good");
   }
   return StatusCode::SUCCESS;
}

//=============================================================================
StatusCode OTt0OnlineClbr::execute()
{
   if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

   return StatusCode::SUCCESS;
}

//=============================================================================
StatusCode OTt0OnlineClbr::analyze (string& saveSet, string task)
{
   if ( msgLevel(MSG::DEBUG) ) debug() << "==> Analyze" << endmsg;

   // Mandatory: call default analyze method
   StatusCode sc = AnalysisTask::analyze(saveSet, task);
   if ( sc.isFailure() ) return sc;

   // Lambda to convert runs as keys of calibrated runs container to only run
   // numbers
   auto calibrating = [this] {
      // Store calibrated runs in a vector to pass to publish
      vector<unsigned int> runs(m_calibrating.size());
      std::transform(begin(m_calibrating), end(m_calibrating), begin(runs),
                     [](const decltype(m_calibrating)::value_type& entry) {
                        return entry.first;
                     });
      return runs;
   };
   
   unique_ptr<TFile> outFile;
   // Get latest version
   const FileVersion latest = latestVersion();
   const unsigned int run = 0;
   
   if(m_RunOnline){
      const string fileName{fs::path{saveSet}.filename().string()};
      boost::regex re(string("Brunel") + "-(?<run>\\d+)-(?<time>[\\dT]+)\\.root$");
      boost::smatch matches;
      auto flags = boost::match_default;
      if (!boost::regex_match(begin(fileName), end(fileName), matches, re, flags)) {
         error() << "Bad SaveSet filename: " << saveSet << endmsg;
         return StatusCode::FAILURE;
      }

      // Get run number
      const unsigned int run = boost::lexical_cast<unsigned int>(matches.str("run"));
      debug() << "File name: " << saveSet << ", run: " << run << endmsg;

      // Check if already done, if so: return.
      if (m_calibratedRuns.count(run)) {
         debug() << "Run : " << run << " already calibrated." << endmsg;
         return StatusCode::SUCCESS;
      }

      // Parse timestamp
      std::tm tm;
      bzero(&tm, sizeof(std::tm));
      strptime(matches.str("year").c_str(), "%Y%m%dT%H%M%S", &tm);
      auto tp = std::chrono::system_clock::from_time_t(std::mktime(&tm));

      // Calculate time difference with previous SaveSet.
      unsigned int diff = 0;
      if (m_calibrating.empty()) {
         // First file we get
         m_start = tp;
      } else {
         diff = std::chrono::duration_cast<std::chrono::seconds>(tp - m_start).count();
      }

      // If too much difference and some runs are left, publish problem + old
      // versions for those runs.
      if (diff > m_maxTimeDiff) {
         publish(calibrating(), latest, "bad");
         m_calibrating.clear();
      }
      
      // Check if a calibration can be done, if not return, else calibrate.
      if (saveSet.find(EOR) != string::npos) {
         // Either no intermediate saveset were produced and this is the EOR one, 
         // or some files already processed, but not enough statistics.
         // Use only the EOR set in both cases.
         m_calibrating[run] = vector<string>(1, saveSet);
      } else {
         // Add to files for check of sufficient stats.
         m_calibrating[run].emplace_back(saveSet);
      }
   } else {
      // Test mode.
      m_calibrating[0].push_back(m_InputFilePath + m_InputFileName);
      outFile = unique_ptr<TFile>{new TFile("calibration_monitoring.root", "RECREATE")};
   }

   unique_ptr<TH1D> histogram{getHistogram()};
   if (!histogram || histogram->Integral() < m_minEntries) {
      // Not enough entries, return and wait for next file.
      debug() << "Not enough statistics, wait for next file." << endmsg;
      return StatusCode::SUCCESS;
   }

   double t0 = 0.0;
   double prevT0 = 0.0;

   // Read last XML to get reference.
   auto xmlFile = xmlFileName(latest);
   auto r = readXML(xmlFile);
   m_calibratedRuns.insert(r.first);
   prevT0 = r.second;

   debug() << "Global t0 = " << prevT0 << endmsg;

   // Calibrate the T0
   double residual = 0.0;
   double residual_err = 0.0;

   if (m_useMean){
      residual = histogram->GetMean();
      residual_err = histogram->GetMeanError();
   } else {
      fitHistogram(histogram.get(), residual, residual_err, outFile.get());
   }
   t0 = prevT0 + residual;

   debug() << "Global t0: "      << t0 << endmsg;
   debug() << "Global dt0: "     << residual  << endmsg;
   debug() << "Global dt0_err: " << residual_err << endmsg;

   // The runs we are calibrating.
   auto runs = calibrating();
   
   // Check and improve condition for writing and publishing
   if (std::abs(t0) > m_threshold) {
      if (std::abs(t0) < m_maxDifference) {
         writeXML(latest + 1, run, t0);
         publish(std::move(runs), latest + 1, "good");
         debug() << "Wrote global t0 xml. global t0 = "<< t0
                 << ", global t0 threshold = " << m_threshold <<endmsg;
      } else {
         // Indicate problem and use previous threshold
         publish(std::move(runs), latest, "bad");
         debug() << "Did not write global t0 xml. global t0 = "<< t0
                 << ", global t0 threshold = " << m_threshold <<endmsg;
      }
   } else {
      // No update, publish last file.
      publish(std::move(runs), latest, "good");
      debug() << "NOT writing global t0 xml. global t0 = "<< t0
              << ", global t0 threshold = " << m_threshold <<endmsg;
   }
   // Store calibrated run
   m_calibratedRuns.insert(run);
   m_calibrating.clear();

   return StatusCode::SUCCESS;
}

//=============================================================================
double OTt0OnlineClbr::readCondDB()
{
   string loc ="/dd/Conditions/Calibration/OT/CalibrationGlobal";

   Condition *cond = get<Condition>(detSvc(), loc);
   double t0 = cond->paramAsDouble( "TZero" );
   debug() << "Read global t0 fom CondDB " << t0 << endmsg;
   return t0;
}

//=============================================================================
fs::path OTt0OnlineClbr::xmlFileName(FileVersion v) const
{
   return fs::path(m_xmlFilePath) / (string("v") + to_string(v) + ".xml");
}

//=============================================================================
fs::path OTt0OnlineClbr::writeXML(const FileVersion version,
                                  const unsigned int run, const double t0)
{

   StatusCode sc = StatusCode::SUCCESS ;

   fs::path filename = xmlFileName(version);
   ofstream file(filename.string().c_str());

   if ( msgLevel(MSG::DEBUG) ) debug() << "Writing the global t0 XML for online to "
                                       << filename.string() << endmsg ;

   // file << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n";
   // file << "<!DOCTYPE DDDB SYSTEM \"conddb:/DTD/structure.dtd\">\n";
   // file << "\n";
   // file << "<DDDB>\n";
   file << "<!-- run: " << run << " -->" << endl;
   file << "<condition classID=\"5\" name=\"" << "CalibrationGlobal" << "\">\n";
   file << "  <param name=\"TZero\" type=\"double\" comment=\"Global t0 of OT\">\n";
   //file << "      " << 0.001 * (int)(1000.0 * t0 + 0.5) << "\n";
   file << "    " <<  t0  << "\n";
   file << "  </param>\n";
   file << "</condition>\n";
   // file << "</DDDB>\n";
   //  }
   file.close();
   return filename;
}

//=============================================================================
void OTt0OnlineClbr::fitHistogram(TH1D* hist, double& result,
                                  double& result_error,  TFile* outFile)
{
   double left = hist->GetXaxis()->GetXmin();
   double right = hist->GetXaxis()->GetXmax();

   for(int i = 0; i < 5; i++)
   {
      hist->Fit("gaus", "QRLL", "", left, right);
      left = hist->GetFunction("gaus")->GetParameter(1) - 1.0 * hist->GetFunction("gaus")->GetParameter(2);
      right = hist->GetFunction("gaus")->GetParameter(1) + 1.0 * hist->GetFunction("gaus")->GetParameter(2);
   }
   result = hist->GetFunction("gaus")->GetParameter(1);
   result_error = hist->GetFunction("gaus")->GetParError(1);
   debug() << "Fit result: " << result << " +- " << result_error << endmsg ;

   //only to save stuff
   if(m_saveFits && outFile){
      outFile->cd();
      hist->SetName("Global_hist");
      hist->Write();
   }
}

//=============================================================================
StatusCode OTt0OnlineClbr::finalize() {

   if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

   return AnalysisTask::finalize();  // must be called after all other actions
}

//=============================================================================
unsigned int OTt0OnlineClbr::latestVersion() const
{
   boost::regex re("^v([0-9]+)\\.xml$");
   auto version = [re, this](const string& filename) {
      boost::smatch matches;
      auto flags = boost::match_default;
      if (!fs::is_regular_file(fs::path{m_xmlFilePath} / fs::path{filename}))
         return 0u;
      boost::regex_match(begin(filename), end(filename), matches, re, flags);
      if (!matches.empty()) {
         return boost::lexical_cast<unsigned int>(matches[1]);
      } else {
         return 0u;
      }
   };

   // Store filenames
   vector<string> files;
   std::transform(fs::directory_iterator{fs::path{m_xmlFilePath}},
                  fs::directory_iterator{}, std::back_inserter(files),
                  [](const fs::directory_entry& e) {
                     return e.path().filename().string();
                  });

   // Find highest version
   auto v = std::max_element(begin(files), end(files),
                             [version](const string& a, const string& b) {
                                return version(a) < version(b);
                             });
   if (v == end(files)) {
      return 0;
   } else {
      return version(*v);
   }
}

//=============================================================================
pair<unsigned int, double> OTt0OnlineClbr::readXML(const fs::path& xmlFile)
{
   namespace sp = boost::spirit;
   namespace qi = sp::qi;
   namespace ph = boost::phoenix;
   using qi::double_;
   using qi::uint_;
   using qi::phrase_parse;
   using qi::lexeme;
   using qi::_1;
   using qi::lit;
   using sp::ascii::space;
   using sp::ascii::space_type;
   using sp::ascii::char_;

   // open file, disable skipping of whitespace
   std::ifstream in(xmlFile.string().c_str());
   in.unsetf(std::ios::skipws);

   // wrap istream into iterator
   sp::istream_iterator file_begin(in);
   sp::istream_iterator file_end;

   unsigned int run = 0;
   string name;
   string param;
   double t0 = 0.;

   auto rule =
      lit("<!-- run:")
      >> *space >> uint_[ph::ref(run) = _1] >> *space >> "-->" // comment: <!-- run: int >
      >> "<condition"
      >> *(
           ("name" >> *space >> '=' >> *space >> '"'
            >> sp::as_string[+(char_ - '"')][ph::ref(name) = _1]
            >> char_('"'))
           | (~char_('>'))
           )
      >> '>' // <condition ... name="{name}" ... >
      >> *space >> "<param"
      >> *(
           ("name" >> *space >> '=' >> *space >> '"'
            >> sp::as_string[+(char_ - '"')][ph::ref(param) = _1]
            >> char_('"'))
           | (~char_('>'))
           ) >> '>' // <param ... name="{param}" ... >
      >> (*space)
      >> double_[ph::ref(t0) = _1]
      >> *space >> "</param>"
      >> "</condition>";

   bool full = phrase_parse(file_begin, file_end, rule, space);
   if (!full || file_begin != file_end) {
      warning() << "XML file " << xmlFile << " could not be parsed to obtain previous T0"
                << endmsg;
      return make_pair(0, 0.);
   }
   if (name != "CalibrationGlobal" || param != "TZero") {
      warning() << "XML file " << xmlFile << " was parsed, but parameter names do not match: "
                << name << " should be CalibrationGlobal, and "
                << param << " should be TZero."
                << endmsg;
      return make_pair(0, 0.);
   } else {
      debug() << "Read xml file " << xmlFile << " and found run: " << run << " with T0: "
              << t0 << endmsg;
      return make_pair(run, t0);
   }
}

//=============================================================================
void OTt0OnlineClbr::publish(const vector<unsigned int> runs,
                             const FileVersion version,
                             const string status)
{
   //static int Rn=0;
   char crn[255];
   auto pub = [&crn, this] (const unsigned int run,
                            const string& end, string& member,
                            const string& item) {
      ::sprintf(crn, (to_string(run) + " " + end).c_str());
      member = crn;
      m_pPublishSvc->updateItem(item.c_str());
   };
   for (unsigned int run : runs) {
      pub(run, string("v") + to_string(version), m_pubString, "OTT0Calibration");
      pub(run, status, m_pubStatus, "OTT0Calibration_Status");
      debug() << "Publishing calibration status: " << m_pubStatus << endmsg;
      if (m_calibrating.size() > 1) {
         std::this_thread::sleep_for(std::chrono::seconds(m_pubSleep));
      }
   }
}

//=============================================================================
unique_ptr<TH1D> OTt0OnlineClbr::getHistogram() const {

   auto getHist = [this](const string& filename) {
      unique_ptr<TFile> f{new TFile(filename.c_str(),"READ")};
      unique_ptr<TH1D> histo;
      if (f->IsZombie()) {
         warning() << filename << " is zombie." << endmsg;
         f->Close();
         return std::move(histo);
      }
      
      for (const auto& name : m_histogramNames) {
         const TH1D* h = static_cast<const TH1D*>(f->Get(name.c_str()));
         if (!histo) {
            histo = unique_ptr<TH1D>(static_cast<TH1D*>(h->Clone()));
            histo->SetDirectory(0);
            histo->Reset("ICES");
            histo->Add(h, 1.0);
         } else {
            histo->Add(h, 1.0);
         }
      }
      return std::move(histo);
   };

   unique_ptr<TH1D> histo;
   for (const auto& entry : m_calibrating) {
      for (const auto& filename : entry.second) {
         auto h = getHist(filename);
         if (!h) continue;
         if (!histo) {
            histo = unique_ptr<TH1D>{h.release()};
         } else {
            histo->Add(h.get(), 1.0);
         }
      }
   }
   return std::move(histo);
}
