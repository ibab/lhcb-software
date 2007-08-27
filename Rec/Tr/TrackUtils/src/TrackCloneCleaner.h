// $Id: TrackCloneCleaner.h,v 1.1 2007-08-27 14:52:33 mneedham Exp $
#ifndef _TrackCloneCleaner_H_
#define _TrackCloneCleaner_H_

/** @class TrackCloneCleaner TrackCloneCleaner.h
 *
 *  Clean out tracks with some criteria from the container
 *
 *  @author M.Needham
 *  @date   30/05/2006
 */

#include "GaudiAlg/GaudiAlgorithm.h"

#include <string>
#include <vector>

namespace LHCb{
  class Track;
}

class TrackCloneCleaner: public GaudiAlgorithm {

public:

  // Constructors and destructor
  TrackCloneCleaner(const std::string& name,
              ISvcLocator* pSvcLocator);
  virtual ~TrackCloneCleaner();

  virtual StatusCode execute();

private:

  class WorkingTrack{
    public:
      WorkingTrack(){}    
      LHCb::Track* track;
      bool clone;
      double chi2() const {return track->chi2PerDoF();}
      double nMeas() const {return track->nMeasurements();}
      bool sameTrack(const LHCb::Track* aTrack) const {return track == aTrack;}
  };

  typedef std::vector<WorkingTrack> WorkingTracks;

  void tagClone(WorkingTracks& tracks, 
		const LHCb::Track* aTrack) const;

  std::string m_inputLocation;
  std::string m_outputLocation;
  std::string m_linkerLocation;

};

#endif
