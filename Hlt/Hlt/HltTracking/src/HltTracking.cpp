// $Id: HltTracking.cpp,v 1.10 2007-02-06 08:43:37 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/IJobOptionsSvc.h"

// local
#include "Event/HltNames.h"
#include "HltTracking.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltTracking
//
// 2006-08-28 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltTracking );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltTracking::HltTracking( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
{

  declareProperty("MeasureTime"   , m_measureTime   = false );
  declareProperty("RecoName", m_recoName = "empty");
  declareProperty("OrderByPt", m_orderByPt = false);

  m_configs["Velo"] = RecoConfiguration("Velo","RZVelo","PatVeloSpaceTracking",
                                        LHCb::TrackLocation::RZVelo,
                                        LHCb::TrackLocation::HltVelo);
    
  m_configs["VeloTT"] = RecoConfiguration("VeloTT","Velo","PatVeloTT",
                                          LHCb::TrackLocation::HltVelo,
                                          LHCb::TrackLocation::HltVeloTT);
    
  m_configs["Forward"] = RecoConfiguration("Forward","Velo",
                                           "PatForward",
                                           LHCb::TrackLocation::HltVelo,
                                           LHCb::TrackLocation::HltForward);

  m_configs["MuonForward"] = RecoConfiguration("MuonForward","VeloMuon",
                                               "PatForward",
                                               "Hlt/Track/MuonVelo",
                                               "Hlt/Track/MuonForward");
  
}
//=============================================================================
// Destructor
//=============================================================================
HltTracking::~HltTracking() {} 

//=============================================================================// Initialization
//=============================================================================
StatusCode HltTracking::initialize() {

  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  m_doRest = (m_inputTracksName == "empty");

  // deal with timing
  if ( m_measureTime ) {
    m_timer = tool<ISequencerTimerTool>( "SequencerTimerTool" );
    m_timer->increaseIndent();
    m_timeFlag = m_timer->addTimer( "Marck Tracks" );
    m_timePat  = m_timer->addTimer( "Pat Reco" );   
    m_timeLoad  = m_timer->addTimer( "Load Tracks" );
    m_timer->decreaseIndent();
  }

  // create the internal reconstruction algorithms and set properties
  //-------------------------------------------------------------------
  if (m_configs.find(m_recoName) == m_configs.end())
    return Error( " No configuration for reconstruction named "+m_recoName );

  sc = iniRecoAlgorithm();
  if ( sc.isFailure() ) return sc;  // error printed already by iniRecoAlgorithm

  // get the Pat tracks that uses the reconstruction algorithm  
  //----------------------------------------------------------
  std::string patInputTracksName  = m_configs[m_recoName].inputTracksName;
  std::string patOutputTracksName = m_configs[m_recoName].outputTracksName;

  m_patInputTracks  = m_patDataStore->tracks(patInputTracksName);
  m_patOutputTracks = m_patDataStore->tracks(patOutputTracksName);

  infoContainer(m_patInputTracks,  " pat  input tracks", patInputTracksName);
  infoContainer(m_patOutputTracks, " pat output tracks", patOutputTracksName);

  initializeHisto(m_histoPatInputTracks,"PatInputTracks");
  initializeHisto(m_histoPatOutputTracks,"PatOutputTracks");

  return StatusCode::SUCCESS;

};

