// Include files
// STD and STL
#include <algorithm>
#include <iostream>
#include <fstream>
#include <chrono>
#include <memory>
#include <thread>
#include <vector>
#include <string>
#include <mutex>
#include <tuple>
#include <map>
#include <set>
#include <regex>

// Boost Regex, cast and FileSystem
#include <boost/regex.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>
#include <boost/filesystem.hpp>

// Boost Qi, Spirit and Phoenix
#define FUSION_MAX_VECTOR_SIZE 20
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>

// from Gaudi
#include <GaudiKernel/AlgFactory.h>
#include <DetDesc/Condition.h>
#include <DetDesc/RunChangeIncident.h>

// OT detector element
#include <OTDet/DeOTDetector.h>

//to publish online
#include <GaudiKernel/MsgStream.h>
#include <GaudiKernel/ServiceLocatorHelper.h>
#include <GaudiKernel/IPublishSvc.h>

//for monitoring histogram
#include <AIDA/IHistogram1D.h> 

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
   using std::vector;
   using std::string;
   using std::ofstream;
   using std::to_string;
   using std::pair;
   using std::make_pair;
   using std::tuple;
   using std::make_tuple;

   const std::string EOR = "EOR";
}

//=============================================================================
OTt0OnlineClbr::OTt0OnlineClbr( const string& name, ISvcLocator* pSvcLocator)
   : AnalysisTask ( name , pSvcLocator ),
     m_pubString{},
     m_pubStatus{},
     m_publishSvc{nullptr},
     m_condition{nullptr},
     m_incidentSvc{nullptr},
     m_phase{nullptr}
{

   //currently broken: doesn't do anything
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
   declareProperty("UseClockPhase", m_useClockPhase = true);

   declareProperty("XMLFilePath", m_xmlFilePath = "/group/online/alignment/OT/Calib" );
   declareProperty("PublishedName", m_pubName = "OT/Calib");
   declareProperty("PublishSleep", m_pubSleep = 1);
   declareProperty("MaxTimeDiff", m_maxTimeDiff = 3600);
   declareProperty("InitialTime",  m_initialTime  = 0);
   declareProperty("RunOnline",  m_RunOnline  = true);
   declareProperty("CheckDataT0",  m_checkDataT0  = true);
   declareProperty("RunConditionPath", m_runCondPath
                   = "/group/online/hlt/conditions/LHCb/2015/%d/ot.xml");
   declareProperty("T0ConditionPath", m_conditionPath
                   = "/dd/Conditions/Calibration/OT/CalibrationGlobal");
   declareProperty("Histograms",  m_histogramNames =
         {"OT/OTTrackMonitor/station1/drifttimeresidualgood",
          "OT/OTTrackMonitor/station2/drifttimeresidualgood",
          "OT/OTTrackMonitor/station3/drifttimeresidualgood"});

   //inputs for testing

   declareProperty("InputFileName", m_InputFileName  = "clbr_hists_109.root");
   declareProperty("InputFilePath", m_InputFilePath  = "/afs/cern.ch/work/l/lgrillo/OT_CALIBRATION_files/");
   declareProperty("DataT0"       , m_dataT0  = 0.);
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

   if(m_RunOnline){
     sc = serviceLocator()->service("LHCb::PublishSvc", m_publishSvc, true);
     if ( sc.isFailure() ) return sc;
     m_publishSvc->declarePubItem("OT/Calib", m_pubString);
     m_publishSvc->declarePubItem("OT/CalibStatus", m_pubStatus);
   }
   // ApplicationMgr
   m_evtProc = service("ApplicationMgr");
   if (!m_evtProc) {
      error() << "Failed to retrieve ApplicationMgr as EventProcessor" << endmsg;
      return StatusCode::FAILURE;
   }

   // Detector data service
   m_detDataSvc = service("DetectorDataSvc");
   if (!m_detDataSvc) {
      error() << "Failed to retrieve DetectorDataSvc" << endmsg;
      return StatusCode::FAILURE;
   }

   // IncidentSvc
   updMgrSvc();
   m_incidentSvc = service("IncidentSvc");
   if (!m_incidentSvc) {
      error() << "Failed to retrieve IndicentSvc" << endmsg;
      return StatusCode::FAILURE;
   }

   // Register the condition so we can get its value.
   updMgrSvc()->registerCondition<OTt0OnlineClbr>(this, m_conditionPath,
                                                  &OTt0OnlineClbr::updateT0,
                                                  m_condition);

   // LHCb Clock phase from DIM
   if (m_useClockPhase) {
      m_phase = unique_ptr<ClockPhase>{new ClockPhase{}};
   }

   // Regex and lambda to extract version from filename vN.xml
   boost::regex re("^v([0-9]+)\\.xml$");
   auto version = [re, this](const string& filename) {
      boost::smatch matches;
      auto flags = boost::match_default;
      if (!fs::is_regular_file(fs::path{m_xmlFilePath} / fs::path{filename}))
         return 0u;
      if (boost::regex_match(begin(filename), end(filename), matches, re, flags)) {
         return boost::lexical_cast<unsigned int>(matches[1]);
      } else {
         return 0u;
      }
   };

   // Store versions
   std::transform(fs::directory_iterator{fs::path{m_xmlFilePath}},
                  fs::directory_iterator{}, std::inserter(m_versions, end(m_versions)),
                  [version](const fs::directory_entry& e) {
                     return version(e.path().filename().string());
                  });

   // Read XML to get previous phases.
   for (auto v : m_versions) {
      auto r = readXML(xmlFileName(v));
      const auto phase = std::get<1>(r);
      if (phase) {
         m_phases.emplace(std::get<0>(r), *phase);
      }
   }

   return sc;
}

