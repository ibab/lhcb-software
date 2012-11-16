#ifndef NUMBEROFTRACKSFILTER_H 
#define NUMBEROFTRACKSFILTER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class NumberOfTracksFilter NumberOfTracksFilter.h
 *  
 *  Looks into TES locations foir tracks and returns filterPassed
 *  true or false depending on the numebr of tracks.
 *  Default is at least 0 tracks (i.e. do nothing)
 *  
 *  @code
 *  NumberOfTracksFilter.TrackLocations = { "Rec/Track/Best" };
 *  NumberOfTracksFilter.MinTracks = 2 ;
 *  NumberOfTracksFilter.MaxTracks = 1000 ;
 *  @endcode
 *
 *  This will require there are at least 2 and at most 1000 tracks in
 *  Rec/Track/Best to constinue the sequence. MaxTracks = -1 (default) 
 *  does nothing. 
 *
 *  @author Patrick Koppenburg
 *  @date   2008-03-05
 */
class NumberOfTracksFilter : public GaudiAlgorithm {
public: 
  /// Standard constructor
  NumberOfTracksFilter( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~NumberOfTracksFilter( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:
  std::vector<std::string> m_tracksPath ; ///< locations
  int m_minTracks  ; ///< min number of tracks
  int m_maxTracks ;  ///< max number of tracks


};
#endif // NUMBEROFTRACKSFILTER_H
