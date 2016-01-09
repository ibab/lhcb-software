#ifndef HLTTRACKFILTERGHOSTPROB_H 
#define HLTTRACKFILTERGHOSTPROB_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/extends.h"
#include "GaudiKernel/IIncidentListener.h"
#include "TrackInterfaces/ITracksFromTrack.h"            // Interface
#include "Event/Track.h"

#include "TrackInterfaces/IGhostProbability.h"

/** @class HltTrackFilterGhostProb HltTrackFilterGhostProb.h
 *
 * Class to calculate and filter on the ghost probability in Hlt1.
 *
 *  @author Sascha Stahl
 *  @date   2016-01-08
 */


class HltTrackFilterGhostProb : public extends1<GaudiTool,  ITracksFromTrack >, public IIncidentListener{
public: 
  /// Standard constructor
  HltTrackFilterGhostProb( const std::string& type, 
               const std::string& name,
               const IInterface* parent);

  /// Initialize method
  StatusCode initialize() override;
  ~HltTrackFilterGhostProb( ) override = default; ///< Destructor

  StatusCode tracksFromTrack( const LHCb::Track& seed,
			      std::vector<LHCb::Track*>& tracks ) override;

  void handle ( const Incident& incident );

protected:

private:

  bool m_initializeEvent;
  std::string m_ghostToolName;
  IGhostProbability* m_ghostTool;
  double m_maxGhostProbCut;
  double m_maxChi2PerDoFCut;
};
#endif // HLTTRACKFILTERGHOSTPROB_H
