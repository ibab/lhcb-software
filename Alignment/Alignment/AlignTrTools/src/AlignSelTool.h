// $Id: AlignSelTool.h,v 1.2 2007-04-25 14:07:06 jblouw Exp $
#ifndef ALIGNTRTOOLS_ALIGNSELTOOL_H 
#define ALIGNTRTOOLS_ALIGNSELTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// Interface
#include "AlignmentInterfaces/IAlignSelTool.h"

#include "TrackInterfaces/ITrackExtrapolator.h"

// #include "OTDet/DeOTDetector.h"
// #include "OTDet/DeOTModule.h"

// #include "STDet/DeSTDetector.h"
// #include "STDet/DeITLayer.h"

#include "Event/Track.h"
#include "Event/OTTime.h"
#include "Event/STLiteCluster.h"

// Other
#include "gsl/gsl_cdf.h"

/** @class AlignSelTool AlignSelTool.h
 *  
 *
 *  @author Louis Nicolas
 *  @date   2006-12-01
 */
class AlignSelTool : public GaudiTool, virtual public IAlignSelTool {
public: 
  /// Standard constructor
  AlignSelTool( const std::string& type, 
                const std::string& name,
                const IInterface* parent);

  virtual ~AlignSelTool( ); ///< Destructor

  StatusCode AlignSelTool::initialize();

  virtual bool AlignSelTool::selectTrack(LHCb::Track* aTrack);

protected:

private:

  typedef LHCb::STLiteCluster::STLiteClusters STLiteClusters;

  const int m_nStations;
  const int m_nLayers;
  const int m_nStrips;

  const int m_nSubLayers;
  const int m_nModules;
  const int m_nStraws;

  bool m_fieldOn;

  double m_otCloseHitTol;
  double m_nStrawsTol;
  double m_itCloseHitTol;
  double m_nStripsTol;

  //=============================================================================
  // Defining the cuts
  //=============================================================================
  double c_minP;
  double c_maxChi2PerDoF;
  double c_minChi2Prob;
  int c_maxNHoles;
  int c_maxNSharedHits;
  int c_maxNCloseHits;
  //=============================================================================

  ITrackExtrapolator* m_extrapolator; ///< Interface to track extrapolator

//   DeOTDetector* m_otTracker;           ///< Pointer to the OT XML geom
//   std::string   m_otTrackerPath;       ///< Name of the OT XML geom path
//   DeSTDetector* m_itTracker;           ///< Pointer to the IT XML geom
//   std::string   m_itTrackerPath;       ///< Name of the IT XML geom path

  std::string m_tracksPath;
  LHCb::Track theTrack;

  std::string m_itClustersPath;
  STLiteClusters* m_itClusters;
  std::string m_otTimesPath;
  const LHCb::OTTimes* m_otTimes;

  double m_trP;
  double m_trChi2PerDoF;
  double m_trChi2Prob;
  int m_nHoles;
  int m_nSharedHits;
  int m_nCloseHits;

  int m_nITDoubleHits;
  int m_nOTDoubleHits;
  int m_nLadOverlaps;
  int m_nBoxOverlaps;

  static const int defValue = -999999;

  //=============================================================================
  // Getting some important variables
  //=============================================================================
  int AlignSelTool::getAllVariables ( LHCb::Track* aTrack );
  bool AlignSelTool::isSharedHit ( const LHCb::Track* aTrack,
                                   const LHCb::Node* aNode );
  int AlignSelTool::nNeighbouringHits ( const LHCb::Track* aTrack );
  bool AlignSelTool::isNeighbouringHit ( LHCb::OTChannelID timeID,
                                         LHCb::OTChannelID hitID,
                                         unsigned int hitStraw );  
  //=============================================================================

  //=============================================================================
  // Cutting on some variables
  //=============================================================================
  bool AlignSelTool::cutTrackP ( );
  bool AlignSelTool::cutTrackChi2PerDoF ( );
  bool AlignSelTool::cutTrackChi2Prob ( );
  bool AlignSelTool::cutNHoles ( );
  bool AlignSelTool::cutNSharedHits ( );
  bool AlignSelTool::cutNCloseHits ( );
  //=============================================================================
};
#endif // ALIGNTRTOOLS_ALIGNSELTOOL_H
