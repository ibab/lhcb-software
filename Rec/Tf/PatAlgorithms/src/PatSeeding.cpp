// $Id: PatSeeding.cpp,v 1.1.1.1 2007-10-09 18:23:10 smenzeme Exp $
// Include files

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "OTDet/DeOTDetector.h"
// from TrackEvent
#include "Event/StateParameters.h"

// local
#include "PatSeeding.h"
#include "PatFwdFitLine.h"
#include "PatFwdPlaneCounter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatSeeding
//
// 2006-10-13 : Olivier Callot
//-----------------------------------------------------------------------------


// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( PatSeeding );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatSeeding::PatSeeding( const std::string& name,
                        ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "OutputTracksName",  m_outputTracksName = LHCb::TrackLocation::Seed     );
  
}
//=============================================================================
// Destructor
//=============================================================================
PatSeeding::~PatSeeding() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode PatSeeding::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  m_seedingTool = tool<IPatSeedingTool>( "PatSeedingTool","PatSeedingTool" );

  m_tracking = 0;
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PatSeeding::execute() {
 
  m_seedingTool->prepareHits();

  LHCb::Tracks* outputTracks = new LHCb::Tracks();
  outputTracks->reserve(200);
  put(outputTracks, m_outputTracksName);

  m_seedingTool->performTracking(outputTracks);
  
  return StatusCode::SUCCESS;
}
   
//=============================================================================
//  Finalize
//=============================================================================
StatusCode PatSeeding::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}
