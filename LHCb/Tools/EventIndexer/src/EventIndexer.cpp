// Include files

// From Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Incident.h"

// From LHCb
#include "Event/RecHeader.h"
#include "Event/RecSummary.h"
#include "Event/HltDecReports.h"

// local
#include "EventIndexer.h"

#define ON_DEBUG if (UNLIKELY(outputLevel() <= MSG::DEBUG))
#define ON_VERBOSE if (UNLIKELY(outputLevel() <= MSG::VERBOSE))

#define DEBMSG ON_DEBUG debug()
#define VERMSG ON_VERBOSE verbose()

// ----------------------------------------------------------------------------
// Implementation file for class: EventIndexer
//
// 13/03/2013: Marco Clemencic
// ----------------------------------------------------------------------------
DECLARE_ALGORITHM_FACTORY(EventIndexer)

// ============================================================================
// Standard constructor, initializes variables
// ============================================================================
EventIndexer::EventIndexer(const std::string& name, ISvcLocator* pSvcLocator)
  : base_class(name, pSvcLocator), m_file(0), m_tree(0)
{
  declareProperty("Stripping", m_stripping = "",
                  "Stripping version.");
  declareProperty("OutputFile", m_outputFileName = "indexer_data.root",
                  "Output file name for the indexer data.");
}

// ============================================================================
// Destructor
// ============================================================================
EventIndexer::~EventIndexer() {}

// ============================================================================
// Initialization
// ============================================================================
StatusCode EventIndexer::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc; // error printed already by GaudiAlgorithm

  DEBMSG << "==> Initialize" << endmsg;

  // register for "open input file" events
  m_incSvc = service("IncidentSvc");
  if (!m_incSvc) return StatusCode::FAILURE;
  m_incSvc->addListener( this, IncidentType::BeginInputFile);

  m_data.stripping = m_stripping;

  DEBMSG << "Opening " << m_outputFileName << " (mode: RECREATE)" << endmsg;
  m_file = TFile::Open(m_outputFileName.c_str(), "RECREATE");
  if (!m_file) {
    error() << "Cannot open file " << m_outputFileName << " (mode: RECREATE)" << endmsg;
    return StatusCode::FAILURE;
  }

  DEBMSG << "Creating the TTree" << endmsg;
  m_tree = new TTree("index", "Event metadata list", 1);
  m_tree->Branch("data", &m_data);

  return StatusCode::SUCCESS;
}


std::string pairs2dict(std::vector<LHCb::CondDBNameTagPair> v) {
    std::stringstream result;
    std::set <std::string> known_keys;
    result << "{";

    for (unsigned int i = 0; i < v.size(); i++) {
        LHCb::CondDBNameTagPair it = v[i];
        if (known_keys.insert(it.first).second) {
            result << "\"" << it.first << "\": \"" << it.second << "\", ";
        }
    }
    result << "}";
    return result.str();
}


std::string stripping2dict(LHCb::HltDecReports *decReports) {
    std::stringstream result;
    result << "{";
    for(LHCb::HltDecReports::Container::const_iterator it=decReports->begin(); it!=decReports->end(); ++it){
        result << "\"" << it->first << "\": \"" << it->second.numberOfCandidates() << "\", ";
    }
    result << "}";
    return result.str();
}


