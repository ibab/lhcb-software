// $Id: AlignTrackSel.h,v 1.1 2006-10-02 15:20:37 lnicolas Exp $
#ifndef _AlignTrackSel_H_
#define _AlignTrackSel_H_

/** @class AlignTrackSel AlignTrackSel Tr/AlignTrackSel.h
 *
 *  First tests on tracks (goal is to save a NTuple)
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
#include "GaudiAlg/Tuple.h"

// Linkers
#include "Linker/LinkerTool.h"
#include "Linker/LinkedTo.h"

// Event
#include "Event/MCHeader.h"
#include "Event/MCParticle.h"
#include "Event/Track.h"
#include "Event/STCluster.h"
#include "Event/MuonPID.h"
#include "Event/STMeasurement.h"

// Track
#include "TrackInterfaces/ITrackExtrapolator.h"
// #include "TrackInterfaces/ITrackSelector.h"

// STDet
#include "STDet/DeITDetector.h"
#include "STDet/DeITLayer.h"

// Other
#include "gsl/gsl_cdf.h"
#include <vector>
#include <list>
//===========================================================================

class ITrackExtrapolator;
// class ITrackSelector;

class AlignTrackSel: public GaudiTupleAlg{
  friend class AlgFactory<AlignTrackSel> ;

public:

  // Constructors and destructor
  AlignTrackSel(const std::string& name, ISvcLocator* pSvcLocator); 
  virtual ~AlignTrackSel();  

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

  // Fill the Variables
  StatusCode fillVariables( const LHCb::Track* aTrack,
                            std::vector<const LHCb::Node*> theNodes,
                            Tuples::Tuple trackSelTuple );

  // Is the track linked to a MC particle or not?
  bool isGhostTrack( const LHCb::Track* aTrack );

  // Check if track can be matched to a muon
  bool AlignTrackSel::isMuonTrack( const LHCb::Track* aTrack );

  // Check if the hit is being shared with (at least) one other Track
  bool AlignTrackSel::isSharedHit( const LHCb::Track* aTrack,
                                   const LHCb::Node* aNode );

  // Is the track isolated?
  int nNeighbouringHits( const LHCb::Track* aTrack,
                         std::vector<const LHCb::Node*> theNodes );

  // Get the box overlaps for this track (given by the itNodes)
  std::vector<double>
  AlignTrackSel::boxOverlap( const LHCb::Track* aTrack,
                             std::vector<const LHCb::Node*> theNodes,
                             LHCb::STChannelID STChanID1,
                             LHCb::STChannelID STChanID2 );


  // Fit track given by a set of hits with straight line,
  // using chi2 minimisation and return fit parameters
  std::vector<double>
  AlignTrackSel::fitTrackPiece( const LHCb::Track* aTrack,
                                std::vector<const LHCb::Node*> hitsOverlapBox );
  
  // Write the NTuple
  StatusCode writeNTuple( Tuples::Tuple trackSelTuple );

  //======================================================================
  // Properties
  //======================================================================
  DeITDetector* m_itTracker;           ///< Pointer to the IT XML geom
  std::string   m_itTrackerPath;       ///< Name of the IT XML geom path

  const int m_nStations;
  const int m_nBoxes;
  const int m_nLayers;
  const int m_nLadders;
  const int m_nStrips;

  int m_nMaxITHits;
  int m_nStripsTol;

//   std::string m_selectorName; 
//   ITrackSelector* m_selector;

  std::string m_tracksPath;
  std::string m_clustersPath;
  std::string m_muonPIDsPath;
  //======================================================================

  ITrackExtrapolator* m_extrapolator;

  const LHCb::Tracks* m_tracks;
  const LHCb::MCHeader* m_evtHeader;
  const LHCb::MuonPIDs* m_muIDs;
  const LHCb::STClusters* m_clusters;

  DirectTable* m_directTable;

  // Global Variables
  long m_nEvents;

  // Track Variables
  int m_nGoodTracks;

  bool m_isAGhost;
  bool m_isAMuon;

  int m_nITHits;
  int m_nDoubleHits;
  int m_nSharedHits;
  double m_fSharedHits;
  int m_nCloseHits;
  int m_nHoles;

  double m_trackChi2PerDoF;
  double m_trackChi2Prob;

  double m_trackP;
  double m_trackPt;
  double m_trackMCP;
  double m_trackMCPt;
  double m_trackErrP;

  // Hits Variables
  int m_nRes;
  Array m_res;
  Array m_errRes;

  Array m_hitX;
  Array m_hitY;
  Array m_hitZ;

  Array m_tx;
  Array m_ty;

  Array m_lToHit;

  int m_nLadOverlaps;
  Array m_ladOverlaps;
  Array m_ladOverlapsZ;

  int m_nBoxOverlaps;
  Matrix m_boxOverlaps;
};

#endif // _AlignTrackSel_H_
