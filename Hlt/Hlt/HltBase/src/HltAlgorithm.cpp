// $Id: HltAlgorithm.cpp,v 1.8 2007-02-12 16:13:25 hernando Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/IUpdateManagerSvc.h"

// local
#include "HltBase/HltAlgorithm.h"
#include "HltBase/IHltDataStore.h"

#include "HltBase/ESequences.h"
#include "Event/HltNames.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltAlgorithm
//
// 2006-06-15 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltAlgorithm::HltAlgorithm( const std::string& name,
                            ISvcLocator* pSvcLocator)
  : HltBaseAlg ( name , pSvcLocator )
{
  
  // location of the summary and the summary box name
  declareProperty("SummaryLocation",
                  m_summaryLocation = LHCb::HltSummaryLocation::Default);
  declareProperty("SelectionName",
                  m_selectionName = "");
  declareProperty("IsTrigger", m_isTrigger = false);
  
  // location of the input tracks, primary vertices and vertices
  declareProperty("PatInputTracksName",     m_patInputTracksName = "");
  declareProperty("PatInputTracks2Name",     m_patInputTracks2Name = "");
  declareProperty("PatInputVerticesName",     m_patInputVerticesName = "");

  // location of the input tracks, primary vertices and vertices
  declareProperty("InputTracksName",     m_inputTracksName = "");
  declareProperty("InputTracks2Name",    m_inputTracks2Name = "");
  declareProperty("InputVerticesName",   m_inputVerticesName = "");
  declareProperty("PrimaryVerticesName", m_primaryVerticesName = "");

  // output location for tracks and vertices
  declareProperty("OutputTracksName",    m_outputTracksName = "");
  declareProperty("OutputverticesName",  m_outputVerticesName = "");

  m_selectionSummaryID = -1;

}
//=============================================================================
// Destructor
//=============================================================================
HltAlgorithm::~HltAlgorithm() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltAlgorithm::initialize() {
  StatusCode sc = HltBaseAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if (m_selectionName != "") {
    m_selectionSummaryID = 
      HltNames::selectionSummaryID(m_selectionName);
    info() << " selection summary Name " << m_selectionName 
           << "  ID " << m_selectionSummaryID << endreq;
  }
  
  bool ok = true;
  ok = initContainers();
  if (!ok) return stop(" problems with Containers ");

  initCounters();

  initHistograms();

  debug() << "==> Initialize" << endreq;
  return StatusCode::SUCCESS;

}

void HltAlgorithm::init(Hlt::TrackContainer*& con,
                        IHltDataStore*& store,
                        const std::string& name) {
  con = NULL;
  if (name != "") {
    con = &(store->tracks(name));
    debug() << " initialize container " << name << endreq;
  } 
}

void HltAlgorithm::init(Hlt::VertexContainer*& con,
                        IHltDataStore*& store, const std::string& name) {
  con = NULL;
  if (name != "") { con = &(store->vertices(name));
  debug() << " initialize container " << name << endreq;} 
}

void HltAlgorithm::init(PatTrackContainer*& con,
                        PatDataStore*& store,const std::string& name) {
  con = NULL;
  if (name != "") {con = store->tracks(name);
  debug() << " initialize pat container " << name << endreq;} 
}

void HltAlgorithm::init(PatVertexContainer*& con,
                        PatDataStore*& store, const std::string& name) {
  con = NULL;
  if (name != "") { con = store->vertices(name);
  debug() << " initialize pat container " << name << endreq;} 
}

bool HltAlgorithm::initContainers() {
  

  // from HltDataStore

  m_hltDataStore     = tool<IHltDataStore>( "HltDataStore" );
  if (!m_hltDataStore) return false;
  
  // get the input tracks and vertices containers 
  init(m_inputTracks,m_hltDataStore,m_inputTracksName);
  init(m_inputTracks2,m_hltDataStore,m_inputTracks2Name);

  init(m_primaryVertices,m_hltDataStore,m_primaryVerticesName);
  init(m_inputVertices,m_hltDataStore,m_inputVerticesName);

  init(m_outputTracks,m_hltDataStore,m_outputTracksName);
  init(m_outputVertices,m_hltDataStore,m_outputVerticesName);

  //  release(m_hltDataStore);  

  // from PatDataStore
  m_patDataStore = tool<PatDataStore>("PatDataStore");
  if (!m_hltDataStore) return false;
  
  init(m_patInputTracks,m_patDataStore,m_patInputTracksName);
  init(m_patInputTracks2,m_patDataStore,m_patInputTracks2Name);
  init(m_patInputVertices,m_patDataStore,m_patInputVerticesName);
  
  // release(patstore);
  

  // info of using containers...
  infoContainer(m_patInputTracks," pat input tracks ",m_patInputTracksName);
  infoContainer(m_patInputTracks2," pat input tracks 2",m_patInputTracks2Name);
  infoContainer(m_patInputVertices," pat input vertices ",
                m_patInputVerticesName);
  
  infoContainer(m_inputTracks,   " input tracks ",   m_inputTracksName);
  infoContainer(m_inputTracks2,  " input tracks 2 ", m_inputTracks2Name);
  infoContainer(m_inputVertices, " input vertices ", m_inputVerticesName);
  infoContainer(m_outputTracks,  " output track ",   m_outputTracksName);
  infoContainer(m_outputVertices," output vertices ",m_outputVerticesName);

  return true;
}

