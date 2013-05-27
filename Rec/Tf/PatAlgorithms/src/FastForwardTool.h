#ifndef FASTFORWARDTOOL_H
#define FASTFORWARDTOOL_H 1

// Include files
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

static const InterfaceID IID_FastForwardTool ( "FastForwardTool", 1, 0 );


/** @class FastForwardTool FastForwardTool.h
 *  Tool to extend one Velo (VeloTT) track through the magnet
 *
 *  @author Olivier Callot
 *  @date   2010-11-02 Rewrite form PatFowardTool
 */

class FastForwardTool : public GaudiTool, virtual public IPatForwardTool,
                        virtual public ITracksFromTrack{
public:

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_FastForwardTool; }

  /// Standard constructor
  FastForwardTool( const std::string& type,
                   const std::string& name,
                   const IInterface* parent);

  virtual ~FastForwardTool( ); ///< Destructor

  virtual void forwardTrack( const LHCb::Track* track, LHCb::Tracks* output );

  virtual StatusCode tracksFromTrack( const LHCb::Track& seed,
                                      std::vector<LHCb::Track*>& tracks );

  virtual StatusCode initialize();

  // added for NNTools
  void setNNSwitch( bool nnSwitch)  { m_nnSwitch = nnSwitch;}
  bool nnSwitch()       const       { return m_nnSwitch;}

  struct sortQuality{
    bool operator()( PatFwdTrackCandidate first, PatFwdTrackCandidate second)
    {
      bool sortDecision = ( first.quality() < second.quality());
      if(first.quality() == second.quality()){
        sortDecision = ( first.chi2PerDoF() < second.chi2PerDoF());
        if(first.chi2PerDoF() == second.chi2PerDoF()){
          sortDecision = ( first.qOverP() < second.qOverP());
        }
      }
      return sortDecision;
    }
  };

  struct LowerByID  {
    bool operator() (const PatForwardHit* lhs, const PatForwardHit* rhs) const { 
      return lhs->planeCode() < rhs->planeCode();
    }
  };

  struct LowerByAbsProjection  {
    bool operator() (const std::pair<double,PatForwardHit*> lhs, const std::pair<double,PatForwardHit*> rhs) const { 
      return lhs.first < rhs.first ; 
    }
  };


protected:

  void buildXCandidatesList( PatFwdTrackCandidate& track );

  void fillXList( PatFwdTrackCandidate& track, double xMin, double xMax );

  bool fitXCandidate ( PatFwdTrackCandidate& track, double maxChi2, int minPlanes, bool isDebug );
  
  bool fillStereoList( PatFwdTrackCandidate& track, double tol );

  bool isClone( PatFwdTrackCandidate& one, PatFwdTrackCandidate& two );

  double getTTOffset ( PatFwdTrackCandidate& track, bool isDebug );
  
  void printTTHit( const PatTTHit* hit );

  void printCoord( const PatForwardHit* hit );

  void printTrack( PatFwdTrackCandidate& track );

  bool matchKey( const PatForwardHit* hit ) {
    if ( m_debugTool ) {
      LHCb::LHCbID id = hit->hit()->lhcbID();
      return m_debugTool->matchKey( id, m_wantedKey );
    }
    return false;
  }

private:

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

  double m_errY;  ///< Estiimated error in Y from the Velo extrapolation

  // setting the cov matrix of the state
  double m_stateErrorX2;
  double m_stateErrorY2;
  double m_stateErrorTX2;
  double m_stateErrorTY2;
  double m_stateErrorP;

  std::vector<PatFwdHits>  m_xHitsPerStation;
  std::vector<double> m_zStation;

  std::vector<PatFwdTrackCandidate> m_candidates;
  double m_ttOffset;

  bool   m_withoutBField;

  static const unsigned int m_nSta = Tf::RegionID::OTIndex::kNStations;
  static const unsigned int m_nLay = Tf::RegionID::OTIndex::kNLayers;
  static const unsigned int m_nReg = Tf::RegionID::OTIndex::kNRegions + Tf::RegionID::ITIndex::kNRegions;
  static const unsigned int m_nOTReg = Tf::RegionID::OTIndex::kNRegions;
  static const unsigned int m_nITReg = Tf::RegionID::ITIndex::kNRegions;

  bool   m_nnSwitch;                   // switch on or off NN var. writing

  std::string  m_debugToolName;
  IPatDebugTool* m_debugTool;
  int m_wantedKey;
  int m_wantedVelo;
};

#endif // FASTFORWARDTOOL_H
