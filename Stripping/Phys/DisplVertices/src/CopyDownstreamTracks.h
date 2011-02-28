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
  std::string m_downstreamTrackLocation;      ///< where the downstream tracks are saved
};


#endif // COPYDOWNSTREAMTRACKS_H

