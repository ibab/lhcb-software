// $Id: $
// Include files

// local
#include "Event/RecVertex.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RecVertex
//
// 2012-02-17 : Chris Jones
//-----------------------------------------------------------------------------

void LHCb::RecVertex::removeFromTracks(const LHCb::Track * track)
{
  if ( !m_tracks.empty() )
  {
    SmartRefVector<LHCb::Track> newTracks;
    newTracks.reserve(m_tracks.size()-1);
    std::vector<float>          newWeights;
    newWeights.reserve(m_tracks.size()-1);
    std::vector<float>::const_iterator iW = m_weights.begin();
    for ( SmartRefVector<LHCb::Track>::const_iterator iTk = m_tracks.begin();
          iTk != m_tracks.end() && iW != m_weights.end();
          ++iTk, ++iW )
    {
      const LHCb::Track * tk = *iTk;
      if ( tk != track )
      {
        newTracks.push_back(tk);
        newWeights.push_back(*iW);
      }
    }
    m_tracks  = newTracks;
    m_weights = newWeights;
  }
}

LHCb::RecVertex::TrackWithWeightVector
LHCb::RecVertex::tracksWithWeights() const
{
  TrackWithWeightVector twV;
  twV.reserve( m_tracks.size() );
  std::vector<float>::const_iterator iW = m_weights.begin();
  for ( SmartRefVector<LHCb::Track>::const_iterator iTk = m_tracks.begin();
        iTk != m_tracks.end() && iW != m_weights.end();
        ++iTk, ++iW )
  {
    const LHCb::Track * track = *iTk;
    twV.push_back( TrackWithWeight(track,*iW) );
  }
  return twV;
}

void 
LHCb::RecVertex::setTracksWithWeights(const TrackWithWeightVector& tracksAndWeights)
{
  // Clear the current vectors
  clearTracks();

  // Loop over the tracks and weights and store them
  for ( TrackWithWeightVector::const_iterator iTW = tracksAndWeights.begin();
        iTW != tracksAndWeights.end(); ++iTW )
  {
    addToTracks( iTW->first, iTW->second );
  }
}
