// $Id: PatVeloFitLHCbIDs.cpp,v 1.3 2008-10-01 14:33:58 dhcroft Exp $
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
  declareProperty( "beamState",             m_beamState = false   );
  declareProperty( "stepError"       , m_stepError        = 0.002     );
  declareProperty( "variableMS"      , m_variableMS       = false     );
  declareProperty( "forwardStepError", m_forwardStepError = 0.00035   );
}
//=============================================================================
// Destructor
//=============================================================================
Tf::PatVeloFitLHCbIDs::~PatVeloFitLHCbIDs() {
}

StatusCode Tf::PatVeloFitLHCbIDs::initialize(){
  StatusCode sc = GaudiTool::initialize();
  if(!sc) return sc;
  
  m_rHitManager = tool<PatVeloRHitManager>( "Tf::PatVeloRHitManager" );
  m_phiHitManager = tool<PatVeloPhiHitManager>( "Tf::PatVeloPhiHitManager" );
  m_PatVeloTrackTool = 
    tool<PatVeloTrackTool>("Tf::PatVeloTrackTool","PatVeloTrackTool"); 
  m_stepError        = m_stepError;
  m_forwardStepError = m_forwardStepError; 
  return StatusCode::SUCCESS;
}

StatusCode Tf::PatVeloFitLHCbIDs::fit( LHCb::Track & track, LHCb::ParticleID){

  // place to store the LHCbIDs ignored in the PatSpaceTrack fit
  std::vector<LHCb::LHCbID> nonVELOIDs;
   
  PatVeloSpaceTrack * patVeloTrack = new  PatVeloSpaceTrack();
  std::vector<LHCb::LHCbID>::const_iterator iID;
  for( iID = track.lhcbIDs().begin() ; iID != track.lhcbIDs().end() ; ++iID){
    if( ! iID->isVelo() ) {
      nonVELOIDs.push_back(*iID);
      continue;
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

  // collect all states past the end of the VELO plus q/p if present
  // to copy back into track
  double veloQoverP = 0.;
  double veloQoverPerr2 = 0.;

  LHCb::Track::StateContainer savedStates;

  const std::vector< LHCb::State * > states = track.states();
  std::vector< LHCb::State * >::const_iterator iState;
  for( iState = states.begin() ; iState != states.end() ; ++iState ){
    LHCb::State::Location location = (*iState)->location();
    if( ( location == LHCb::State::ClosestToBeam || 
	  location == LHCb::State::FirstMeasurement ||
	  location == LHCb::State::EndVelo ) && veloQoverP == 0.){
      veloQoverP = (*iState)->qOverP();
      veloQoverPerr2 = (*iState)->errQOverP2();
    }else{
      // clone state to add back to track
      savedStates.push_back((*iState)->clone());
    }
  }


  patVeloTrack->fitRZ();
  // set R on phi hits
  m_PatVeloTrackTool->setPhiCoords(patVeloTrack);
  // fit the track trajectory
  double msCorr = m_stepError;
  // use a rough functional form but only if the momentum has been estimated
  if(m_variableMS && veloQoverP!=0.){
    if( fabs(veloQoverP) > 2.e-4 ) {
      msCorr = 0.03; // fixed below 5 GeV/c
    }else{
      msCorr = 0.00025 + 0.04*exp(-7.e-5/fabs(veloQoverP));
    }
  }

  patVeloTrack->fitSpaceTrack( msCorr, true, m_beamState );

  
  StatusCode sc =
    m_PatVeloTrackTool->makeTrackFromPatVeloSpace(patVeloTrack,&track,
						  m_forwardStepError,
						  m_beamState);
  delete(patVeloTrack);

  // add back the non-velo LHCbIDs
  for( iID = nonVELOIDs.begin() ; iID != nonVELOIDs.end() ; ++iID){
    track.addToLhcbIDs(*iID);
  }

  // now copy back q/p and error from remade track states
  const std::vector< LHCb::State * > newStates = track.states();
  for( iState = newStates.begin() ; iState != newStates.end() ; ++iState ){
    LHCb::State::Location location = (*iState)->location();
    if( location == LHCb::State::ClosestToBeam || 
	location == LHCb::State::FirstMeasurement ||
	location == LHCb::State::EndVelo ) {
      (*iState)->setQOverP(veloQoverP);
      (*iState)->setErrQOverP2(veloQoverPerr2);
    }
  }
  // copy the other track states back into the new track
  if(!savedStates.empty()){
    track.addToStates(savedStates);
  }
  

  return sc;
}
