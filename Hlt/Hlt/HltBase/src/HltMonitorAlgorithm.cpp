// $Id: HltMonitorAlgorithm.cpp,v 1.3 2007-02-08 17:32:39 hernando Exp $
// Include files

// local
#include "HltBase/HltMonitorAlgorithm.h"
#include "HltBase/HltUtils.h"
#include "HltBase/ESequences.h"
#include "Event/HltNames.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltMonitorAlgorithm
//
// 2006-06-15 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltMonitorAlgorithm::HltMonitorAlgorithm( const std::string& name,
                                          ISvcLocator* pSvcLocator)
  : HltBaseAlg ( name , pSvcLocator )
{
  
  // location of the summary and the summary box name
  declareProperty("SummaryName",
                  m_summaryName = LHCb::HltSummaryLocation::Default);
  declareProperty("SelectionsName", m_selectionsName);
  declareProperty("DecisionTypesName", m_decisionTypesName);
  
  m_histogramUpdatePeriod = 1;
  m_passPeriod = 1;
}
//=============================================================================
// Destructor
//=============================================================================
HltMonitorAlgorithm::~HltMonitorAlgorithm() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltMonitorAlgorithm::initialize() {
  StatusCode sc = HltBaseAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  const std::vector<std::string> sels = m_selectionsName.value();
  for (std::vector<std::string>::const_iterator it = sels.begin();
       it != sels.end(); it++) {
    int id = HltNames::selectionSummaryID(*it);
    if (id<=0) warning() << " No valid selection summary " << *it << endreq;
    else m_selectionsIDs.push_back(id);
  }

  const std::vector<std::string> decs = m_decisionTypesName.value();
  for (std::vector<std::string>::const_iterator it = decs.begin();
       it != decs.end(); it++) {
    int id = HltNames::decisionTypeID(*it);
    if (id<=0) warning() << " No valid decision type " << *it << endreq;
    else m_selectionsIDs.push_back(id);
  }
  
    
  
  initCounters();

  debug() << "==> Initialize" << endreq;
  return StatusCode::SUCCESS;
  
}

void HltMonitorAlgorithm::initCounters() 
{

  initializeCounter(m_counterInput,    "Input");
  initializeCounter(m_counterAccepted ,     "Accepted");  
  
}


//// Main execution
StatusCode HltMonitorAlgorithm::execute() {

  StatusCode sc = StatusCode::SUCCESS;
  
  bool ok = beginExecute();
  if (!ok) return sc;
  
  endExecute();
  
  return sc;
}


bool HltMonitorAlgorithm::beginExecute() {
  bool ok = HltBaseAlg::beginExecute();
  if (!ok) return ok;

  m_summary = NULL;
  summary();
  m_selectionSummary = NULL;
  
  increaseCounter(m_counterInput );

  return ok;
}

bool HltMonitorAlgorithm::endExecute() {
  bool ok = true;

  increaseCounter(m_counterAccepted);

  ok = HltBaseAlg::endExecute();
  return ok;
}

////  Finalize
StatusCode HltMonitorAlgorithm::finalize() {

  infoSubsetEvents  ( m_counterPassed , m_counterInput,  " passed/inputs ");
  infoSubsetEvents  ( m_counterAccepted , m_counterInput,  " accepted/inputs ");  
  return HltBaseAlg::finalize();  
}


const HltSelectionSummary& HltMonitorAlgorithm::selectionSummary(int id) {
  if (!m_summary) summary();
  if (!m_summary->hasSelectionSummary(id))
    error() << " No selection summary with ID " << id << endreq;
  m_selectionSummary = &(m_summary->selectionSummary(id));
  return *m_selectionSummary;
}

bool HltMonitorAlgorithm::isInSelection(const Track& track, int id) {
  bool ok = false;
  if (!hasSelection(id)) return ok;
  std::vector<Track*> tracks;
  retrieveFromSummary(id,tracks);
  for (std::vector<Track*>::const_iterator it = tracks.begin(); 
       it != tracks.end(); it++) {
    const Track& otrack = *(*it);
    if (HltUtils::matchIDs(otrack,track)) ok = true;
  }
  return ok;
}