//=============================================================================
StatusCode OTt0OnlineClbr::start()
{
   // The thread is started here to ensure that everything is ready when we need
   // it.
   m_thread = std::thread([this]{
         while (true) {
            auto r = m_queue.get();
            if (r.first == "stop") {
               break;
            } else {
               calibrate(r.first, r.second);
            }
         }
      });
   return StatusCode::SUCCESS;
}

//=============================================================================
StatusCode OTt0OnlineClbr::analyze (string& saveSet, string task)
{
   // We do the work on a thread so the DIM thread is not blocked if we want to
   // to wait for start to have completed.
  m_queue.put(make_pair(saveSet, task));
  return StatusCode::SUCCESS;
}

//=============================================================================
StatusCode OTt0OnlineClbr::calibrate(string saveSet, string task)
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

   // Get latest version, bootstrap if needed
   boost::optional<unsigned int> latest = latestVersion();
   unsigned int prevRun = 0;
   double prevT0 = 0.;
   boost::optional<double> prevPhase;
   if (!latest || m_readFromDB) {
     // booststrap from DB
     double t0 = m_condition->param<double>("TZero");
     boost::optional<double> phase;
     latest = 0;
     //writeXML(*latest, 0, phase, t0);
     fs::path filename_xml = xmlFileName(*latest);
     writeXML(*latest,filename_xml, 0, phase, t0);
     publish({}, *latest, "good");
   } else {
     // Parse the file to get the info from the previous calibration.
     std::tie(prevRun, prevPhase, prevT0) = readXML(xmlFileName(*latest));
     if (prevRun) {
       m_calibratedRuns.emplace(prevRun);
       publish({}, *latest, "good");
     }
   }

   // Output file for testing
   unique_ptr<TFile> outFile;
   //Path for monitoring file
   string monitoring_path;
   string file_name{fs::path{saveSet}.string()};
   int found = file_name.find_last_of("/");
   monitoring_path = file_name.substr(0,found+1);
   string monitoring_file = file_name.substr(found+1);
   //info() << "Monitoring histogram path: " << monitoring_path << endmsg;
   //info() << "Monitoring histogram file: " << monitoring_file << endmsg;
   int found_b = monitoring_path.find_last_of("b");
   string first_path = monitoring_path.substr(0,found_b+2);
   //info() << "first_path: " << first_path << endmsg;
   int found_l = monitoring_path.find_last_of("l");
   string date_path = monitoring_path.substr(found_l+1);
   //info() << "date_path: " << date_path << endmsg;
   int found_l_file = monitoring_file.find_last_of("l");
   string date_file = monitoring_file.substr(found_l_file+1);
   //info() << "date_file: " << date_file << endmsg;

   // Run number
   unsigned int run = 0;

   // Figure out which files we need.
   if(m_RunOnline){
      const string fileName{fs::path{saveSet}.filename().string()};
      boost::regex re(string("Brunel") + "-(?<run>\\d+)-(?<time>[\\dT]+)(-EOR)?\\.root$");
      boost::smatch matches;
      auto flags = boost::match_default;
      if (!boost::regex_match(begin(fileName), end(fileName), matches, re, flags)) {
         error() << "Bad SaveSet filename: " << saveSet << endmsg;
         return StatusCode::FAILURE;
      } else {
         // Get run number
         run = boost::lexical_cast<unsigned int>(matches.str("run"));
         info() << "Filename: " << saveSet << ", run: " << run << endmsg;
      }

      // Check if the required XML is there so we can read it. If not, we con't care about this run.
      if (!fs::exists((boost::format(m_runCondPath) % run).str())) {
         info() << "Cannot find online XML files used for data for run : " << run
                << " skipping it." << endmsg;
         return StatusCode::SUCCESS;
      }

      // Check if already done, if so: return.
      if (m_calibratedRuns.count(run)) {
         info() << "Run : " << run << " already calibrated." << endmsg;
         return StatusCode::SUCCESS;
      }

      // Fire RunChange incident to load XML used in data.
      m_incidentSvc->fireIncident(RunChangeIncident(name(), run));
      updMgrSvc()->newEvent();

      // Parse timestamp
      std::tm tm;
      bzero(&tm, sizeof(std::tm));
      strptime(matches.str("time").c_str(), "%Y%m%dT%H%M%S", &tm);
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
         info() << "Time difference with previous SaveSet too large: " << diff
                << ", not calibrating previous runs." << endmsg;
         publish(calibrating(), *latest, "bad");
         m_calibrating.clear();
      }

      // If previous calibration was not picked up, don't do anything.
      if (*latest && m_checkDataT0 && (std::abs(m_dataT0 - prevT0) > 0.001)) {
         info() << "T0 used in data and previous calibration are different, "
                << "calibration has not been picked up yet." << endmsg;
         return StatusCode::SUCCESS;
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

      // Save your own TFile
      //outFile = unique_ptr<TFile>{new TFile((monitoring_path+"OTOnlineClbr-"+fileName).c_str(), "RECREATE")};
      if(m_saveFits){
	fs::path mpath((first_path+"OTOnlineClbr"+date_path).c_str());
	if(!exists(mpath))
	  fs::create_directories( mpath );
	outFile = unique_ptr<TFile>{new TFile((first_path+"OTOnlineClbr"+date_path+"OTOnlineClbr"+date_file).c_str(), "RECREATE")};
      }

   } else {
      // Test mode.
      m_calibrating[0].push_back(m_InputFilePath + m_InputFileName);
      //outFile = unique_ptr<TFile>{new TFile("calibration_monitoring.root", "RECREATE")};
      //outFile = unique_ptr<TFile>{new TFile((monitoring_path+"OTOnlineClbr-"+m_InputFileName).c_str(), "RECREATE")};
      if(m_saveFits){
	fs::path mpath((first_path+"OTOnlineClbr"+date_path).c_str());
	if(!exists(mpath))
	  fs::create_directories( mpath );
	outFile = unique_ptr<TFile>{new TFile((first_path+"OTOnlineClbr"+date_path+"OTOnlineClbr"+date_file).c_str(), "RECREATE")};
      }
      run = 0;
   }

   unique_ptr<TH1D> histogram{getHistogram()};
   if (!histogram || histogram->Integral() < m_minEntries) {
      // Not enough entries, return and wait for next file.
      info() << "Not enough statistics, wait for next file." << endmsg;
      return StatusCode::SUCCESS;
   }

   // Read last XML to get reference.
   info() << "Latest version: " << (latest ? std::to_string(*latest) : string("No calibration")) << endmsg
          << "T0 in data = " << m_dataT0 << endmsg
          << "LHCb clock phase in data = "
          << (prevPhase ? to_string(*prevPhase) : string("undefined")) << endmsg
          << "Previous t0: "    << prevT0 << endmsg
          << "Last used run: "  << prevRun << endmsg;

   // Calibrate the T0
   double residual = 0.0;
   double residual_err = 0.0;

   if (m_useMean){
      residual = histogram->GetMean();
      residual_err = histogram->GetMeanError();
   } else {
      fitHistogram(histogram.get(), residual, residual_err, outFile.get());
   }
   double t0 = m_dataT0 + residual;

   info() << "Global t0: "      << t0 << endmsg;
   info() << "Global dt0: "     << residual  << endmsg;
   info() << "Global dt0_err: " << residual_err << endmsg;


   // The runs we are calibrating.
   auto runs = calibrating();

   // Take LHCb clock phase into account
   boost::optional<double> phase;
   if (m_useClockPhase) {
      phase = m_phase->phase();
   }

   // In case the difference is large, but it's actaully the global clock that has changed.
   double diffWithPhase = residual;
   if (prevPhase && phase) {
      diffWithPhase = residual + *phase - *prevPhase;
   }

   // Check and improve condition for writing and publishing
   if (std::abs(residual) > m_threshold) {
      if (std::abs(diffWithPhase) < m_maxDifference) {
	//writeXML(*latest + 1, run, phase, t0);
	 fs::path filename_xml = xmlFileName(*latest + 1);
	 writeXML(*latest + 1,filename_xml, 0, phase, t0);
         publish(std::move(runs), *latest + 1, "good");
         info() << "Wrote global t0 xml. global t0 = "<< t0
                << ", global t0 threshold = " << m_threshold <<endmsg;
      } else {
         // Indicate problem and use previous threshold
         publish(std::move(runs), *latest, "bad");
         info() << "Did not write global t0 xml. global t0 = "<< t0
                << ", global t0 threshold = " << m_threshold <<endmsg;
      }
   } else {
      // No update, publish last file.
      publish(std::move(runs), *latest, "good");
      info() << "NOT writing global t0 xml. global t0 = "<< t0
             << ", global t0 threshold = " << m_threshold <<endmsg;
   }
   // Store calibrated run
   m_calibratedRuns.insert(run);
   m_calibrating.clear();

   return StatusCode::SUCCESS;
}

