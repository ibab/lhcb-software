// $Id: HltAlgorithm.cpp,v 1.26 2008-01-25 16:55:03 hernando Exp $
// Include files 

#include "HltBase/HltAlgorithm.h"

#include "HltBase/ESequences.h"
#include "HltBase/EParser.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltAlgorithm
//
// 2006-06-15 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------


HltAlgorithm::HltAlgorithm( const std::string& name,
                            ISvcLocator* pSvcLocator)
  : HltBaseAlg ( name , pSvcLocator )
{
  
  HltBaseAlg::create();

  declareProperty("PassPeriod", m_passPeriod = 0);  
  declareProperty("HistogramUpdatePeriod" , m_histogramUpdatePeriod = 0 );
  declareProperty("SaveConfiguration", m_saveConf = true);

  declareProperty("ConsiderInputs",m_considerInputs = true);  
  declareProperty("MinCandidates",m_minNCandidates = 1);

  declareProperty("InputSelection", m_inputSelectionName = "");
  declareProperty("InputSelection2", m_inputSelection2Name = "");
  declareProperty("InputSelections", m_extraInputSelectionsNames);
  declareProperty("OutputSelection", m_outputSelectionName = name);

  // for backward compatibility
  declareProperty("InputTracksName", m_inputSelectionName);
  declareProperty("InputVerticesName", m_inputSelectionName);
  declareProperty("InputTracks2Name", m_inputSelection2Name);
  declareProperty("PrimaryVerticesName", m_inputSelection2Name);
  declareProperty("OutputTracksName", m_outputSelectionName);
  declareProperty("OutputVerticesName", m_outputSelectionName);

  m_algoType = "Unknown";

  m_inputSelectionsNames.clear();
  m_inputSelections.clear();
  m_outputSelection = 0;

  // for backward compatibility
  m_doInitSelections = false;
  m_inputTracks = 0;
  m_inputTracks2 = 0;
  m_inputVertices = 0;
  m_primaryVertices = 0;

  m_outputTracks = 0;  
  m_outputVertices = 0;  
}

HltAlgorithm::~HltAlgorithm() {

  if (!useTES())
    if (m_outputSelection) delete m_outputSelection;
} 


StatusCode HltAlgorithm::initialize() {
  StatusCode sc = HltBaseAlg::initialize(); 
  if ( sc.isFailure() ) return sc;

  initCounters();

  if (m_doInitSelections) {
    initSelections();
    saveConfiguration();  
  }

  return StatusCode::SUCCESS;

}

void HltAlgorithm::initCounters() 
{

   // counters
  initializeCounter(m_counterEntries,    "Entries");
  initializeCounter(m_counterInputs,     "Inputs");
  initializeCounter(m_counterAccepted,   "Accepted Events");
  initializeCounter(m_counterPassed,     "Passed Events");
  initializeCounter(m_counterCandidates, "nCandidates");
}

