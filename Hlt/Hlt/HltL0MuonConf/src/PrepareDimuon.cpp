// $Id: PrepareDimuon.cpp,v 1.2 2007-09-08 18:34:11 sandra Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "PrepareDimuon.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PrepareDimuon
//
// 2007-06-06 : Alessia Satta
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( PrepareDimuon );

using namespace LHCb;
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PrepareDimuon::PrepareDimuon( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
{
 declareProperty("OutputL0MuonVerticesName"   ,
                  m_outputL0MuonVerticesName);

}
//=============================================================================
// Destructor
//=============================================================================
PrepareDimuon::~PrepareDimuon() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode PrepareDimuon::initialize() {
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;
  m_patVertexBank =
    m_patDataStore->createVertexContainer(m_outputL0MuonVerticesName,36);



  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PrepareDimuon::execute() {

  debug() << "==> Execute" << endmsg;
  setFilterPassed(true);
  HltAlgorithm::beginExecute(); 
  for ( std::vector<Track*>::const_iterator itT = m_inputTracks->begin();
        m_inputTracks->end() != itT; itT++ ) {
    for ( std::vector<Track*>::const_iterator itT2 = itT+1;
          m_inputTracks->end() != itT2; itT2++ ) {
      if((*itT)->checkFlag(Track::L0Candidate)||
         (*itT2)->checkFlag(Track::L0Candidate)){
        
        //info()<<" l0 candidate ? "<<(*itT)->checkFlag(Track::L0Candidate)<<endreq;
        RecVertex* ver = m_patVertexBank->newEntry();
        ver->addToTracks (*itT);
        ver->addToTracks (*itT2);
        m_outputVertices->push_back(ver);
      }
    }
    
      // info()<<" l0 candidate ? "<<(*itT)->checkFlag(Track::L0Candidate)<<endreq;
    
    
  }
  
  HltAlgorithm::endExecute();
     
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode PrepareDimuon::finalize() {

  debug() << "==> Finalize" << endmsg;

  return HltAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