//=============================================================================
fs::path OTt0OnlineClbr::xmlFileName(FileVersion v) const
{
   return fs::path(m_xmlFilePath) / (string("v") + to_string(v) + ".xml");
}

//=============================================================================
// fs::path OTt0OnlineClbr::writeXML(const FileVersion version,
//                                   const unsigned int run,
//                                   const boost::optional<double> phase,
//                                   const double t0)
fs::path OTt0OnlineClbr::writeXML(const FileVersion version,
				  const boost::filesystem::path XML_path,
				  const unsigned int run,
				  const boost::optional<double> phase,
				  const double t0)
{

   StatusCode sc = StatusCode::SUCCESS ;

   //   fs::path filename = xmlFileName(version);
   //   fs::path filename(XML_path+XML_filename);
   fs::path filename = XML_path;
   ofstream file(filename.string().c_str());

   info() << "Writing the global t0 XML for online to "
          << filename.string() << endmsg ;

   // file << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n";
   // file << "<!DOCTYPE DDDB SYSTEM \"conddb:/DTD/structure.dtd\">\n";
   // file << "\n";
   // file << "<DDDB>\n";
   file << "<!-- run: " << run << " -->" << endl;
   if (phase) file << "<!-- phase: " << phase << " -->" << endl;
   file << "<condition classID=\"5\" name=\"" << "CalibrationGlobal" << "\">\n";
   file << "  <param name=\"TZero\" type=\"double\" comment=\"Global t0 of OT\">\n";
   //file << "      " << 0.0001 * (int)(10000.0 * t0 + 0.5) << "\n";
   file << "      " << std::setprecision(4) << t0 << "\n";
   //file << "    " <<  t0  << "\n";
   file << "  </param>\n";
   file << "</condition>\n";
   // file << "</DDDB>\n";
   //  }
   file.close();


   // Insert the version we wrote.
   m_versions.insert(version);
   if (phase) m_phases.emplace(run, *phase);
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
   info() << "Fit result: " << result << " +- " << result_error << endmsg ;

   //for publish monitoring histograms
   if(m_saveFits && m_RunOnline){
     //to save your own TH1 in your own file
     outFile->cd();
     info()<< "Saving histogram" <<endmsg;                                                                                                      
     hist->SetName("Global_hist");                                                                                                              
     hist->SetTitle("Global t0");                                                                                                               
     m_hist_aida = book1D( "Global_hist", "Global t0", hist->GetXaxis()->GetXmin(),hist->GetXaxis()->GetXmax(),hist->GetNbinsX());              
     declareInfo( "Global_hist", m_hist_aida, "Global t0" );                                                                                    
     for(int i = 0; i<hist->GetNbinsX()+1;i++){                                                                                                 
       m_hist_aida->fill(hist->GetBinCenter(i),hist->GetBinContent(i));                                                                         
     }                                                                                                                             
     hist->Write();
   }

   //only to save stuff
   if(m_saveFits && outFile && !m_RunOnline){
      outFile->cd();
      info()<< "Saving histogram" <<endmsg;
      hist->SetName("Global_hist");
      hist->Write();
   }
}

