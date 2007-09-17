// $Id: TrackCloneCleaner.h,v 1.3 2007-09-17 09:25:03 jonrob Exp $
#ifndef _TrackCloneCleaner_H_
#define _TrackCloneCleaner_H_

#include "GaudiAlg/GaudiAlgorithm.h"

#include <string>
#include <vector>

#include "Event/Track.h"

// Boost
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>

// from Event/LinkerEvent
#include "Linker/LinkedFrom.h"

/** @class TrackCloneCleaner TrackCloneCleaner.h
 *
 *  Clean out clone tracks, using information from the Clone linker table
 *
 *  @author M.Needham
 *  @date   30/05/2006
 */
class TrackCloneCleaner: public GaudiAlgorithm
{

public:

  // Constructors and destructor
  TrackCloneCleaner(const std::string& name,
                    ISvcLocator* pSvcLocator);

  /// Destructor
  virtual ~TrackCloneCleaner();

  /// Initialise
  virtual StatusCode initialize();

  /// Execute
  virtual StatusCode execute();

private:

  /** @class WorkingTrack TrackCloneCleaner.h
   *
   *  Working track object for TrackCloneCleaner algorithm
   *
   *  @author M.Needham
   *  @date   30/05/2006
   */
  class WorkingTrack
  {
  public:
    WorkingTrack() : track(NULL), clone(false) {}
    WorkingTrack( LHCb::Track* _track, const bool _clone = false )
      : track(_track), clone(_clone) {}
    // Access track Chi^2
    inline double chi2() const  { return track->chi2PerDoF(); }
    /// Access number of LHCbIDs
    inline double nLHCbIDs() const { return track->nLHCbIDs(); }
    /// Compare to see if its the same track
    inline bool sameTrack(const LHCb::Track* aTrack) const { return track == aTrack; }
    /// Return the track type ranking
    int trackTypeRank() const;
  public:
    LHCb::Track* track; ///< Pointer to the track object
    bool clone;         ///< Clone flag
  public:
    typedef std::vector<WorkingTrack> Vector;
  };

private:

  std::string m_inputLocation;  ///< Locations of Tracks in TES
  std::string m_linkerLocation; ///< Location of Clone linker in TES
  double m_cloneCut;            ///< Clone cut value

};

#endif
