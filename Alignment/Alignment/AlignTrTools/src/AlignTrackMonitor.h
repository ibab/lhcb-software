// $Id: AlignTrackMonitor.h,v 1.11 2009-10-09 12:00:00 wouter Exp $
#ifndef _AlignTrackMonitor_H_
#define _AlignTrackMonitor_H_

/** @class AlignTrackMonitor AlignTrackMonitor Align/AlignTrTools/AlignTrackMonitor.h
 *
 *  Provide a monitoring tool for T-Station alignment studies
 *
 *  @author L. Nicolas
 *  @date   13/06/2006
 */

//===========================================================================
// Includes
//===========================================================================
// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiKernel/ToolHandle.h"

// Linkers
#include "Linker/LinkerTool.h"
#include "Linker/LinkedTo.h"

// Event
#include "Event/Track.h"
#include "Event/STLiteCluster.h"
#include "Event/VeloCluster.h"
#include "Event/OTTime.h"
#include "Event/FitNode.h"

// Interface
#include "TrackInterfaces/ITrackExtrapolator.h"
#include "TrackInterfaces/IHitExpectation.h"

//===========================================================================

//===========================================================================
// Forward declarations
//===========================================================================
class ITrackExtrapolator;
class IMagneticFieldSvc;

class DeOTDetector;
class DeSTDetector;
class DeITDetector;
//===========================================================================

class AlignTrackMonitor: public GaudiHistoAlg{
  friend class AlgFactory<AlignTrackMonitor>;

public:

  // Constructors and destructor
  AlignTrackMonitor(const std::string& name, ISvcLocator* pSvcLocator); 
  virtual ~AlignTrackMonitor();  

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

private:

  typedef std::vector<double> Array;
  typedef LHCb::STLiteCluster::STLiteClusters STLiteClusters;

  static const int defValue = -999999;
  
  // Fill the Variables
  StatusCode fillVariables ( const LHCb::Track* aTrack );

  // Check if the hit is being shared with (at least) one other Track
  void getSharedHits ( );
  bool isSharedHit ( const LHCb::Track* aTrack,
                     const LHCb::LHCbID aID );
  
  // Is the track isolated?
  void getCloseHits ( );
  // Is the strip/straw trajectory too close to the track at z?
  bool isNeighbouringHit ( LHCb::STChannelID clusID,
                           LHCb::STChannelID hitID );
  bool isNeighbouringHit ( LHCb::OTChannelID timeID,
                           LHCb::OTChannelID hitID );
  
  // Get the box overlaps for this track (given by the itNodes)
  double boxOverlap ( const LHCb::Track* aTrack,
                      LHCb::STChannelID STChanID1,
                      LHCb::STChannelID STChanID2 );
  

  // Fit track given by a set of hits with straight line,
  // using chi2 minimisation and return fit parameters
  std::vector<double> fitTrackPiece ( const LHCb::Track* aTrack,
                                      std::vector<const LHCb::Node*> hitsOverlapBox );
  
  // Fill the histos
  StatusCode fillHistos ( );
  
  //======================================================================
  // Properties
  //======================================================================
  std::string m_otTrackerPath;       ///< Name of the OT XML geom path
  std::string m_itTrackerPath;       ///< Name of the IT XML geom path

  std::string m_tracksPath;
  std::string m_itClustersPath;
  std::string m_otTimesPath;

  double m_nStripsTol;
  double m_nStrawsTol;
  //======================================================================

  //======================================================================
  // Geometry constants
  //======================================================================
  int m_nStations;
  int m_nOTModules;
  int m_nStraws;
  int m_nLayers;
  int m_nStrips;
  //======================================================================

  ToolHandle<ITrackExtrapolator> m_extrapolator; // Interface to track extrapolator
  ToolHandle<IHitExpectation> m_itHitExpectation ;
  ToolHandle<IHitExpectation> m_otHitExpectation ;

  IMagneticFieldSvc* m_pIMF; ///< Pointer to the magn. field service
  DeOTDetector* m_otTracker;           ///< Pointer to the OT XML geom
  DeITDetector* m_itTracker;           ///< Pointer to the IT XML geom

  // Various containers
  const LHCb::Tracks* m_tracks;
  const STLiteClusters* m_itClusters;
  const LHCb::OTTimes* m_otTimes;
  mutable std::vector<LHCb::LHCbID> m_closeHits;
  mutable std::vector<LHCb::LHCbID> m_sharedHits;

  // event variables
  int m_eventMultiplicity;
  int m_nITClusters;
  int m_nVeloClusters;

  // Various counters
  unsigned int m_nITHits;
  unsigned int m_nOTHits;
  unsigned int m_nSharedHits;
  double m_fSharedHits;
  unsigned int m_nCloseHits;
  unsigned int m_nHoles;
  unsigned int m_nLadOverlaps;
  unsigned int m_nBoxOverlaps;

  // Track "quality"
  double m_trackChi2PerDoF;
  double m_trackChi2Prob;

  // Track momentum and more
  double m_trackP;
  double m_trackPt;
  double m_trackErrP;

  // Track pseudo-rapidity
  double m_trackEta;

  // Track entry point in T-station
  double m_entryTX;
  double m_entryTY;
  double m_entryX;
  double m_entryY;

  //=============================================================================
  // for comparing LHCbIDs
  //=============================================================================
  class lessByID {
  public:
    inline bool operator() ( const LHCb::LHCbID& obj1 ,
                             const LHCb::LHCbID& obj2 ) const { 
      return obj1.lhcbID() < obj2.lhcbID() ; 
    }
  };
  //=============================================================================
};

#endif // _AlignTrackMonitor_H_
