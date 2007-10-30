// $Id: AlignSaveTuple.h,v 1.2 2007-10-30 11:22:35 lnicolas Exp $
#ifndef _AlignSaveTuple_H_
#define _AlignSaveTuple_H_

/** @class AlignSaveTuple AlignSaveTuple Align/AlignTrTools/AlignSaveTuple.h
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
#include "Event/FitNode.h"
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

class AlignSaveTuple: public GaudiTupleAlg{
  friend class AlgFactory<AlignSaveTuple>;

public:

  // Constructors and destructor
  AlignSaveTuple(const std::string& name, ISvcLocator* pSvcLocator); 
  virtual ~AlignSaveTuple();  

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

private:

  typedef std::vector<double> Array;
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
  bool isSharedHit ( const LHCb::Track* aTrack,
                     const LHCb::Node* aNode );
  
  // Is the track isolated?
  std::vector<LHCb::LHCbID> getCloseHits ( );
  int nNeighbouringHits ( const LHCb::Track* aTrack );
  
  // Is the strip/straw trajectory too close to the track at z?
  bool isNeighbouringHit ( LHCb::STChannelID clusID,
                           LHCb::STChannelID hitID );
  bool isNeighbouringHit ( LHCb::OTChannelID timeID,
                           LHCb::OTChannelID hitID );  
  
  // Get the box overlaps for this track
  double boxOverlap ( const LHCb::Track* aTrack,
                      LHCb::STChannelID STChanID1,
                      LHCb::STChannelID STChanID2 );
  

  // Fit track given by a set of hits with straight line,
  // using chi2 minimisation and return fit parameters
  std::vector<double> fitTrackPiece ( const LHCb::Track* aTrack,
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
  mutable std::vector<LHCb::LHCbID> m_closeHits;
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

  double m_entryTX;
  double m_entryTY;
  double m_entryX;
  double m_entryY;

  // Hits Variables
  Array m_res;
  Array m_errRes;

//   Array m_hitX;
//   Array m_hitY;
//   Array m_hitZ;

//   Array m_hitLocalX;
//   Array m_hitLocalY;
//   Array m_hitLocalZ;

  int m_nLadOverlaps;
  Array m_ladOvlapRes;
  Array m_ladOvlapLay;

  int m_nBoxOverlaps;
  Array m_boxOvlapRes;
  Array m_boxOvlapSta;

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

#endif // _AlignSaveTuple_H_
