// $Id: CleanPatTrackSelector.h,v 1.1 2006-06-29 06:08:28 mneedham Exp $
#ifndef _CleanPatTrackSelector_H
#define _CleanPatTrackSelector_H

/** @class CleanPatTrackSelector CleanPatTrackSelector TrackTools/CleanPatTrackSelector.h
 *
 *  Track Selection tool
 *
 *  Select only PatTracks that are not ghosts or have outliers 
 *  @author M.Needham Matt.Needham@cern.ch
 *
 *  @date   30/12/2005
 */

#include "GaudiAlg/GaudiTool.h"
#include "TrackInterfaces/ITrackSelector.h"
#include <string>

#include "Linker/LinkerTool.h"
#include "Event/Track.h"

namespace LHCb{
  class MCParticle;
  class Track;
};

class CleanPatTrackSelector : public GaudiTool,
	                      virtual public ITrackSelector{

public:

  /// constructer
  CleanPatTrackSelector( const std::string& type,
                 const std::string& name,
                 const IInterface* parent );

  virtual ~CleanPatTrackSelector();

  /// the method
  virtual bool accept(const LHCb::Track& aTrack) const;

private:

  typedef LinkerTool<LHCb::Track, LHCb::MCParticle> Asct;
  typedef Asct::DirectType            Table;
  typedef Asct::DirectType::Range     Range;
  typedef Table::iterator               iterator;


  bool cleanTrack(const LHCb::Track& aTrack) const;

  bool m_rejectTracksWithOutliers;

};

#endif
