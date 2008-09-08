// $Id: HltTrackMatch.cpp,v 1.11 2008-09-08 10:14:22 graven Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "boost/foreach.hpp"

// local
#include "HltTrackMatch.h"
#include "HltBase/HltUtils.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltTrackMatch
//
// 2007-12-07 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltTrackMatch );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltTrackMatch::HltTrackMatch( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
  , m_selections(*this)
  , m_tool(0)
  , m_qualityID(0)
  , m_quality2ID(0)
{
  declareProperty("MatchName",m_matchName = "");
  
  declareProperty("MaxQuality",m_maxQuality = 1e6);
  declareProperty("MaxQuality2",m_maxQuality2 = 1e6);

  m_selections.declareProperties();

  m_TESOutput = "";
  m_qualityName = "";
  m_quality2Name = "";
}
//=============================================================================
// Destructor
//=============================================================================
HltTrackMatch::~HltTrackMatch() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltTrackMatch::initialize() {
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by HltAlgorithm

  debug() << "==> Initialize" << endmsg;

  m_selections.retrieveSelections();
  m_selections.registerSelection();
  
  recoConfiguration();
  
  sc = setReco(m_matchName);
  if ( sc.isFailure() ) return sc;
  
  saveConfiguration();

  return StatusCode::SUCCESS;
}

void HltTrackMatch::recoConfiguration() {
  // configure
  m_recoConf.add("VeloCalo/Tool",std::string("HltVeloTCaloMatch"));
  m_recoConf.add("VeloCalo/RecoID", (int) 100); //TODO
  m_recoConf.add("VeloCalo/TransferIDs",true);
  m_recoConf.add("VeloCalo/TransferAncestor",true);
  m_recoConf.add("VeloCalo/TransferInfo",false);
  m_recoConf.add("VeloCalo/TrackType", (int) LHCb::Track::Velo);
  m_recoConf.add("VeloCalo/TESOutput", std::string("Hlt/Track/VeloCalo"));
  m_recoConf.add("VeloCalo/Quality",std::string("VeloCalo3DChi2"));
  m_recoConf.add("VeloCalo/Quality2",std::string(""));
  
  m_recoConf.add("VeloTDist/Tool",std::string("HltMatchTVeloTracks"));
  m_recoConf.add("VeloTDist/RecoID", (int) 101); //TODO
  m_recoConf.add("VeloTDist/TransferIDs",true);
  m_recoConf.add("VeloTDist/TransferAncestor",true);
  m_recoConf.add("VeloTDist/TransferInfo",false);
  m_recoConf.add("VeloTDist/TrackType", (int) LHCb::Track::Long);
  m_recoConf.add("VeloTDist/TESOutput", std::string("Hlt/Track/VeloTDist"));
  m_recoConf.add("VeloTDist/Quality",std::string("deltaX"));
  m_recoConf.add("VeloTDist/Quality2",std::string("deltaY"));

  m_recoConf.add("VeloT/Tool",std::string("PatMatchTool"));
  m_recoConf.add("VeloT/RecoID", (int) 102); //TODO
  m_recoConf.add("VeloT/TransferIDs",true);
  m_recoConf.add("VeloT/TransferAncestor",true);
  m_recoConf.add("VeloT/TransferInfo",true);
  m_recoConf.add("VeloT/TrackType", (int) LHCb::Track::Long);
  m_recoConf.add("VeloT/TESOutput", std::string("Hlt/Track/VeloT")); //TODO
  m_recoConf.add("VeloT/Quality",std::string("chi2_PatMatch"));
  m_recoConf.add("VeloT/Quality2",std::string(""));
}


StatusCode HltTrackMatch::setReco(const std::string& key) 
{
  m_matchName = key;
  debug() << " InitReco " << endreq;
  bool ok = m_recoConf.has_key(m_matchName+"/Tool");
  Assert(ok," no reconstruction with name "+m_matchName);
  
  std::string toolName = m_recoConf.retrieve<std::string>(m_matchName+"/Tool");
  m_recoID   = m_recoConf.retrieve<int>(m_matchName+"/RecoID");
  m_TESOutput = m_recoConf.retrieve<std::string>(m_matchName+"/TESOutput");
  m_trackType = m_recoConf.retrieve<int>(m_matchName+"/TrackType");
  m_transferIDs = m_recoConf.retrieve<bool>(m_matchName+"/TransferIDs");
  m_transferAncestor = 
    m_recoConf.retrieve<bool>(m_matchName+"/TransferAncestor");
  m_transferInfo = 
    m_recoConf.retrieve<bool>(m_matchName+"/TransferInfo");

  m_qualityName = 
    m_recoConf.retrieve<std::string>(m_matchName+"/Quality");
  m_qualityID = 0;
  if (!m_qualityName.empty()) m_qualityID = hltInfoID(m_qualityName);
  m_quality2Name = 
    m_recoConf.retrieve<std::string>(m_matchName+"/Quality2");
  m_quality2ID = 0;
  if (!m_quality2Name.empty()) m_quality2ID = hltInfoID(m_quality2Name);
  
  info() << " Reco: " << m_matchName 
         << " Tool: " << toolName
         << " Output: " << m_TESOutput << endreq;

  debug() << " reco ID " << m_recoID
          << " transfers IDs " << m_transferIDs 
          << " transfer ancestor " << m_transferAncestor
          << " track type " << m_trackType 
          << " quality [1] " << m_qualityName << " " << m_qualityID
          << " quality [2] " << m_quality2Name << " " << m_quality2ID
          << endreq;
  
  m_tool = tool<ITrackMatch>(toolName,this);

  return StatusCode::SUCCESS;
}


//=============================================================================
// Main execution
//=============================================================================
StatusCode HltTrackMatch::execute() {

  debug() << "==> Execute" << endmsg;
  StatusCode sc = StatusCode::SUCCESS;

  Tracks* otracks = 
    getOrCreate<LHCb::Tracks,LHCb::Tracks>(m_TESOutput);

  BOOST_FOREACH( LHCb::Track* track1, *m_selections.input<1>()) {
    if (m_verbose) printInfo(" track [1] ",*track1);
    BOOST_FOREACH( LHCb::Track* track2, *m_selections.input<2>()) {
      if (m_verbose) printInfo(" track [2] ",*track2);

      Track otrack(track1->key());

      double quality = 0.;
      double quality2 = 0.;
      sc = m_tool->match(*track1,*track2,otrack,quality,quality2);
      if (sc.isFailure()) {
        // Warning(" matching failed ",0);
        continue;
      }
      if (m_verbose) {
        verbose() << " track [1+2] quality " 
                  << quality << ", " << quality2 << endreq;
        printInfo(" track [1+2] ",otrack);
      }   
        
      if ((quality<m_maxQuality) && (quality2 < m_maxQuality2)) {
        verbose() << " accepted track [1+2] " << endreq;
        LHCb::Track* track3 = otrack.clone();
        if (m_qualityID != 0) track3->addInfo(m_qualityID,quality);
        if (m_quality2ID != 0) track3->addInfo(m_quality2ID,quality2);
        if (m_transferInfo) {
          Hlt::MergeInfo(*track1,*track3);
          Hlt::MergeInfo(*track2,*track3);
        }  
        otracks->insert( track3 );
        m_selections.output()->push_back(track3);
      }

    }
  }
  
  if (m_debug) printInfo(" matched tracks ",*m_selections.output());


  return StatusCode::SUCCESS;
}