void HltAlgorithm::initSelections() {

  info() << " init Selections " << endreq;
  
  // retrieving input selections
  // to be backward compatible, set the track,vertices input selections
  if (!m_doInitSelections) return;

  if (!m_inputSelectionName.empty()) {
    Hlt::Selection& sel = retrieveSelection(m_inputSelectionName);
    if (sel.classID() == LHCb::Track::classID())
      m_inputTracks = 
        &(retrieveTSelection<LHCb::Track>(m_inputSelectionName));
    else if (sel.classID() == LHCb::RecVertex::classID())
      m_inputVertices = 
        &(retrieveTSelection<LHCb::RecVertex>(m_inputSelectionName));
  }

  if (!m_inputSelection2Name.empty()) {
    Hlt::Selection& sel = retrieveSelection(m_inputSelection2Name);
    if (sel.classID() == LHCb::Track::classID())
      m_inputTracks2 = 
        &(retrieveTSelection<LHCb::Track>(m_inputSelection2Name));
    else if (sel.classID() == LHCb::RecVertex::classID())
      m_primaryVertices = 
        &(retrieveTSelection<LHCb::RecVertex>(m_inputSelection2Name));
  }

  const std::vector<std::string>& names =m_extraInputSelectionsNames.value();
  for (std::vector<std::string>::const_iterator it = names.begin();
       it != names.end(); ++it) {
    std::string name = (*it);
    Hlt::Selection& sel = retrieveSelection(name);
    if (sel.classID() == LHCb::Track::classID()) {
      Hlt::TrackSelection& sel = (retrieveTSelection<LHCb::Track>(name));
      if (!m_inputTracks) m_inputTracks = &sel;
      else m_inputTracks2 = &sel;
    } else if (sel.classID() == LHCb::RecVertex::classID()) {
      Hlt::VertexSelection& sel= (retrieveTSelection<LHCb::RecVertex>(name));
      if (!m_inputVertices) m_inputVertices = &sel;
      else m_primaryVertices = &sel;
    }
  }

  // create and ser the outputselection
  // to be backward compatible, set the type of the output selection
  // Track/Velo 
  
  Assert(!m_outputSelectionName.empty(), 
         "initSelections() HltAlgorithm without output selection!");

  std::string type = "";
  std::vector<std::string> values = 
    EParser::parse(m_outputSelectionName,"/");
  m_outputSelectionName = values.back();
  if (values.size() == 2) type = values[0];

  if (type.empty()) {
    debug() << " output selection name " << m_outputSelectionName
            << " without type: no selection selection created!" << endreq;
    return;
  }
  
  if (type == "Track") {
    m_outputTracks = &(registerTSelection<LHCb::Track>(m_outputSelectionName));
  } else if (type == "Vertex") {
    m_outputVertices = 
      &(registerTSelection<LHCb::RecVertex>(m_outputSelectionName));
  } else{
    Assert(0,"iniSelections() Unknown selection type "+type);
  }
}


void HltAlgorithm::saveConfiguration() {

  if (!m_saveConf) return;

  std::string type = "unknown";
  Assert(m_outputSelection != 0," No output Selection");
  if (m_outputSelection->classID() == LHCb::Track::classID()) type = "Track";
  else if (m_outputSelection->classID() == LHCb::RecVertex::classID())
    type = "Vertex";
  confregister("Algorithm",m_algoType+"/"+name(),m_outputSelectionName);
  confregister("SelectionType",type,m_outputSelectionName);
  confregister("InputSelections",m_inputSelectionsNames,m_outputSelectionName);
  
  info() << " HLT input selections " << m_inputSelectionsNames << endreq;
  info() << " HLT output selection " << m_outputSelectionName << endreq;
}


StatusCode HltAlgorithm::sysExecute() {

  StatusCode sc = StatusCode::SUCCESS;
  
  // verbose() << " sys execute HltBaseAlg beginExecute() " << endreq;
  sc = beginExecute();
  if (sc.isFailure()) return sc;
  
  // verbose() << " sys execute DefAlgo " << endreq;
  sc = HltBaseAlg::sysExecute();
  if (sc.isFailure()) return sc;
  
  // verbose() << " sys execute HltBaseAlg endExecute() " << endreq;
  sc = endExecute();
  
  return sc;

}

StatusCode HltAlgorithm::beginExecute() {

  // verbose() << " entering beginExecute " << endreq;

  StatusCode sc = StatusCode::SUCCESS;
  setDecision(false);
  
  increaseCounter( m_counterEntries );

  m_filter =( m_passPeriod <= 0? true:
              ( !(m_counterEntries % m_passPeriod) == 0 ) );
  
  m_monitor =( m_histogramUpdatePeriod <= 0? false:
               ( (m_counterEntries % m_histogramUpdatePeriod) == 0 ) );
  
  verbose() << " filter? " << m_filter 
            << " monitor? " << m_monitor << endreq;
  
  if (useTES()) {
    debug() << " cleaning from the TES " << endreq;
    resetHltData();
    m_inputSelections.clear();
    m_outputSelection = 0;
    return StatusCode::SUCCESS;
  }

  // do work if we not use the TES for selections
  Assert( m_outputSelection != 0," beginExecute() no output selection !");
  m_outputSelection->clean();
  bool ok = true;
  if (m_considerInputs) ok = considerInputs();
  if (m_monitor) monitorInputs();

  if (ok) increaseCounter(m_counterInputs);
  else sc = StatusCode::FAILURE;
  
  return sc;
}


