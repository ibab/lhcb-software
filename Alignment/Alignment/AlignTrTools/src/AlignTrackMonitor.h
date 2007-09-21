// $Id: AlignTrackMonitor.h,v 1.3 2007-09-21 15:45:52 lnicolas Exp $
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
#include "GaudiAlg/GaudiTupleAlg.h"

// Linkers
#include "Linker/LinkerTool.h"
#include "Linker/LinkedTo.h"

// Event
#include "Event/MCParticle.h"
#include "Event/Track.h"
#include "Event/STLiteCluster.h"
#include "Event/OTTime.h"
#include "Event/ODIN.h"
//===========================================================================

//===========================================================================
// Forward declarations
//===========================================================================
class ITrackExtrapolator;
class IMagneticFieldSvc;
class ITrackCloneFinder;
class ITrackGhostClassification;
class GhostTrackInfo;

class DeOTDetector;
class DeSTDetector;
class DeITDetector;
//===========================================================================

class AlignTrackMonitor: public GaudiTupleAlg{
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
  typedef std::vector<Array> Matrix;
  typedef LinkerTool<LHCb::Track, LHCb::MCParticle> AsctTool;
  typedef AsctTool::DirectType DirectTable;
  typedef DirectTable::Range DirectRange;
  typedef DirectTable::iterator direct_iterator;
  typedef LHCb::STLiteCluster::STLiteClusters STLiteClusters;

  static const int defValue = -999999;
  
  // Fill the Variables
  StatusCode fillVariables ( const LHCb::Track* aTrack,
                             Tuples::Tuple trackSelTuple );

  // Is the track linked to a MC particle or not?
  bool isGhostTrack ( const LHCb::Track* aTrack );

  // Check if the hit is being shared with (at least) one other Track
  bool AlignTrackMonitor::isSharedHit ( const LHCb::Track* aTrack,
                                        const LHCb::Node* aNode );

  // Is the track isolated?
  int AlignTrackMonitor::nNeighbouringHits ( const LHCb::Track* aTrack );

  // Is the strip/straw trajectory too close to the track at z?
  bool AlignTrackMonitor::isNeighbouringHit ( LHCb::STChannelID clusID,
                                              LHCb::STChannelID hitID,
                                              unsigned int hitStrip );
  bool AlignTrackMonitor::isNeighbouringHit ( LHCb::OTChannelID timeID,
                                              LHCb::OTChannelID hitID,
                                              unsigned int hitStraw );  

  // Get the box overlaps for this track (given by the itNodes)
  std::vector<double>
  AlignTrackMonitor::boxOverlap ( const LHCb::Track* aTrack,
                                  LHCb::STChannelID STChanID1,
                                  LHCb::STChannelID STChanID2 );
  

  // Fit track given by a set of hits with straight line,
  // using chi2 minimisation and return fit parameters
  std::vector<double>
  AlignTrackMonitor::fitTrackPiece ( const LHCb::Track* aTrack,
                                     std::vector<LHCb::Node*> hitsOverlapBox );
  
  // Write the NTuple
  StatusCode writeNTuple ( Tuples::Tuple trackSelTuple );
  
  //======================================================================
  // Properties
  //======================================================================
  DeOTDetector* m_otTracker;           ///< Pointer to the OT XML geom
  std::string   m_otTrackerPath;       ///< Name of the OT XML geom path
  DeITDetector* m_itTracker;           ///< Pointer to the IT XML geom
  std::string   m_itTrackerPath;       ///< Name of the IT XML geom path

  int m_nStations;

  int m_nOTModules;
  int m_nStraws;

  int m_nLayers;
  int m_nStrips;

  int m_maxNHits;

  double m_nStripsTol;
  double m_nStrawsTol;

  std::string m_tracksPath;
  std::string m_itClustersPath;
  std::string m_otTimesPath;

  std::string m_cloneFinderName;
  std::string m_ghostToolName;
  //======================================================================

  ITrackExtrapolator* m_extrapolator; ///< Interface to track extrapolator
  ITrackCloneFinder* m_cloneFinder; ///< Interface to clone finder tool
  IMagneticFieldSvc* m_pIMF; ///< Pointer to the magn. field service
  ITrackGhostClassification* m_ghostClassification;  ///< Pointer to ghost tool 

  const LHCb::Tracks* m_tracks;
  const LHCb::ODIN * odin;
  const STLiteClusters* m_itClusters;
  const LHCb::OTTimes* m_otTimes;

  DirectTable* m_directTable;
  bool m_mcData;

  // Global Variables
  long m_iEvent;
  long m_evtNr;
  long m_runNr;
  int m_eventMultiplicity;
  double m_ghostRate;

  // Track Variables
  int m_iGoodTrack;

  bool m_isGhostTrack;
  int m_ghostTrackClassification;

  int m_nITHits;
  int m_nOTHits;
  int m_nSharedHits;
  double m_fSharedHits;
  int m_nCloseHits;
  int m_nHoles;

  double m_trackChi2PerDoF;
  double m_trackChi2Prob;

  double m_trackP;
  double m_trackPt;
  double m_trackErrP;
  double m_trackMCP;
  double m_trackMCPt;

  double m_tx;
  double m_ty;

  // Hits Variables
  int m_nRes;
  Array m_res;
  Array m_errRes;

  Array m_hitX;
  Array m_hitY;
  Array m_hitZ;

  Array m_hitLocalX;
  Array m_hitLocalY;
  Array m_hitLocalZ;

  int m_nLadOverlaps;
  Array m_ladOverlaps;
  Array m_ladOverlapsZ;

  int m_nBoxOverlaps;
  Matrix m_boxOverlaps;
};

#endif // _AlignTrackMonitor_H_
