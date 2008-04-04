// $Id: PatVeloFitLHCbIDs.cpp,v 1.2 2008-04-04 06:36:13 mneedham Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// non-local classes used
#include "Event/Track.h"

// local
#include "PatVeloTrackTool.h"
#include "PatVeloSpaceTrack.h"
#include "PatVeloFitLHCbIDs.h"
#include "PatVeloRHitManager.h"
#include "PatVeloPhiHitManager.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatVeloFitLHCbIDs
// Takes a list of VELO LHCbIDs form a track and reprodcues the PatVelo internal 
// track fit
//
// 2008-02-07 : David Hutchcroft
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
namespace Tf{
  DECLARE_TOOL_FACTORY( PatVeloFitLHCbIDs );
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Tf::PatVeloFitLHCbIDs::PatVeloFitLHCbIDs( const std::string& type,
					  const std::string& name,
					  const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<ITrackFitter>(this);
}
//=============================================================================
// Destructor
//=============================================================================
Tf::PatVeloFitLHCbIDs::~PatVeloFitLHCbIDs() {
  declareProperty( "stepError"       , m_stepError        = 0.002     );
  declareProperty( "forwardStepError", m_forwardStepError = 0.00035   );
}

StatusCode Tf::PatVeloFitLHCbIDs::initialize(){
  StatusCode sc = GaudiTool::initialize();
  if(!sc) return sc;
  
  m_rHitManager = tool<PatVeloRHitManager>( "Tf::PatVeloRHitManager" );
  m_phiHitManager = tool<PatVeloPhiHitManager>( "Tf::PatVeloPhiHitManager" );
  m_PatVeloTrackTool = 
    tool<PatVeloTrackTool>("Tf::PatVeloTrackTool","PatVeloTrackTool"); 
  m_stepError        = 0.002;
  m_forwardStepError = 0.00035; 
  return StatusCode::SUCCESS;
}

StatusCode Tf::PatVeloFitLHCbIDs::fit( LHCb::Track & track, LHCb::ParticleID){
  PatVeloSpaceTrack * patVeloTrack = new  PatVeloSpaceTrack();
   std::vector<LHCb::LHCbID>::const_iterator iID;
  for( iID = track.lhcbIDs().begin() ; iID != track.lhcbIDs().end() ; ++iID){
    if( ! iID->isVelo() ) {
      Warning("Only VeloIDs work for PatVeloFitLHCbIDs");
      delete(patVeloTrack);
      return StatusCode::FAILURE;
    }
    if( iID->isVeloPhi() ){
      int sensor = iID->veloID().sensor();
      // add Phi clusters from sensor sensor
      PatVeloPhiHitManager::Station * phiStation = 
	*(m_phiHitManager->stationIterAll(sensor));
      int zone =phiStation->sensor()->globalZoneOfStrip(iID->veloID().strip());
      PatVeloPhiHit* phiHit = phiStation->hitByLHCbID(zone,*iID);
      if(!phiHit){
	warning() << "Did not find phi cluster for " << *iID << endmsg;
	delete(patVeloTrack);
	return StatusCode::FAILURE;
      }
      patVeloTrack->addPhi(phiHit);
    }
    if( iID->isVeloR() ){
      int sensor = iID->veloID().sensor();
      // add R clusters from sensor sensor
      PatVeloRHitManager::Station * rStation = 
	*(m_rHitManager->stationIterAll(sensor));
      int zone =rStation->sensor()->globalZoneOfStrip(iID->veloID().strip());
      PatVeloRHit* rHit = rStation->hitByLHCbID(zone,*iID);
      if(!rHit){
	warning() << "Did not find R cluster for " << *iID << endmsg;
	delete(patVeloTrack);
	return StatusCode::FAILURE;
      }
      patVeloTrack->addRCoord(rHit);
    }
  }
  patVeloTrack->fitRZ();
  // set R on phi hits
  m_PatVeloTrackTool->setPhiCoords(patVeloTrack);
  // fit the track trajectory
  patVeloTrack->fitSpaceTrack( m_stepError );

  track.reset();
  StatusCode sc =
    m_PatVeloTrackTool->makeTrackFromPatVeloSpace(patVeloTrack,&track,
						  m_forwardStepError);
  delete(patVeloTrack);
  return sc;
}