bool HltAlgorithm::considerInputs() 
{
  if (!m_considerInputs) return true;
  bool ok = true;
  size_t i = 0;
  for (std::vector<Hlt::Selection*>::iterator it = m_inputSelections.begin();
       it != m_inputSelections.end(); ++it, ++i) {
    ok = ok && (*it)->decision();
    if (m_debug) 
      debug() << " input selection " << m_inputSelectionsNames[i]
              << " decision " << (*it)->decision() 
              << " candidates " << (*it)->ncandidates() << endreq;
  }

  if (!ok) {
    warning() << " Empty input or false input selection " << endreq;
    return StatusCode::FAILURE;
  }
  // verbose() << " consider inputs " << ok <<endreq;
  return ok;
}

void HltAlgorithm::monitorInputs() 
{
  if (!m_monitor) return;
  for (std::vector<Hlt::Selection*>::iterator it = m_inputSelections.begin();
       it != m_inputSelections.end(); ++it) {
    int id = (*it)->id();
    size_t n = (*it)->ncandidates();
    fillHisto(*m_inputHistos[id],n,1.);
  }
  // verbose() << " end monitor inputs " <<endreq;
}

void HltAlgorithm::monitorOutput() {
  if (!m_monitor) return;
  size_t nCandidates = m_outputSelection->ncandidates();
  Assert( 0 != m_outputHisto," monitorOutput() no output histo ");
  fillHisto(*m_outputHisto,nCandidates,1.);
  // verbose() << " end monitor output " << nCandidates << endreq;
}

void HltAlgorithm::setDecision() {

  Assert (0 != m_outputSelection," setDecision() no output selection! ");
  
  size_t nCandidates = m_outputSelection->ncandidates();
  increaseCounter(m_counterCandidates,nCandidates);
  if (nCandidates >= m_minNCandidates) {
    increaseCounter(m_counterAccepted);
    setDecision(true);
  } 
  if (!m_filter) setDecision(true);
  if (filterPassed()) increaseCounter(m_counterPassed);
}

void HltAlgorithm::setDecision(bool ok) {
  setFilterPassed(ok);
  if (m_filter) m_outputSelection->setDecision(ok);
  
  // verbose() << " decision " << filterPassed() 
  //        << " in selection " << m_outputSelection->decision() << endreq;
}

StatusCode HltAlgorithm::endExecute() {
  StatusCode sc = StatusCode::SUCCESS;
  
  setDecision();
  if (m_monitor) monitorOutput();
  
  debug() << " output candidates " << m_outputSelection->ncandidates() 
          << " decision " << filterPassed() << endreq;
  
  return sc;
}

////  Finalize
StatusCode HltAlgorithm::finalize() {

  infoTotalEvents(m_counterEntries);
  infoSubsetEvents(m_counterAccepted, m_counterEntries, " passed/entries ");
  infoCandidates(m_counterCandidates, m_counterAccepted, " passed ");

  return StatusCode::SUCCESS;  
}


void HltAlgorithm::setInputSelection(Hlt::Selection& sel,
                                     const std::string& selname) {
  if (zen::find(m_inputSelectionsNames, selname)) {
    debug() << " selection already in input " << selname << endreq;
    return;
  }
  m_inputSelectionsNames.push_back(selname);
  if (m_histogramUpdatePeriod > 0) {
    int id = hltSelectionID(selname);
    Hlt::Histo* histo = initializeHisto(selname);
    bool has = (m_inputHistos.find(id) != m_inputHistos.end());
    Assert(!has,"setInputSelection() already input selection "+selname);
    m_inputHistos[id] = histo;
  }
  if (!useTES()) {
    m_inputSelections.push_back(&sel);
  }
  debug() << " Input selection " << selname << endreq;
}

void HltAlgorithm::setOutputSelection(Hlt::Selection* sel, 
                                      const std::string& selname) {
  m_outputSelectionName = selname;
  Assert( 0 != sel,"setOutputSelection() no output selection");
  if (m_histogramUpdatePeriod > 0) 
    m_outputHisto = initializeHisto(selname);
  if (!useTES()) {
    Assert(m_outputSelection == 0, 
           " setOutputSelection() already set output selection!");    
    m_outputSelection = sel;
    std::vector<int>& selids = sel->inputSelectionsIDs();
    for (Hlt::SelectionIterator it = m_inputSelections.begin();
         it != m_inputSelections.end(); ++it) 
      selids.push_back( (*it)->id() );
  }
  debug() << " Output selection " << selname << endreq;
}
