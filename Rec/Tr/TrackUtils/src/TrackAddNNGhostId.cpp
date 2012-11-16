// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Event/Track.h"
#include "TrackInterfaces/ITrackManipulator.h"

// local
#include "TrackAddNNGhostId.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrackAddNNGhostId
//
// 2009-10-06 : Johannes Albrecht
//-----------------------------------------------------------------------------
using namespace LHCb;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TrackAddNNGhostId )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackAddNNGhostId::TrackAddNNGhostId( const std::string& name,
                                      ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_ghostTool(0)
{
  declareProperty("inputLocation", m_inputLocation = TrackLocation::Default);
  declareProperty("GhostIdTool", m_ghostToolName = "TrackNNGhostId");
}
//=============================================================================
// Destructor
//=============================================================================
TrackAddNNGhostId::~TrackAddNNGhostId() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TrackAddNNGhostId::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( UNLIKELY(msgLevel(MSG::DEBUG)) ) debug() << "==> initialize" << endmsg;

  m_ghostTool =  tool<ITrackManipulator>(m_ghostToolName,this);
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode TrackAddNNGhostId::execute() {

  if ( UNLIKELY(msgLevel(MSG::DEBUG)) ) debug() << "==> Execute" << endmsg;
  
  Tracks* inCont = getIfExists<Tracks>(m_inputLocation);
  if (NULL==inCont) {
    if ( UNLIKELY(msgLevel(MSG::DEBUG)) ) debug() << "no tracks at "<<m_inputLocation << endmsg;
    return StatusCode::SUCCESS;
  }
  
  // loop 
  for (Tracks::iterator iterT = inCont->begin(); iterT != inCont->end(); ++iterT) {
    
    m_ghostTool->execute(**iterT).ignore();
    
  } // iterT
  

  return StatusCode::SUCCESS;
}

//=============================================================================
