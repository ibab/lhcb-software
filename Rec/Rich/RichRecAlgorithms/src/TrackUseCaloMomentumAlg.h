// $Id: TrackUseCaloMomentumAlg.h,v 1.1 2008-07-28 16:08:11 jonrob Exp $
#ifndef TRACKUSECALOMOMENTUMALG_H
#define TRACKUSECALOMOMENTUMALG_H 1

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// Event
#include "Event/Track.h"

// Interfaces
#include "TrackInterfaces/ITrackCaloMatch.h" 

/** @class TrackUseCaloMomentumAlg TrackUseCaloMomentumAlg.h
 *
 *  Clones the tracks from the input location and updates their
 *  momentum using CALO information. Only tracks with CALO 
 *  information are cloned.
 *  
 *  @author Chris Jones
 *  @date   2008-05-27
 */

class TrackUseCaloMomentumAlg : public GaudiAlgorithm

{

public:

  /// Standard constructor
  TrackUseCaloMomentumAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TrackUseCaloMomentumAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private:

  /// Track -> CALO matching tool
  ITrackCaloMatch * m_trackenergy;

  /// Input tracks location
  std::string m_inTracksLoc;

  /// Output tracks location
  std::string m_outTracksLoc;

};

#endif // TRACKUSECALOMOMENTUMALG_H
