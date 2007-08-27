// $Id: TrackBuildCloneTable.h,v 1.1 2007-08-27 14:52:33 mneedham Exp $
#ifndef _TrackBuildCloneTable_H_
#define _TrackBuildCloneTable_H_

/** @class TrackBuildCloneTable TrackBuildCloneTable.h
 *
 *  Copy a container of tracks. By default do not copy tracks that failed the fit
 *
 *  @author M.Needham
 *  @date   30/05/2006
 */

#include "GaudiAlg/GaudiAlgorithm.h"

#include <string>
#include <vector>

namespace LHCb{
  class Track;
  class State;
}

class ITrackExtrapolator;

class TrackBuildCloneTable: public GaudiAlgorithm {

public:

  // Constructors and destructor
  TrackBuildCloneTable(const std::string& name,
              ISvcLocator* pSvcLocator);
  virtual ~TrackBuildCloneTable();

  virtual StatusCode initialize();

  virtual StatusCode execute();

private:

  typedef struct {
    LHCb::Track* track;
    LHCb::State state;
    Gaudi::TrackSymMatrix invCov; 
  } CloneTrack;

  typedef std::vector<CloneTrack> CloneTracks;

  double kullbeckLieblerDist(const CloneTrack& track1,
			     const CloneTrack& track2) const;

  bool invert(Gaudi::TrackSymMatrix& m) const;

  double m_klCut;
  std::string m_inputLocation;
  std::string m_outputLocation;

  double m_zState;
  double m_maxDz;
  ITrackExtrapolator* m_extrapolator;

};

#endif
