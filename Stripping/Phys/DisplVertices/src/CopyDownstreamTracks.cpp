// $Id: $
// Include files 

#ifndef COPYDOWNSTREAMTRACKS_H 
#define COPYDOWNSTREAMTRACKS_H 1

// from Gaudi


#include "GaudiKernel/AlgFactory.h" 
#include "GaudiAlg/GaudiAlgorithm.h"
#include "Event/Track.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CopyDownstreamTracks
//
// 2011-02-22
//
// A little class to copy the downstream track pointer to another container
//
//-----------------------------------------------------------------------------

class CopyDownstreamTracks : public GaudiAlgorithm { 
public: 
  /// Standard constructor
  CopyDownstreamTracks( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CopyDownstreamTracks( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  
private:
  std::vector<std::string> m_TracksLocations; ///< where the input tracks are located
  std::string m_downstreamTrackLocation;          ///< where the downstream tracks are saved
};


#endif // COPYDOWNSTREAMTRACKS_H

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CopyDownstreamTracks );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CopyDownstreamTracks::CopyDownstreamTracks( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : GaudiAlgorithm( name , pSvcLocator )
{
  declareProperty( "Inputs", m_TracksLocations ) ;
  declareProperty( "Output", m_downstreamTrackLocation = "Rec/SelectedDownstream/Tracks" );
}
//=============================================================================
// Destructor
//=============================================================================
CopyDownstreamTracks::~CopyDownstreamTracks() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CopyDownstreamTracks::initialize() {
  if( m_TracksLocations.size() == 0 ){
    m_TracksLocations.push_back("Rec/Track/Best");
  }
  return GaudiAlgorithm::initialize(); // must be executed first

}
//=============================================================================
// Main execution
//=============================================================================
StatusCode CopyDownstreamTracks::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  setFilterPassed(false);


  LHCb::Tracks* downtracks = new LHCb::Tracks(); 
  put( downtracks, m_downstreamTrackLocation );
  for (std::vector< std::string >::iterator ilocation = m_TracksLocations.begin(); m_TracksLocations.end() != ilocation ; ++ilocation){
    if (!exist<LHCb::Tracks>(*ilocation)) 
      return Warning( (*ilocation) +" not found", StatusCode::SUCCESS, 0);
    const LHCb::Tracks* trackContainer = get<LHCb::Tracks*>( *ilocation ) ;
    //Loop on the reconstructed primary vertices
    for( LHCb::Tracks::const_iterator itr = trackContainer->begin() ;
	 itr != trackContainer->end(); ++itr ) 
      if ((*itr)->type() == LHCb::Track::Downstream ) 
	downtracks ->insert( *itr ); 
  }
  if (downtracks->size()>0)  setFilterPassed(true);
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode CopyDownstreamTracks::finalize() {

  if( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();
}
