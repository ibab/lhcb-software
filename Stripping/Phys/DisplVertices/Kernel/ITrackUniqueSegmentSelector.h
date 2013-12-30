#ifndef TRACKINTERFACES_ITrackUniqueSegmentSelector_H 
#define TRACKINTERFACES_ITrackUniqueSegmentSelector_H 1

// Include files
// -------------
#include "GaudiKernel/IAlgTool.h"

// From TrackEvent
#include "Event/Track.h"
// From PhysEvent
#include "Event/Particle.h"

class GAUDI_API ITrackUniqueSegmentSelector : virtual public IAlgTool
{
 public:
  /// interface machinery 
  DeclareInterfaceID(ITrackUniqueSegmentSelector, 1, 0);
  
  /// flag clones by modifying track Clone flag. note this function /changes/ the tracks!
  virtual StatusCode flag( std::vector<LHCb::Track*>& tracks ) const = 0 ;
  
  /// select a subset of tracks with unique velo/T segments
  virtual StatusCode select( const LHCb::Track::Range& tracksin, LHCb::Track::Selection& tracksout ) const = 0 ;
  
  /// select a subset of particles with unique velo/T segments
  virtual StatusCode select( const LHCb::Particle::Range& tracksin, LHCb::Particle::Selection& tracksout ) const = 0 ;
};

#endif // TRACKINTERFACES_ITRACKUNIQUESEGMENTSELECTOR_H
