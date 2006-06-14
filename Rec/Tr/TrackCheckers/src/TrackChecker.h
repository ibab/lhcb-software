// $Id: TrackChecker.h,v 1.6 2006-06-14 12:24:28 erodrigu Exp $
#ifndef TRACKCHECKER_H 
#define TRACKCHECKER_H 1

// Include files

// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"

// from Tr/TrackMCInterfaces
#include "TrackMCInterfaces/ITrackCriteriaSelector.h"
#include "TrackMCInterfaces/IIdealStateCreator.h"

// from Tr/TrackInterfaces
#include "TrackInterfaces/ITrackExtrapolator.h"

// Forward declarations
namespace LHCb
{
  class Track;
  class MCParticle;
}

/** @class TrackChecker TrackChecker.h
 *  
 *  This class checks Tracks. It produces histograms of the track
 *  properties like resolutions and hit and track efficiencies.
 *  It also calculates track efficiencies and ghost rates (track averaged and
 *  event averaged) and outputs this to the message service.
 *  This algorithm uses the TrackAssociator from Track to MCParticle.
 *
 *  To select which Tracks and MCParticles should be monitored the
 *  TrackCriteriaSelector tool (called TrackSelector) is used.
 *
 *  There is a flag in the jobOptions to not make True States, which are needed
 *  for the resolution histograms at different z-positions.
 *  This can be usefull when the MCHits are not available.
 *
 *  Adapted to the new Track Event Model, now using Linkers
 *  @author Edwin Bos
 *  @date   2005-11-14
 *
 *  Based on the Tr/TrCheck/TrChecker code by :
 *  @author Jeroen van Tilburg
 *  @date   2003-07-07
 *
 */

class TrackChecker : public GaudiHistoAlg {

public:

  // Standard constructor
  TrackChecker( const std::string& name, ISvcLocator* pSvcLocator );

  // Destructor
  virtual ~TrackChecker();

  // Algorithm initialization
  virtual StatusCode initialize();

  // Algorithm execution
  virtual StatusCode execute();

  // Algorithm finalization
  virtual StatusCode finalize();

private:

  // Helper functions
  StatusCode resolutionHistos( LHCb::Track* track, LHCb::MCParticle* mcPart );
  StatusCode purityHistos    ( LHCb::Track* track, LHCb::MCParticle* mcPart );
  std::string measType( unsigned int type );

  // Interfaces
  ITrackCriteriaSelector* m_trackSelector; // Pointer to TrackCriteriaSelector
  IIdealStateCreator*     m_stateCreator;  // Pointer to IdealStateCreator
  ITrackExtrapolator*     m_extrapolator;  // Pointer to TrackMasterExtrapolator

  // jobOptions
  std::string m_tracksInContainer; // Input Tracks container location
  std::string m_linkerInTable;     // Input Linker table location
  std::string m_trackSelectorName; // Tracks selector tool name
  // Additional z-positions where to determine resolutions and pulls
  std::vector<double> m_zPositions;
  // Produce series of plots for each Measurement type
  bool m_plotsByMeasType;

  // Counters
  int m_nTracks;      // Total # of selected Tracks
  int m_nMCTracks;    // Total # of selected MCParticles
  int m_nAsctTracks;  // Total # of selected Tracks with a MCParticle associated
  // Total # of selected MCParticles with a selectByTrackType Track associated
  int m_nAsctMCTracks;
  double m_evtAveEff;       // Event averaged efficiency
  double m_err2EvtAveEff;   // Error on event averaged efficiency
  double m_evtAveGhost;     // Event averaged ghostrate
  double m_err2EvtAveGhost; // Error on event averaged ghostrate
  int m_nMCEvt;             // Total # of Events containing selected MCParticles
  int m_nEvt;               // Total # of Events containing selected Tracks

};
#endif // TRACKCHECKER_H
