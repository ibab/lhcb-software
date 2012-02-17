// $Id: $
// Include files

// local
#include "Event/RecVertex.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RecVertex
//
// 2012-02-17 : Chris Jones
//-----------------------------------------------------------------------------

void LHCb::RecVertex::setTracks(const SmartRefVector<LHCb::Track>& tracks)
{
  // Set the track smart refs
  m_tracks = tracks;
  // set implicit weights of 1 for each
  m_weights = std::vector<float>( tracks.size(), 1.0 );
}

void LHCb::RecVertex::addToTracks(const LHCb::Track* track,
                                  const float weight)
{
  m_tracks.push_back(track);
  m_weights.push_back(weight);
}

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
