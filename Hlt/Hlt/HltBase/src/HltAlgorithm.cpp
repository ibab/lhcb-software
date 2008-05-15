// $Id: HltAlgorithm.cpp,v 1.32 2008-05-15 08:56:55 graven Exp $
// Include files 

#include "HltBase/HltAlgorithm.h"

#include "HltBase/ESequences.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltAlgorithm
//
// 2006-06-15 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------


HltAlgorithm::HltAlgorithm( const std::string& name,
                            ISvcLocator* pSvcLocator )
  : HltBaseAlg ( name , pSvcLocator )
  , m_outputSelectionName(name)
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

  declareProperty("InputSelection",  m_inputSelectionName );
  declareProperty("InputSelection2", m_inputSelection2Name );
  declareProperty("InputSelections", m_extraInputSelectionsNames);
  declareProperty("OutputSelection", m_outputSelectionName.property(), "The location the output is written to");
//                                     ->declareUpdateHandler( stringKey::updateHandler, &m_outputSelectionName );

  m_inputSelectionsNames.clear();
  m_inputSelections.clear();
  m_outputSelection = 0;


}

HltAlgorithm::~HltAlgorithm()
{
  if (!useTES()) delete m_outputSelection;
} 


StatusCode HltAlgorithm::sysInitialize() {
  // set up context such that tools can grab the algorithm...
  // kind of non-local, but kind of OK.
  Gaudi::Utils::AlgContext sentry( contextSvc(), this );
  return HltBaseAlg::sysInitialize();
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
  assert( m_outputSelectionName == m_outputSelection->id() );
  if ( m_outputSelection == 0 ) {
    warning() << "m_outputSelection is NULL. Not saving config." << endmsg ;
    return ;
  }
  
  verbose() << " classID: " << m_outputSelection->classID() << endmsg ;

  Assert(m_outputSelection != 0," No output Selection");

  std::string type =
     ( m_outputSelection->classID() == LHCb::Track::classID())    ?   "Track" :
       m_outputSelection->classID() == LHCb::RecVertex::classID() ?  "Vertex" :
       m_outputSelection->classID() == LHCb::Particle::classID() ?  "Particle":
                                                                    "unknown" ;
  verbose() << "Type : " << type << endmsg ;
  
  std::string algoType =  System::typeinfoName(typeid(*this));

  confregister("Algorithm",algoType+"/"+name(),         m_outputSelectionName.str());
  confregister("SelectionType",type,                    m_outputSelectionName.str());
  confregister("InputSelections",m_inputSelectionsNames,m_outputSelectionName.str());
  verbose() << "Done saveConfigureation" << endmsg ;  
  info() << " HLT input selections " << m_inputSelectionsNames << endreq;
  info() << " HLT output selection " << m_outputSelection->id() << endreq;
}


StatusCode HltAlgorithm::sysExecute() {

  StatusCode sc = StatusCode::SUCCESS;
  
  // set up context such that tools can grab the algorithm...
  // kind of non-local, but kind of OK.
  Gaudi::Utils::AlgContext sentry( contextSvc(), this );
  
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

  m_filter =( m_passPeriod <= 0 ? true:
              ( !(m_counterEntries % m_passPeriod) == 0 ) );
  
  m_monitor =( m_histogramUpdatePeriod <= 0 ? false:
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
    assert( m_inputSelectionsNames[i] == (*it)->id() );
    if (m_debug) 
      debug() << " input selection " << (*it)->id()
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
  bool accept = ( nCandidates >= m_minNCandidates );

  if (accept) increaseCounter(m_counterAccepted);
  if (!m_filter||accept) setDecision(true);
  // Assert( (accept || !m_filter) == filterPassed(), "filterPassed unexpected..");
		  
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


void HltAlgorithm::setInputSelection(Hlt::Selection& sel) {
  if (zen::find(m_inputSelectionsNames, sel.id())) {
    debug() << " selection already in input " << sel.id() << endreq;
    return;
  }
  m_inputSelectionsNames.push_back(sel.id());
  if (m_histogramUpdatePeriod > 0) {
    Hlt::Histo* histo = initializeHisto(sel.id().str());
    bool has = (m_inputHistos.find(sel.id()) != m_inputHistos.end());
    Assert(!has,"setInputSelection() already input selection "+sel.id().str());
    m_inputHistos[sel.id()] = histo;
  }
  if (!useTES()) m_inputSelections.push_back(&sel);
  debug() << " Input selection " << sel.id() << endreq;
}

void HltAlgorithm::setOutputSelection(Hlt::Selection* sel) {
  Assert(m_outputSelectionName == sel->id(), "inconsistent selection vs selectionName!");
  m_outputSelectionName = sel->id();
  Assert( 0 != sel,"setOutputSelection() no output selection");
  if (m_histogramUpdatePeriod > 0) m_outputHisto = initializeHisto(sel->id().str());
  if (!useTES()) {
    Assert(m_outputSelection == 0, " setOutputSelection() already set output selection!");    
    m_outputSelection = sel;
    sel->addInputSelectionIDs( m_inputSelections.begin(), m_inputSelections.end() );
  }
  debug() << " Output selection " << sel->id() << endreq;
}
