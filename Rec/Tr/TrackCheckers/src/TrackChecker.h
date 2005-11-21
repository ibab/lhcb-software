#ifndef TRACKCHECKER_H 
#define TRACKCHECKER_H 1

// Include files

// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"

// from Velo/VeloAssociators/VeloAssociators
#include "VeloAssociators/VeloCluster2MCParticleAsct.h"

// from  IT/ITAssociators/ITAssociators
#include "ITAssociators/ITCluster2MCParticleAsct.h"

// from OT/OTAssociators/OTAssociators
#include "OTAssociators/OTTime2MCParticleAsct.h"

// from Tr/TrackInterfaces
#include "TrackInterfaces/ITrackCriteriaSelector.h"
#include "TrackInterfaces/IIdealStateCreator.h"
#include "TrackInterfaces/ITrackExtrapolator.h"

// Forward declarations

class Track;
class MCParticle;

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

  // Helper function
  StatusCode resolutionHistos( Track* track, MCParticle* mcPart );
  StatusCode purityHistos( Track* track, MCParticle* mcPart );

  // Typedefs for readability
  typedef ITCluster2MCParticleAsct::IAsct   ITClusAsct;
  typedef OTTime2MCParticleAsct::IAsct      OTTimAsct;
  typedef VeloCluster2MCParticleAsct::IAsct VeloClusAsct;
  
  // Member pointers to retrieved associator tools
  VeloClusAsct* m_veloClusToMCP;
  ITClusAsct*   m_itClusToMCP;
  OTTimAsct*    m_otTimToMCP;

  // Interfaces
  ITrackCriteriaSelector* m_trackSelector; // Pointer to TrackCriteriaSelector
  IIdealStateCreator* m_stateCreator;      // Pointer to IdealStateCreator
  ITrackExtrapolator* m_extrapolatorL;     // Pointer to LinearExtrapolator
  ITrackExtrapolator* m_extrapolatorM;     // Pointer to MasterExtrapolator

  // jobOptions
  std::string m_tracksInContainer; // Input Tracks container location
  std::string m_linkerInTable;     // Input Linker table location without "Link/"
  // Additional z-positions where to determine resolutions and pulls
  std::vector<double> m_zPositions;

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
