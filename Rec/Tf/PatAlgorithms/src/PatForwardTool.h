// $Id: PatForwardTool.h,v 1.3 2008-07-17 13:16:49 smenzeme Exp $
#ifndef PATFORWARDTOOL_H
#define PATFORWARDTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "TrackInterfaces/IPatForwardTool.h"
#include "Event/STCluster.h"
#include "TrackInterfaces/IAddTTClusterTool.h"
#include "TrackInterfaces/IAddUTClusterTool.h"
#include "TrackInterfaces/ITracksFromTrack.h"

#include "STDet/DeSTDetector.h"

#include "Event/Track.h"

#include "PatFwdTool.h"
#include "PatFwdTrackCandidate.h"

#include "PatKernel/PatTStationHitManager.h"
#include "PatKernel/PatForwardHit.h"



static const InterfaceID IID_PatForwardTool ( "PatForwardTool", 1, 0 );


/** @class PatForwardTool PatForwardTool.h
 *  Tool to extend one Velo (VeloTT) track through the magnet
 *
 *  @author Olivier Callot
 *  @date   2005-10-04 Initial version
 *  @date   2007-08-20 Update for A-Team framework
 */

class PatForwardTool : public GaudiTool, virtual public IPatForwardTool,
                       virtual public ITracksFromTrack{
public:

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_PatForwardTool; }

  /// Standard constructor
  PatForwardTool( const std::string& type,
                  const std::string& name,
                  const IInterface* parent);

  virtual ~PatForwardTool( ); ///< Destructor

  virtual void forwardTrack( const LHCb::Track* track, LHCb::Tracks* output );

  virtual StatusCode tracksFromTrack( const LHCb::Track& seed,
                                      std::vector<LHCb::Track*>& tracks );

  virtual StatusCode initialize();

  // added for NNTools
  void setNNSwitch( bool nnSwitch)  { m_nnSwitch = nnSwitch;}
  bool nnSwitch()       const       { return m_nnSwitch;}

  struct sortQuality{
    bool operator()( const PatFwdTrackCandidate& first, 
                     const PatFwdTrackCandidate& second )
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


protected:

  void buildXCandidatesList( PatFwdTrackCandidate& track );

  void fillXList ( PatFwdTrackCandidate& track, 
                   double kick, double maxRangeRef, double zMagnet );
  


  bool fillStereoList( PatFwdTrackCandidate& track, double tol );

  void debugFwdHits( PatFwdTrackCandidate& track );

  void debugFwdHits( PatFwdTrackCandidate& track, MsgStream& msg );

private:

  PatFwdTool*                                 m_fwdTool;        ///< Tool to compute extrapolations of tracks
  Tf::TStationHitManager <PatForwardHit> *   m_tHitManager;    ///< Tool to provide hits
  IAddTTClusterTool*                          m_addTTClusterTool;
  std::string                                 m_addTtToolName;
  IAddUTClusterTool*                          m_addUTClusterTool;
  std::string                                 m_addUtToolName;

  bool m_useUT;

  //== Parameters of the algorithm
  bool   m_secondLoop;
  bool   m_useMomentumEstimate;
  double m_momentumEstimateError;
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
  double m_minPt;
  double m_minMomentum;
  double m_maxChi2;
  double m_maxChi2Track;
  int    m_minHits;
  int    m_minOTHits;
  double m_centerOTYSize;
  double m_maxDeltaY;
  double m_maxDeltaYSlope;
  int    m_maxXCandidateSize;


  std::vector<double>  m_magnetKickParams ;
  double m_minRange;



  // setting the cov matrix of the state
  double m_stateErrorX2;
  double m_stateErrorY2;
  double m_stateErrorTX2;
  double m_stateErrorTY2;
  double m_stateErrorP;

  PatFwdHits  m_xHitsAtReference;

  std::vector<PatFwdTrackCandidate> m_candidates;

  bool   m_withoutBField;

  static const unsigned int m_nSta = Tf::RegionID::OTIndex::kNStations;
  static const unsigned int m_nLay = Tf::RegionID::OTIndex::kNLayers;
  static const unsigned int m_nReg = Tf::RegionID::OTIndex::kNRegions +
    Tf::RegionID::ITIndex::kNRegions;
  static const unsigned int m_nOTReg = Tf::RegionID::OTIndex::kNRegions;
  static const unsigned int m_nITReg = Tf::RegionID::ITIndex::kNRegions;

  bool   m_nnSwitch;                   // switch on or off NN var. writing
};

#endif // PATFORWARDTOOL_H