// ============================================================================
// Main execution
// ============================================================================
StatusCode EventIndexer::execute() {
  DEBMSG << "==> Execute" << endmsg;

  LHCb::RecHeader *rh = get<LHCb::RecHeader>(LHCb::RecHeaderLocation::Default);

  ++m_data.position; // increment the position counter (first event has got position == 1)
  m_data.eventNumber = rh->evtNumber();
  m_data.runNumber = rh->runNumber();
  m_data.gpsTime = rh->gpsTime();
  m_data.rawID = rh->rawID();
  m_data.applicationName = rh->applicationName();
  m_data.applicationVersion = rh->applicationVersion();
  m_data.condDBTags_dict = pairs2dict(rh->condDBTags());

  LHCb::RecSummary *rsummary = get<LHCb::RecSummary>(LHCb::RecSummaryLocation::Default);
  m_data.nPVs = rsummary->info(LHCb::RecSummary::nPVs, -999);
  m_data.nLongTracks = rsummary->info(LHCb::RecSummary::nLongTracks, -999);
  m_data.nDownstreamTracks = rsummary->info(LHCb::RecSummary::nDownstreamTracks, -999);
  m_data.nUpstreamTracks = rsummary->info(LHCb::RecSummary::nUpstreamTracks, -999);
  m_data.nVeloTracks = rsummary->info(LHCb::RecSummary::nVeloTracks, -999);
  m_data.nTTracks = rsummary->info(LHCb::RecSummary::nTTracks, -999);
  m_data.nBackTracks = rsummary->info(LHCb::RecSummary::nBackTracks, -999);
  m_data.nTracks = rsummary->info(LHCb::RecSummary::nTracks, -999);
  m_data.nRich1Hits = rsummary->info(LHCb::RecSummary::nRich1Hits, -999);
  m_data.nRich2Hits = rsummary->info(LHCb::RecSummary::nRich2Hits, -999);
  m_data.nVeloClusters = rsummary->info(LHCb::RecSummary::nVeloClusters, -999);
  m_data.nITClusters = rsummary->info(LHCb::RecSummary::nITClusters, -999);
  m_data.nTTClusters = rsummary->info(LHCb::RecSummary::nTTClusters, -999);
  m_data.nUTClusters = rsummary->info(LHCb::RecSummary::nUTClusters, -999);
  m_data.nOTClusters = rsummary->info(LHCb::RecSummary::nOTClusters, -999);
  m_data.nFTClusters = rsummary->info(LHCb::RecSummary::nFTClusters, -999);
  m_data.nSPDhits = rsummary->info(LHCb::RecSummary::nSPDhits, -999);
  m_data.nMuonCoordsS0 = rsummary->info(LHCb::RecSummary::nMuonCoordsS0, -999);
  m_data.nMuonCoordsS1 = rsummary->info(LHCb::RecSummary::nMuonCoordsS1, -999);
  m_data.nMuonCoordsS2 = rsummary->info(LHCb::RecSummary::nMuonCoordsS2, -999);
  m_data.nMuonCoordsS3 = rsummary->info(LHCb::RecSummary::nMuonCoordsS3, -999);
  m_data.nMuonCoordsS4 = rsummary->info(LHCb::RecSummary::nMuonCoordsS4, -999);
  m_data.nMuonTracks = rsummary->info(LHCb::RecSummary::nMuonTracks, -999);

  LHCb::HltDecReports *rreports = get<LHCb::HltDecReports>("Strip/Phys/DecReports");
  m_data.stripping_lines_dict = stripping2dict(rreports);

  if (m_tree) m_tree->Fill();

  return StatusCode::SUCCESS;
}

namespace {
  inline const StatusCode & operator&&(const StatusCode& lhs, const StatusCode& rhs) {
    if (lhs.isFailure()) {
      rhs.ignore();
      return lhs;
    } else {
      return rhs;
    }
  }
}
// ============================================================================
// Finalize
// ============================================================================
StatusCode EventIndexer::finalize() {
  DEBMSG << "==> Finalize" << endmsg;

  VERMSG << "Flush the TTree" << endmsg;
  if (m_tree) {
    m_file->cd(); // ensure that we write to the right file
    m_tree->Write();
  }

  VERMSG << "Close the output file" << endmsg;
  if (m_file) {
    m_file->Close();
    delete m_file;
    m_file = 0;
    m_tree = 0; // the TTree is automatically deleted when the file is closed
  }

  m_incSvc.reset();

  return GaudiAlgorithm::finalize(); // must be called after all other actions
}

// ============================================================================

void EventIndexer::handle(const Incident& incident) {
  // set the current input file
  m_data.lfn = incident.source();

  VERMSG << "New input file opened: " << m_data.lfn << endmsg;
  // reset the position counter
  m_data.position = 0;
  std::string::size_type end = m_data.lfn.rfind('.');
  if (end == std::string::npos){
    m_data.stream = "";
  } else {
    std::string::size_type begin = m_data.lfn.rfind('.', end - 1);
    if (begin == std::string::npos){
      m_data.stream = "";
    } else {
      ++begin;
      m_data.stream = m_data.lfn.substr(begin, end - begin);
      VERMSG << "Stream name: " << m_data.stream << endmsg;
    }
  }
}
