// $Id: $
// Include files 

#ifndef VELOWITHIP_H 
#define VELOWITHIP_H 1

// from Gaudi


#include "GaudiKernel/AlgFactory.h" 
#include "GaudiAlg/GaudiAlgorithm.h"
#include "Event/Track.h"
#include "Event/VertexBase.h"
#include "Event/RecVertex.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CopyDownstreamTracks
//
// 2011-02-22
//
// A little class to copy the downstream track pointer to another container
//
//-----------------------------------------------------------------------------

class VeloWithIP : public GaudiAlgorithm { 
public: 
  /// Standard constructor
  VeloWithIP( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~VeloWithIP( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  
private:
  std::vector<std::string> m_TracksLocations; ///< where the input tracks are located
  std::string m_WithIPTrackLocation;      ///< where the downstream tracks are saved
  std::string m_PVLocation;      ///< where the downstream tracks are saved
  bool m_discardFromPV;
  double m_ipcut;      ///< where the downstream tracks are saved
};


#endif // COPYDOWNSTREAMTRACKS_H

