
#include "Event/Track.h"

namespace Hlt {

template <typename T> 
void MergeInfo(const T& in, T& out) 
{
  for (const auto& p : in.extraInfo()) { out.addInfo(p.first,p.second); }

}
  /* merge two tracks (adds info, ids, states)
   *
   */
inline void TrackMerge(const LHCb::Track& track, LHCb::Track& otrack) 
{
  // setting ancestors
  otrack.addToAncestors(track);
  
  // adding info
  MergeInfo(track,otrack);
  
  // adding states
  for (auto state : track.states() ) { otrack.addToStates(*state) ; }
  
  // adding ids
  otrack.addSortedToLhcbIDs( track.lhcbIDs() );
}
  
}
