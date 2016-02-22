#ifndef FASTFORWARDTOOL_H
#define FASTFORWARDTOOL_H 1

// Include files
#include <memory>
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "TrackInterfaces/IPatForwardTool.h"
#include "Event/STCluster.h"
#include "TrackInterfaces/IAddTTClusterTool.h"
#include "TrackInterfaces/ITracksFromTrack.h"

#include "STDet/DeSTDetector.h"

#include "Event/Track.h"

#include "FastFwdTool.h"
#include "PatFwdTrackCandidate.h"

#include "PatKernel/PatTStationHitManager.h"
#include "PatKernel/PatForwardHit.h"
#include "TfKernel/TTStationHitManager.h"
#include "PatKernel/PatTTHit.h"

#include "PatKernel/IPatDebugTool.h"


/** @class FastForwardTool FastForwardTool.h
 *  Tool to extend one Velo (VeloTT) track through the magnet
 *
 *  @author Olivier Callot
 *  @date   2010-11-02 Rewrite form PatFowardTool
 */

class FastForwardTool : public extends<GaudiTool, IPatForwardTool, ITracksFromTrack> {
public:

  /// Standard constructor
  FastForwardTool( const std::string& type,
                   const std::string& name,
                   const IInterface* parent);

  ~FastForwardTool( ) override; ///< Destructor

  void forwardTrack( const LHCb::Track& track, LHCb::Tracks& output ) const override;

  StatusCode tracksFromTrack( const LHCb::Track& seed, std::vector<LHCb::Track*>& out) const override;

  StatusCode initialize() override;

  // added for NNTools
  void setNNSwitch( bool nnSwitch) override { m_nnSwitch = nnSwitch;}
  bool nnSwitch()       const       { return m_nnSwitch;}

private:

  void buildXCandidatesList( PatFwdTrackCandidate& track ) const;

  void fillXList( PatFwdTrackCandidate& track, double xMin, double xMax ) const;

  bool fitXCandidate ( PatFwdTrackCandidate& track, double maxChi2, int minPlanes, bool isDebug ) const;
  
  bool fillStereoList( PatFwdTrackCandidate& track, double tol ) const;

  bool isClone( PatFwdTrackCandidate& one, PatFwdTrackCandidate& two ) const;

  double getTTOffset ( PatFwdTrackCandidate& track, bool isDebug ) const;
  
  void printTTHit( const PatTTHit* hit ) const;

  void printCoord( const PatForwardHit* hit ) const;

  void printTrack( const PatFwdTrackCandidate& track ) const;

  bool matchKey( const PatForwardHit* hit ) const {
    if ( m_debugTool ) {
      LHCb::LHCbID id = hit->hit()->lhcbID();
      return m_debugTool->matchKey( id, m_wantedKey );
    }
    return false;
  }

private:
  static const unsigned int m_nSta = Tf::RegionID::OTIndex::kNStations;
  static const unsigned int m_nLay = Tf::RegionID::OTIndex::kNLayers;
  static const unsigned int m_nReg = Tf::RegionID::OTIndex::kNRegions + Tf::RegionID::ITIndex::kNRegions;
  static const unsigned int m_nOTReg = Tf::RegionID::OTIndex::kNRegions;
  static const unsigned int m_nITReg = Tf::RegionID::ITIndex::kNRegions;

  FastFwdTool*                               m_fwdTool;        ///< Tool to compute extrapolations of tracks
  Tf::TStationHitManager <PatForwardHit> *   m_tHitManager;    ///< Tool to provide hits
  Tf::TTStationHitManager <PatTTHit> *       m_ttHitManager;    ///< Tool to provide hits
  IAddTTClusterTool*                         m_addTTClusterTool;
  std::string                                m_addTtToolName;

  //== Parameters of the algorithm
  double m_zAfterVelo;
  double m_yCompatibleTol;
  double m_yCompatibleTolFinal;
  double m_minOTDrift;
  double m_maxOTDrift;
  double m_maxSpreadX;
  double m_maxSpreadY;
  double m_maxSpreadSlopeX;
  double m_maxSpreadSlopeY;
  int    m_minXPlanes;
  int    m_minPlanes;
  double m_xMatchTol;
  double m_minPt;
  double m_minMomentum;
  double m_maxChi2;
  double m_maxChi2Track;
  int    m_minHits;
  int    m_minOTHits;
  double m_centerOTYSize;
  double m_maxDeltaY;
  double m_maxDeltaYSlope;

  double m_rangePerMeV;
  double m_minRange;
  double m_rangeErrorFraction;
  bool   m_withTTEstimate;
  double m_zTTProj;
  double m_zTTField;
  double m_maxTTProj;
  double m_ttToTScale;
  double m_ttToTWindow;

  mutable double m_errY;  ///< Estiimated error in Y from the Velo extrapolation

  // setting the cov matrix of the state
  double m_stateErrorX2;
  double m_stateErrorY2;
  double m_stateErrorTX2;
  double m_stateErrorTY2;
  double m_stateErrorP;

  mutable std::vector<PatFwdHits>  m_xHitsPerStation;
  std::array<double,m_nSta> m_zStation;

  mutable std::vector<PatFwdTrackCandidate> m_candidates;
  mutable double m_ttOffset;

  bool   m_withoutBField;


  bool   m_nnSwitch = false;                   // switch on or off NN var. writing

  std::string  m_debugToolName;
  IPatDebugTool* m_debugTool = nullptr;
  int m_wantedKey;
  int m_wantedVelo;
};

#endif // FASTFORWARDTOOL_H