//=============================================================================
StatusCode OTt0OnlineClbr::execute() {
  if (!m_RunOnline){
    string saveSet = m_InputFilePath + m_InputFileName;
    return analyze(saveSet, " ");
  }
  return StatusCode::SUCCESS; 
}
//=============================================================================
StatusCode OTt0OnlineClbr::finalize() {

   if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;
   // Stop worker thread
   m_queue.put(make_pair("stop", ""));
   m_thread.join();
   return AnalysisTask::finalize();  // must be called after all other actions
}

//=============================================================================
boost::optional<unsigned int> OTt0OnlineClbr::latestVersion() const
{
   boost::optional<unsigned int> r;
   // Find highest version
   auto v = std::max_element(begin(m_versions), end(m_versions));
   if (v == end(m_versions)) {
      debug() << "No XML found." << endmsg;
   } else {
      r = *v;
      debug() << "Latest version: " << *r << endmsg;
   }
   return r;
}

//=============================================================================
tuple<unsigned int, boost::optional<double>, double>
OTt0OnlineClbr::readXML(const fs::path& xmlFile)
{
   namespace sp = boost::spirit;
   namespace qi = sp::qi;
   namespace ph = boost::phoenix;
   using qi::double_;
   using qi::uint_;
   using qi::phrase_parse;
   using qi::lexeme;
   using qi::_1;
   using qi::_val;
   using qi::attr;
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
   boost::optional<double> phase;

   auto rule =
      lit("<!-- run:")
      >> uint_[ph::ref(run) = _1] >> "-->" // comment: <!-- run: int >
      >> -("<!-- phase:" >> double_[ph::ref(phase) = _1] >> "-->")
      >> "<condition"
      >> *(
           (lit("name") >> '=' >> '"'
            >> sp::as_string[+(char_ - '"')][ph::ref(name) = _1]
            >> char_('"'))
           | (~char_('>'))
           ) >> '>' // <condition ... name="{name}" ... >
      >> "<param"
      >> *(
           (lit("name") >> '=' >> '"'
            >> sp::as_string[+(char_ - '"')][ph::ref(param) = _1]
            >> char_('"'))
           | (~char_('>'))
           ) >> '>' // <param ... name="{param}" ... >
      >> double_[ph::ref(t0) = _1]
      >> "</param>"
      >> "</condition>";

   bool full = phrase_parse(file_begin, file_end, rule, space);

   if (!full || file_begin != file_end) {
      warning() << "XML file " << xmlFile << " could not be parsed to obtain previous T0"
                << endmsg;
      return make_tuple(0, phase, 0.);
   }
   if (name != "CalibrationGlobal" || param != "TZero") {
      warning() << "XML file " << xmlFile << " was parsed, but parameter names do not match: "
                << name << " should be CalibrationGlobal, and "
                << param << " should be TZero."
                << endmsg;
      return make_tuple(0, phase, 0.);
   } else {
      debug() << "Read xml file " << xmlFile << " and found run: " << run << " with T0: "
              << t0 << " and clock phase: " << (phase ? to_string(*phase) : string{"undefined"})
              << endmsg;
      return make_tuple(run, phase, t0);
   }
}

//=============================================================================
void OTt0OnlineClbr::publish(const vector<unsigned int> runs,
                             const FileVersion version,
                             const string status)
{
   // Lambda to do the actual publishing.
   auto pub = [this] (const unsigned int run,
                      const string& end, string& member,
                      const string& item) {
      if (run != 0 && m_RunOnline) {
         member = to_string(run) + " " + end;
         debug() << "Publishing : " << item << " " << member << endmsg;
         m_publishSvc->updateItem(item);
      }
   };

   for (unsigned int run : runs) {
      pub(run, string("v") + to_string(version), m_pubString, m_pubName);
      pub(run, status, m_pubStatus, m_pubName + "Status");
      if (m_calibrating.size() > 1) {
         std::this_thread::sleep_for(std::chrono::seconds(m_pubSleep));
      }
   }
}

//=============================================================================
StatusCode OTt0OnlineClbr::updateT0() {
   m_dataT0 = m_condition->param<double>("TZero");
   return StatusCode::SUCCESS;
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
