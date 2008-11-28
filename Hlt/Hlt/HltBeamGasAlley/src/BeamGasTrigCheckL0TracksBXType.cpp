
#include "GaudiKernel/AlgFactory.h"
#include "Event/L0DUReport.h"
#include "Event/Track.h"
#include "Event/ODIN.h"
#include "BeamGasTrigCheckL0TracksBXType.h"

//-----------------------------------------------------------------------------
// Implementation file for class : BeamGasTrigCheckL0TracksBXType
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( BeamGasTrigCheckL0TracksBXType );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
BeamGasTrigCheckL0TracksBXType::BeamGasTrigCheckL0TracksBXType(const std::string& name, ISvcLocator* pSvcLocator)
  : GaudiAlgorithm(name, pSvcLocator)
{
  declareProperty("L0DULocation", m_l0Location = LHCb::L0DUReportLocation::Default );  
  declareProperty("PVRTTracksLocation", m_PVRTTracksLocation = "Hlt/Track/RZVelo" );
  declareProperty("ODINLocation", m_odinLocation = "DAQ/ODIN");  
  declareProperty("BXTypeAccept1", m_bxType1 = 1 );
  declareProperty("BXTypeAccept2", m_bxType2 = 2 );  
  declareProperty("CheckL0Decision", m_checkL0 = true );
  declareProperty("CheckTracks", m_checkTracks = true );
  declareProperty("ChechBXType", m_checkBXType = true );
}

//=============================================================================
// Destructor
//=============================================================================
BeamGasTrigCheckL0TracksBXType::~BeamGasTrigCheckL0TracksBXType() {;}

//=============================================================================
// Initialization
//=============================================================================
StatusCode BeamGasTrigCheckL0TracksBXType::initialize()
{
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if (sc.isFailure()) return sc;  // error printed already by GaudiAlgorithm

  verbose() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode BeamGasTrigCheckL0TracksBXType::execute()
{
  verbose() << "==> Execute" << endmsg;
  
  setFilterPassed(false);
  
  if( m_checkL0 ){  
    // Get The L0 Decision and reject the event if it is negative
    if( exist<LHCb::L0DUReport>(m_l0Location) ){
      LHCb::L0DUReport* l0 = get<LHCb::L0DUReport>(m_l0Location);
      if( l0->decision() == false ){
	debug() << "L0 decision is FALSE ==> rejecting the event" << endmsg;
	return StatusCode::SUCCESS; 
      }
    }
    else { error() << "Can't find L0 decision !!!" << endmsg; return StatusCode::SUCCESS; }
  }


  if( m_checkTracks ){	  
    // If the PatVeloRTracking output container is not present or empty ==> reject the event
    if( exist<LHCb::Tracks>(m_PVRTTracksLocation) ){
      if( get<LHCb::Tracks>(m_PVRTTracksLocation)->empty() ){   
	debug() << "PatVeloRTracking output container is empty ==> rejecting the event" << endmsg;
	return StatusCode::SUCCESS; 
      }
    }
    else{ error() << "PatVeloRTracking output container doesn't exist ==> rejecting the event" << endmsg; return StatusCode::SUCCESS; }
  }
  
  
  if ( m_checkBXType ){	  
    // If the Bunch-crossing type from ODIN and reject the event if it is not what we look for
    if( exist<LHCb::ODIN>(m_odinLocation) ){ 
      LHCb::ODIN* odin = get<LHCb::ODIN>(m_odinLocation);
      unsigned int bXType = (unsigned int)odin->bunchCrossingType();   // 1=SingleBeamLeft, 2=SingleBeamRight
      bool matchBXType = ( bXType==m_bxType1 || bXType==m_bxType2 );

      debug() << "ODIN BX Type = " << odin->bunchCrossingType() 
              << "( " << (unsigned int)( odin->bunchCrossingType() ) << " )"
              << "run Number = " << odin->runNumber() << endmsg;	

      if( !matchBXType ){ debug() << "Bunch crossing type is not of the type we look for" << endmsg; return StatusCode::SUCCESS; }  
    }
    else{ error() << "Can't access ODIN !!!" << endmsg; return StatusCode::SUCCESS; }
  }
    
  
  //If we reached to here :
  setFilterPassed(true);
 
  return StatusCode::SUCCESS;
}

//=============================================================================
// Finalize
//=============================================================================
StatusCode BeamGasTrigCheckL0TracksBXType::finalize()
{
  verbose() << "==> Finalize" << endmsg;
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}


