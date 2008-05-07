// $Id: HltAlgorithm.cpp,v 1.31 2008-05-07 11:36:40 graven Exp $
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
  , m_inputTracks(0)
  , m_inputTracks2(0)
  , m_inputVertices(0)
  , m_primaryVertices(0)
  , m_outputTracks(0)
  , m_outputVertices(0)
{
  
  declareProperty("PassPeriod", m_passPeriod = 0);  
  declareProperty("HistogramUpdatePeriod" , m_histogramUpdatePeriod = 0 );
  declareProperty("SaveConfiguration", m_saveConf = true);

  declareProperty("ConsiderInputs",m_considerInputs = true);  
  declareProperty("MinCandidates",m_minNCandidates = 1);

  declareProperty("InputSelection", m_inputSelectionName = "");
  declareProperty("InputSelection2", m_inputSelection2Name = "");
  declareProperty("InputSelections", m_extraInputSelectionsNames);
  declareProperty("OutputSelection", m_outputSelectionName = name);

  m_inputSelectionsNames.clear();
  m_inputSelections.clear();
  m_outputSelection = 0;


}

HltAlgorithm::~HltAlgorithm() {

  if (!useTES())
    if (m_outputSelection) delete m_outputSelection;
} 


StatusCode HltAlgorithm::initialize() {
  StatusCode sc = HltBaseAlg::initialize(); 
  if ( sc.isFailure() ) return sc;

  initCounters();

  verbose() << "Initialised HltAlgorithms" << endmsg ;
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



void HltAlgorithm::saveConfiguration() {
  
  if (!m_saveConf) return;
  verbose() << "Saving Config " << m_outputSelectionName << endmsg ;
  if ( m_outputSelection == 0 ) {
    warning() << "m_outputSelection is NULL. Not saving config." << endmsg ;
    return ;
  }
  
  verbose() << " classID: " << m_outputSelection->classID() << endmsg ;

  Assert(m_outputSelection != 0," No output Selection");
  std::string type = "unknown";
  if (m_outputSelection->classID() == LHCb::Track::classID()) type = "Track";
  else if (m_outputSelection->classID() == LHCb::RecVertex::classID())
    type = "Vertex";
  verbose() << "Type : " << type << endmsg ;
  
  std::string algoType =  System::typeinfoName(typeid(*this));

  confregister("Algorithm",algoType+"/"+name(),         m_outputSelectionName);
  confregister("SelectionType",type,                    m_outputSelectionName);
  confregister("InputSelections",m_inputSelectionsNames,m_outputSelectionName);
  verbose() << "Done saveConfigureation" << endmsg ;  
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
    fillHisto(*m_inputHistos[(*it)->id()],(*it)->ncandidates(),1.);
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
  debug() << "HltAlgorithm::finalize()" << endmsg;

  infoTotalEvents(m_counterEntries);
  infoSubsetEvents(m_counterAccepted, m_counterEntries, " passed/entries ");
  infoCandidates(m_counterCandidates, m_counterAccepted, " passed ");

  return HltBaseAlg::finalize();
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
