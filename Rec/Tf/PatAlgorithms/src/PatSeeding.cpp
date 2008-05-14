// $Id: PatSeeding.cpp,v 1.2 2008-05-14 17:22:18 mschille Exp $
// Include files

#include <vector>

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "OTDet/DeOTDetector.h"
// from TrackEvent
#include "Event/StateParameters.h"
#include "Event/Track.h"

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
StatusCode PatSeeding::initialize()
{
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  m_seedingTool = tool<IPatSeedingTool>(
		  "PatSeedingTool", "PatSeedingTool", this );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PatSeeding::execute()
{
  LHCb::Tracks* outputTracks = new LHCb::Tracks();

  m_seedingTool->prepareHits();
  StatusCode sc = m_seedingTool->performTracking(outputTracks);

  put(outputTracks, m_outputTracksName);

  return sc;
}
   
//=============================================================================
//  Finalize
//=============================================================================
StatusCode PatSeeding::finalize()
{
  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}
