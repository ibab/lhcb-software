// $Id: CleanPatTrackSelector.h,v 1.4 2009-01-29 12:28:48 mneedham Exp $
#ifndef _CleanPatTrackSelector_H
#define _CleanPatTrackSelector_H

/** @class CleanPatTrackSelector CleanPatTrackSelector.h
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
};

class IMCParticleSelector;

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

  /// initialize
  StatusCode initialize();

private:

  typedef LinkerTool<LHCb::Track, LHCb::MCParticle> Asct;
  typedef Asct::DirectType            Table;
  typedef Asct::DirectType::Range     Range;
  typedef Table::iterator               iterator;


  bool cleanTrack(const LHCb::Track& aTrack) const;

  bool m_rejectTracksWithOutliers;
 
  IMCParticleSelector* m_selector;

};

#endif
