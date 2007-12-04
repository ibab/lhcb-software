// $Id: HltPrepareL0Muons.cpp,v 1.2 2007-12-04 13:26:35 hernando Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "HltPrepareL0Muons.h"
#include "Event/HltEnums.h"
//#include "Event/HltSummaryFunctor.h"
#include "HltBase/HltSequences.h"
#include "HltBase/HltFunctions.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltPrepareL0Muons
//
// 2006-07-28 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltPrepareL0Muons );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltPrepareL0Muons::HltPrepareL0Muons( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
{
  // declareCondition("MinPt",m_ptMin = 0.);
  
  declareProperty("MinPt", m_PtMin = 0.1);
  declareProperty("OutputL0MuonTracksName"   ,
                  m_outputMuonTracksName );

  m_maker = 0;
}
//=============================================================================
// Destructor
//=============================================================================
HltPrepareL0Muons::~HltPrepareL0Muons() {
} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltPrepareL0Muons::initialize() {
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorith

  m_maker = tool<IMuonSeedTool>("MuonSeedTool");
  if (!m_maker)
    error() << " not able to create maker muon tracks tool " << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltPrepareL0Muons::execute() {

  StatusCode sc = StatusCode::SUCCESS;

  L0MuonCandidates* inputL0Muon = 
    get<L0MuonCandidates>(L0MuonCandidateLocation::Default);

  Tracks* muons = new Tracks();
//  put(muons,"Hlt/Track/L0Muon");
  put(muons,  m_outputMuonTracksName);

  for (std::vector<L0MuonCandidate*>::iterator it= inputL0Muon->begin();
       it !=  inputL0Muon->end(); ++it){
    const L0MuonCandidate& l0muon = *(*it);
    if (fabs(l0muon.pt()) < m_PtMin) continue;
    debug() << "l0pt " << l0muon.pt()<< endmsg;
    bool isClone=checkClone(*it);
    if (isClone) debug() << "is clone " << endmsg;
    if (isClone) continue;
    Track* track = new Track();
    sc = m_maker->makeTrack(l0muon,*track);
    muons->insert(track);
    m_outputTracks->push_back(track);
  }

  if (m_debug) {
    debug() << " n L0MuonCandidates " << inputL0Muon->size() << endreq;
    debug() << " n L0Muon Tracks " << m_outputTracks->size() << endreq;
  }

  return sc;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltPrepareL0Muons::finalize() {

  debug() << "==> Finalize" << endmsg;
  return HltAlgorithm::finalize();  
}

//=============================================================================
bool HltPrepareL0Muons::checkClone(L0MuonCandidate* muon)
{
  std::vector<MuonTileID> list_of_tileM1= muon->muonTileIDs(0);
  MuonTileID tileM1=*(list_of_tileM1.begin());

  std::vector<MuonTileID> list_of_tileM2= muon->muonTileIDs(1);
  MuonTileID tileM2=*(list_of_tileM2.begin());
  for( std::vector<Track*>::const_iterator pItr=m_outputTracks->begin();
            pItr<m_outputTracks->end();pItr++){
    std::vector< LHCb::LHCbID > list_lhcb=      (*pItr)->lhcbIDs();
    MuonTileID oldTileM1;
    MuonTileID oldTileM2;

    for(std::vector< LHCb::LHCbID >::iterator iM1=list_lhcb.begin();iM1<list_lhcb.end();iM1++){
      if(iM1->isMuon()){
        if(iM1->muonID().station()==0){
          oldTileM1=iM1->muonID();
          break;
        }
      }
    }
    for(std::vector< LHCb::LHCbID >::iterator iM1=list_lhcb.begin();iM1<list_lhcb.end();iM1++){
      if(iM1->isMuon()){
        if(iM1->muonID().station()==1){
          oldTileM2=iM1->muonID();
          break;
        }
      }
    }
    if(oldTileM1.isValid()&&oldTileM1==tileM1&&oldTileM2.isValid()&&oldTileM2==tileM2)return true;



  }
  return false;

}