void HltAlgorithm::initCounters() 
{

  initializeCounter(m_counterInput,    "Input");
  initializeCounter(m_counterAccepted ,     "Accepted");  
  
}

void HltAlgorithm::initHistograms() {

  if (m_histogramUpdatePeriod == 0) return;

  // Input
  if (m_primaryVertices)
    initializeHisto(m_histoInputPVs,      "PrimaryVertices");
  if (m_inputVertices)
    initializeHisto(m_histoInputVertices, "InputVertices");
  if (m_inputTracks)
    initializeHisto(m_histoInputTracks,   "InputTracks");
  if (m_inputTracks2)
    initializeHisto(m_histoInputTracks2,  "InputTracks2");

  if (m_patInputTracks)
    initializeHisto(m_histoPatInputTracks, "PatInputTracks");
  if (m_patInputTracks2)
    initializeHisto(m_histoPatInputTracks2, "PatInputTracks2");
  if (m_patInputVertices)
    initializeHisto(m_histoPatInputVertices, "PatInputVertices");

  // Output
  initializeHisto(m_histoCandidates ,   "Candidates");
  if (m_outputVertices)
    initializeHisto(m_histoOutputVertices,"OutputVertices");
  if (m_outputTracks)
    initializeHisto(m_histoOutputTracks,  "OutputTracks");
}


//// Main execution
StatusCode HltAlgorithm::execute() {

  StatusCode sc = StatusCode::SUCCESS;
  
  bool ok = beginExecute();
  if (!ok) return sc;
  
  endExecute();
  
  return sc;
}


bool HltAlgorithm::beginExecute() {
  bool ok = HltBaseAlg::beginExecute();
  if (!ok) return ok;

  m_summary = NULL;
  m_selectionSummary = NULL;
  if (m_selectionSummaryID>0) {
    selectionSummary();
    if (!m_filter) m_selectionSummary->setDecisionType(HltEnums::Forced, true);
  }
  
  ok = size(m_inputTracks,m_nInputTracks,m_histoInputTracks,
            " input tracks ");
  if (!ok) return ok;

  ok = size(m_inputTracks2,m_nInputTracks2,m_histoInputTracks2,
            " input tracks 2 ");
  if (!ok) return ok;

  ok = size(m_patInputTracks,m_nPatInputTracks,m_histoPatInputTracks,
            " pat input tracks ");
  if (!ok) return ok;

  ok = size(m_patInputTracks2,m_nPatInputTracks2,m_histoPatInputTracks2,
            " pat input tracks 2");
  if (!ok) return ok;

  ok = size(m_primaryVertices,m_nPrimaryVertices,m_histoInputPVs,
            " input primary vertices ");
  if (!ok) return ok;

  ok = size(m_inputVertices,m_nInputVertices,m_histoInputVertices,
            " input vertices ");
  if (!ok) return ok;

  ok = size(m_patInputVertices,m_nPatInputVertices,m_histoPatInputVertices,
            " pat input vertices ");
  if (!ok) return ok;

  increaseCounter(m_counterInput );

  // Increase input counters
  // increaseCounter( m_countNonEmpty );
  // increaseCounter( m_countInputVertices, m_nInputVertices );
  // increaseCounter( m_countInputPVs,      m_nPrimaryVertices );
  // increaseCounter( m_countInputTracks,   m_nInputTracks );
  // increaseCounter( m_countInputTracks2,  m_nInputTracks2 );

  return ok;
}

bool HltAlgorithm::endExecute() {
  bool ok = true;

  // fillHisto(m_histoCandidates, m_nCandidates, 1.);

  ok = size(m_outputTracks,m_nOutputTracks,m_histoOutputTracks,
            " output tracks ");
  if (!ok) return ok;

  ok = size(m_outputVertices,m_nOutputVertices,m_histoOutputVertices,
            " output vertices ");
  if (!ok) return ok;

  increaseCounter(m_counterAccepted);

  ok = HltBaseAlg::endExecute();
  return ok;
}

////  Finalize
StatusCode HltAlgorithm::finalize() {

  StatusCode sc =  HltBaseAlg::finalize();
  infoSubsetEvents  ( m_counterAccepted , m_counterInput,  " accepted/inputs ");  
  if (m_passPeriod >0) 
    infoSubsetEvents  ( m_counterPassed , m_counterInput,  " passed/inputs ");

  return sc;
  
}


HltSelectionSummary& HltAlgorithm::selectionSummary(int id) {
  if (!m_summary) summary();
  if (id>0) return m_summary->selectionSummary(id);
  id = m_selectionSummaryID;
  if (!m_selectionSummary)
    debug() << " retrieving selection summary id " << id << endreq;
  m_selectionSummary = &(m_summary->selectionSummary(id));
  return *m_selectionSummary;
}
