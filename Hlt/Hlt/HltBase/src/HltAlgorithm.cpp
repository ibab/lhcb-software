// $Id: HltAlgorithm.cpp,v 1.22 2007-11-19 14:57:03 graven Exp $
// Include files 

// from boost
#include <boost/functional/hash.hpp>
#include <boost/lexical_cast.hpp>

// from Gaudi
#include "GaudiKernel/IUpdateManagerSvc.h"

// local
#include "HltBase/HltAlgorithm.h"

#include "HltBase/ESequences.h"
#include "Event/HltNames.h"
#include "HltBase/HltConfigurationHelper.h"

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
  declareProperty("DataSummaryLocation",
                  m_dataSummaryLocation = LHCb::HltSummaryLocation::Default);

  declareProperty("SelectionName", m_selectionName = "");
  declareProperty("IsTrigger", m_isTrigger = false);

  // location of the input tracks, primary vertices and vertices
  declareProperty("InputTracksName",     m_inputTracksName = "");
  declareProperty("InputTracks2Name",    m_inputTracks2Name = "");
  declareProperty("InputVerticesName",   m_inputVerticesName = "");
  declareProperty("PrimaryVerticesName", m_primaryVerticesName = "");

  // output location for tracks and vertices
  declareProperty("OutputTracksName",    m_outputTracksName = "");
  declareProperty("OutputVerticesName",  m_outputVerticesName = "");

  declareProperty("MinCandidates",m_minNCandidates = 1);

  m_consider1 = false;
  m_consider2 = false;
  m_selectionID = 0;
  m_outputHolder = 0;
  m_outputTracks = 0;
  m_outputVertices = 0;
  m_inputSelections.clear();
  
}
//=============================================================================
// Destructor
//=============================================================================
HltAlgorithm::~HltAlgorithm() {

} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltAlgorithm::initialize() {
  StatusCode sc = HltBaseAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  m_hltSvc = 0;
  sc = serviceLocator()->service("HltDataSvc", m_hltSvc);
  if (sc.isFailure() || !m_hltSvc) 
    error() << " not able to create HltData Provider " << endreq;

  m_datasummary = 0;
  hltretrieve(m_datasummary,m_dataSummaryLocation);
  if (m_datasummary == 0) 
    fatal() << " not able to hltretrieve summary data " << endreq;

  m_conf = 0;
  hltretrieve(m_conf,m_dataSummaryLocation+"/Configuration");
  if (m_conf == 0) 
    fatal() << " not able to hltretrieve hlt configuration " << endreq;

  bool ok = true;
  ok = initContainers();
  if (!ok) return stop(" problems with Containers ");

  initCounters();

  initHistograms();

  saveConfiguration();

  //  infoConfiguration();

  return StatusCode::SUCCESS;

}


bool HltAlgorithm::initContainers() {

  m_inputTracks = 0; m_inputTracks2 = 0;
  sumretrieve(m_inputTracks,m_inputTracksName);
  sumretrieve(m_inputTracks2,m_inputTracks2Name);

  m_inputVertices = 0; m_primaryVertices = 0;
  sumretrieve(m_primaryVertices,m_primaryVerticesName);
  sumretrieve(m_inputVertices,m_inputVerticesName);

  if (m_selectionName != "") sumregister(m_selectionName);

  m_outputTracks = 0; m_outputVertices = 0;
  sumregister(m_outputVertices,m_outputVerticesName);
  sumregister(m_outputTracks,m_outputTracksName);

  
  infoContainer(m_inputTracks,   " input tracks ",   m_inputTracksName);
  infoContainer(m_inputTracks2,  " input tracks2 ", m_inputTracks2Name);
  infoContainer(m_inputVertices, " input vertices ", m_inputVerticesName);
  infoContainer(m_outputTracks,  " output tracks ",   m_outputTracksName);
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

  // Output
  initializeHisto(m_histoCandidates ,   "Candidates");
  if (m_outputVertices) initializeHisto(m_histoOutputVertices,"OutputVertices");
  if (m_outputTracks)   initializeHisto(m_histoOutputTracks,  "OutputTracks");
}

void HltAlgorithm::saveConfiguration() {
  
  if (m_selectionName == "") {
    warning() << " Not able to store hlt configuration "
              << name() << " missing selection name! " << endreq;
    return;
  }

  std::string mykey = name()+"/Selection";
  m_conf->add(mykey,m_selectionName);
  info() << " HLT ["<<mykey<<"] = "
         << m_conf->retrieve<std::string>(mykey) << endreq;
  
  confregister("Algo",name());

  if (m_inputTracks) 
    confregister("InputTracksName",m_inputTracksName);
  if (m_inputTracks2) 
    confregister("InputTracks2Name",m_inputTracks2Name);
  if (m_inputVertices) 
    confregister("InputVerticesName",m_inputVerticesName);
  if (m_primaryVertices) 
    confregister("PrimaryVerticesName",m_primaryVerticesName);
  
  if (m_outputTracks) {
    confregister("OutputTracksName",m_outputTracksName);
    confregister("SelectionType",std::string("Tracks"));
  }
  if (m_outputVertices) {
    confregister("OutputVerticesName",m_outputVerticesName);
    confregister("SelectionType",std::string("Vertices"));
  }
  if ((m_outputTracks) && (m_outputVertices))
    error() << " Output tracks and vertices it is not allowed! " << endreq;
  
  if (m_inputSelections.size()>0)
    confregister("InputSelections",m_inputSelections);
  
}

