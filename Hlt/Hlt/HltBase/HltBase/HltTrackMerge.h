
#include "Event/Track.h"

namespace Hlt {
  /* merge two tracks (adds info, ids, states)
   *
   */
inline void TrackMerge(const LHCb::Track& track, LHCb::Track& otrack) 
{
  
  // setting ancestors
  otrack.addToAncestors(track);
  
  // adding info
  const GaudiUtils::VectorMap<int,double>& info = track.extraInfo();
  for ( GaudiUtils::VectorMap<int,double>::const_iterator it = info.begin();
        it != info.end(); ++it)
  { otrack.addInfo(it->first,it->second); }
  
  // adding states
  const std::vector<LHCb::State*>& states = track.states();
  for (std::vector<LHCb::State*>::const_iterator it = states.begin();
       it != states.end(); ++it) 
  { otrack.addToStates(**it); }
  
  
  // adding ids
  otrack.addSortedToLhcbIDs( track.lhcbIDs() );
}
  
  template <class T>
  void MergeInfo(const T& input, T& output) 
  {
    const GaudiUtils::VectorMap<int,double>& info = input.extraInfo();
    GaudiUtils::VectorMap<int,double>::const_iterator it = info.begin();
    for (; it != info.end(); ++it)
    { output.addInfo(it->first,it->second); }
  }
}
