// $Id: HltPrepareMuonSeg.cpp,v 1.4 2008-05-03 15:20:38 graven Exp $
// Include files 

// from Gaudi

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SystemOfUnits.h"
// local
#include "HltPrepareMuonSeg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltPrepareMuonSeg
//
// 2007-12-4 : Sandra Amato 
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltPrepareMuonSeg );

using namespace LHCb;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltPrepareMuonSeg::HltPrepareMuonSeg( const std::string& name,
                                      ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
{
  declareProperty("OutputMuonTracksName"   ,
                  m_outputMuonTracksName = "Hlt/Tracks/ConfirmedTMuon");

  m_doInitSelections = false;
}
//=============================================================================
// Destructor
//=============================================================================
HltPrepareMuonSeg::~HltPrepareMuonSeg() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltPrepareMuonSeg::initialize() {
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  m_inputTracks = &(retrieveTSelection<LHCb::Track>(m_inputSelectionName));
  
  m_outputTracks = &(registerTSelection<LHCb::Track>(m_outputSelectionName));

  debug() << "==> Initialize" << endmsg;
  
  m_prepareMuonSeed = tool<IMuonSeedTool>("MuonSeedTool",this);
  


  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltPrepareMuonSeg::execute() {
  
  debug() << "==> Execute" << endmsg;

  //Container with all T tracks
  Tracks* muontracks = new Tracks();
  put(muontracks,m_outputMuonTracksName);
  debug() << "Muon segments tracks size " << m_inputTracks->size()<< endmsg;
  
   for ( std::vector<Track*>::const_iterator itT = m_inputTracks->begin();
        m_inputTracks->end() != itT; itT++ ) {
    Track* itMuonSeg = (*itT);
    Track* seedTrack = new Track();
    //    LHCb::Track seedTrack; 
    StatusCode sctmp = m_prepareMuonSeed->makeTrack( *itMuonSeg , *seedTrack );
    if( sctmp.isFailure() ){
      err()<<"Failed to prepare the seed"<<endmsg;
    }
    muontracks->insert(seedTrack); 
    m_outputTracks->push_back(seedTrack);
 }//for ( std::vector<Track*>::const_iterator itT = m_inputTracks->begin() 
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltPrepareMuonSeg::finalize() {
  
  debug() << "==> Finalize" << endmsg;
  
  return HltAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