bool HltAlgorithm::beginExecute() {

  bool ok = HltBaseAlg::beginExecute();
  if (!ok) return ok;

  // verbose() << " entering HltAlgorithm beginExecute " << endreq;

  if (m_outputTracks) m_outputTracks->clear();
  if (m_outputVertices) m_outputVertices->clear();

  if (!m_filter)
    selectionSummary().setDecisionType(HltEnums::Forced, true);

  // verbose() << " histograming inputs " << endreq;

  bool ok1,ok2;

  // must check that the producer of input tracks was actually run and succeeded
  if( m_consider1 && m_inputTracks ){
    std::string::size_type s = m_inputTracksName.rfind('/');
    std::string selname = (s!=std::string::npos ? m_inputTracksName.substr(s+1)
                                                : m_inputTracksName );
    int id = HltConfigurationHelper::getID(*m_conf,"SelectionID",selname);
    if( m_datasummary->hasSelectionSummary(id) ){
      if( ! selectionSummary(id).decision() ){
        m_inputTracks->clear();
      }
    } else {
      m_inputTracks->clear();
    }
  }
  ok1 = size(m_inputTracks,m_nInputTracks,m_histoInputTracks,
            " input tracks ");

  if (!(ok1 || m_consider1)) return ok1;

  // must check that the producer of input tracks was actually run and succeeded
  if( m_consider2 && m_inputTracks2 ){
    std::string::size_type s = m_inputTracks2Name.rfind('/');
    std::string selname = (s!=std::string::npos ? m_inputTracks2Name.substr(s+1)
                                                : m_inputTracks2Name );
    int id = HltConfigurationHelper::getID(*m_conf,"SelectionID",selname);
    if( m_datasummary->hasSelectionSummary(id) ){
      if( ! selectionSummary(id).decision() ){
        m_inputTracks2->clear();
      }
    } else {
      m_inputTracks2->clear();
    }
  }
  ok2 = size(m_inputTracks2,m_nInputTracks2,m_histoInputTracks2,
            " input tracks 2 ");
  if (!(ok2 || m_consider2)) return ok2;

  if( m_consider1 && !(ok1||ok2) )return false;

  ok = size(m_primaryVertices,m_nPrimaryVertices,m_histoInputPVs,
            " input primary vertices ");
  if (!ok) return ok;
  
  ok = size(m_inputVertices,m_nInputVertices,m_histoInputVertices,
            " input vertices ");
  if (!ok) return ok;


  increaseCounter(m_counterInput );

  return ok;
}

bool HltAlgorithm::endExecute() {
  bool ok = true;

  fillHisto(m_histoCandidates, m_nCandidates, 1.);

  ok = size(m_outputTracks,m_nOutputTracks,m_histoOutputTracks,
            " output tracks ");
  if (!ok) return ok;

  ok = size(m_outputVertices,m_nOutputVertices,m_histoOutputVertices,
            " output vertices ");
  if (!ok) return ok;

  if (m_outputHolder) 
    candidateFound(m_outputHolder->size());

  debug() << " output candidates " << m_nCandidates << endreq;
  if (m_nCandidates >= m_minNCandidates) {
    increaseCounter(m_counterAccepted);
    setDecision(true);
  }
  
  ok = HltBaseAlg::endExecute();
  return ok;
}

void HltAlgorithm::setDecision(bool ok) {
  HltBaseAlg::setDecision(ok);
  if (m_filter) {
    selectionSummary().setDecision(ok);
    debug() << " set decision " << selectionSummary().decision()
            << " at selection " << m_selectionID << endreq;
  }
}

void HltAlgorithm::setDecisionType(int decisionType) {
  selectionSummary().setDecisionType(decisionType,true);
  if (m_isTrigger) m_datasummary->setDecisionType(decisionType,true);
  setDecision(true);
}

////  Finalize
StatusCode HltAlgorithm::finalize() {

  StatusCode sc =  HltBaseAlg::finalize();
  infoSubsetEvents  ( m_counterAccepted , 
                      m_counterInput,  " accepted/inputs ");  
  if (m_passPeriod >0) 
    infoSubsetEvents  ( m_counterPassed , 
                        m_counterInput,  " passed/inputs ");

  delete m_outputHolder;
  delete m_outputTracks;
  delete m_outputVertices;
  return sc;
  
}

void HltAlgorithm::printInfo(const std::string& title,
                             const Track& track) {
  info() << title << " track  " << track.key() << " slopes " 
         << track.slopes()  << " pt " << track.pt() 
         << " qop " << track.firstState().qOverP() << endreq
         << endreq;
}

void HltAlgorithm::printInfo(const std::string& title,
                             const RecVertex& vertex) {
  info() << title << " vertex  " << vertex.key() << " position " 
         << vertex.position()  << endreq;
  printInfo(title,vertex.tracks());
}

void HltAlgorithm::sumregister(const std::string& selname) {
  int id = HltConfigurationHelper::getID(*m_conf,"SelectionID",selname);
  if (m_datasummary->hasSelectionSummary(id))
    fatal() << " selection already registered! " << selname << endreq;
  else {
    m_selectionName = selname;
    m_selectionID = id;
  } 
}