StatusCode HltTracking::iniRecoAlgorithm() 
{
 
  std::string algoType = m_configs[m_recoName].type;
  std::string algoName = "Pat"+name();  

  std::string recoName     = m_configs[m_recoName].reco+"Key";
  std::string prevrecoName = m_configs[m_recoName].prevreco+"Key";
 
  m_recoKey     = HltNames::particleInfoID( recoName );
  m_prevrecoKey = HltNames::particleInfoID( prevrecoName );
  m_recoHasPt   = HltNames::particleInfoID( "VeloTTKey" );

  IAlgManager* appMgr;
  StatusCode sc = service( "ApplicationMgr", appMgr );
  if ( !sc.isSuccess() )  return Error( "Unable to locate the ApplicationMgr" );

  IJobOptionsSvc* optSvc = svc<IJobOptionsSvc>( "JobOptionsSvc" );

  StringProperty propertySelectorName = 
    StringProperty("TrackSelectorName","HltTrackSelector");
  optSvc->addPropertyToCatalogue( algoName, propertySelectorName);

  StringProperty propertyInputTracksName = 
    StringProperty("InputTracksName",m_configs[m_recoName].inputTracksName);
  optSvc->addPropertyToCatalogue( algoName, propertyInputTracksName);

  StringProperty propertyOutputTracksName = 
    StringProperty("OutputTracksName",m_configs[m_recoName].outputTracksName);
  optSvc->addPropertyToCatalogue( algoName, propertyOutputTracksName);

  
  // debug properties of internal reconstruction algorithm
  const std::vector< const Property*>* props = optSvc->getProperties(algoName);
  for (std::vector<const Property*>::const_iterator itp = props->begin();
       itp != props->end(); ++itp)
    debug() << " added property " << (*itp)->name() 
            << " to algorithm named " << algoName << endreq;
  release(optSvc);
  

  sc = appMgr->createAlgorithm(algoType, algoName, m_algo);
  if (!sc.isSuccess())
    return Error( "Unable to create algorithm named " + algoName + 
                  " of type " + algoType );
  
  sc = m_algo->sysInitialize();
  if (!sc.isSuccess())
    return Error( "Unable to initialize algorithm named " + algoName + 
                  " of type " + algoType );
  appMgr->release();
  
  debug() << " created reco algorithm named " << algoName << " of type "
          << algoType << endreq;

  debug() << "          reco Key " << recoName << " = " << m_recoKey << endreq;
  debug() << " previous reco Key " << prevrecoName 
          << " = " << m_prevrecoKey << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltTracking::execute() {

  bool ok = HltAlgorithm::beginExecute();
  if (!ok) return stop( " No input tracks ");

  flag();
  if (m_measureTime) m_timer->start(m_timePat);
  StatusCode sc = m_algo->execute();
  if (m_measureTime) m_timer->stop(m_timePat);
  load();

  HltAlgorithm::endExecute();

  return sc;
  
}


void HltTracking::flag() {
  if (m_measureTime) m_timer->start(m_timeFlag);

  for (PatTrackContainer::iterator it = m_patInputTracks->begin();
       it != m_patInputTracks->end(); ++it) 
    (*it)->setFlag(Track::IPSelected,false);

  if (m_inputTracks) markTracks(*m_inputTracks);
  else markTracks(*m_patInputTracks);

  if (m_measureTime) m_timer->stop(m_timeFlag);
  
  debug() << " pat input      tracks " << m_patInputTracks->size() << endreq;
  if (m_inputTracks)
    debug() << "     input      tracks " << m_inputTracks->size() << endreq;
}


void HltTracking::loadFrom(const Track& mon) {
  int keymon = mon.key();
  for (PatTrackContainer::iterator it = m_patOutputTracks->begin();
       it != m_patOutputTracks->end(); ++it) {
    if ((*it)->nStates() <=0) continue; // TODO protection to bugy tracks
    Track& son = **it;
    bool ok = false;
    int ikeymon = (int) son.info(m_prevrecoKey,-1);
    if (ikeymon == keymon) ok = true;
    else if (ikeymon == -1) ok = isAncestor(son,mon);
    if (ok) {
      son.addInfo(m_prevrecoKey, keymon);
      if (m_outputTracks) m_outputTracks->push_back( (Track*) &son );
    }
  }
}



void HltTracking::load() {
  if (m_measureTime) m_timer->start(m_timeLoad);

  if (m_inputTracks) loadTracks(*m_inputTracks);
  else loadTracks(*m_patInputTracks);
  
  if ((m_orderByPt) && (m_outputTracks) && (m_recoKey >= m_recoHasPt)) 
    std::sort(m_outputTracks->begin(),m_outputTracks->end(),_sortByPt);

  if (m_measureTime) m_timer->stop(m_timeLoad);
  if (m_debug) {
    debug() << " pat output tracks " << m_patOutputTracks->size() << endreq;
    if (m_outputTracks)
      debug() << "     output tracks " << m_outputTracks->size() << endreq;
  }
}


bool HltTracking::isAncestor(const Track& son, const Track& mother) {
  const SmartRefVector<Track>& tracks = son.ancestors();
  for (SmartRefVector<Track>::const_iterator it = tracks.begin(); 
       it != tracks.end(); ++it) {
    const Track& imon = **it;
    if (  (Track*) (&imon) == (Track*) (&mother) ) return true;
  }
  return false;
}


//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltTracking::finalize() {

  debug() << "==> Finalize" << endmsg;

  m_algo->finalize();
  return HltAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
