// Include files

// From Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Incident.h"

// From LHCb
#include "Event/RecHeader.h"

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

// ============================================================================
// Main execution
// ============================================================================
StatusCode EventIndexer::execute() {
  DEBMSG << "==> Execute" << endmsg;

  LHCb::RecHeader *rh = get<LHCb::RecHeader>(LHCb::RecHeaderLocation::Default);

  ++m_data.position; // increment the position counter (first event has got position == 1)
  m_data.eventNumber = rh->evtNumber();
  m_data.runNumber = rh->runNumber();

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
  if (m_tree)
    m_tree->Write();

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
      m_data.stream = m_data.lfn.substr(++begin, end);
    }
  }
}
