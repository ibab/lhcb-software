// $Id: PatSeedingTool.h,v 1.5 2008-02-18 14:39:53 mschille Exp $
#ifndef PATSEEDINGTOOL_H
#define PATSEEDINGTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiAlg/ISequencerTimerTool.h"

#include "TrackInterfaces/IPatSeedingTool.h"
#include "TrackInterfaces/ITracksFromTrack.h"

#include "PatSeedTool.h"
#include "PatSeedTrack.h"
#include "PatKernel/PatForwardHit.h"
#include "Event/Track.h"

#include "TfKernel/RegionID.h"
#include "TfKernel/RecoFuncs.h"
#include "TfKernel/TStationHitManager.h"

namespace LHCb {
  class State;
}

static const InterfaceID IID_PatSeedingTool ( "PatSeedingTool", 1, 0 );


  /** @class PatSeedingTool PatSeedingTool.h
   *
   *  Pat Seeding tool. Used internally by Pat Seeding.
   *
   *  @author Olivier Callot
   *  @date   2006-10-13 Initial version
   *  @date   2007-08-20 Update for a-team framework 
   *
   */

  class PatSeedingTool : public GaudiTool,  virtual public IPatSeedingTool,
                         virtual public ITracksFromTrack {

  public:

    // Return the interface ID
    static const InterfaceID& interfaceID() { return IID_PatSeedingTool; }

    /// Standard constructor
    PatSeedingTool( const std::string& type,
                    const std::string& name,
                    const IInterface* parent);

    virtual ~PatSeedingTool( );

    virtual StatusCode initialize();

    virtual StatusCode tracksFromTrack(const LHCb::Track&,
                                       std::vector<LHCb::Track*, std::allocator<LHCb::Track*> >&);

    virtual StatusCode performTracking (LHCb::Tracks* output,
		    const LHCb::State* state = 0);

    virtual StatusCode performTracking (std::vector<LHCb::Track*>& outputTracks,
		    const LHCb::State* state = 0);

    virtual void prepareHits();

 

  protected:
    void debugFwdHit ( PatFwdHit* coord, MsgStream& msg );

    bool addIfBetter( PatSeedTrack& track, std::vector<PatSeedTrack>& candidates );

    void findXCandidates ( unsigned int lay, unsigned int typ,
		    std::vector<PatSeedTrack>& candidates,
		    const LHCb::State* state );

    void collectStereoHits ( PatSeedTrack& track, unsigned int typ,
		    PatFwdHits& stereo, const LHCb::State* state );

    bool findBestRange ( unsigned int typ, int minNbPlanes, PatFwdHits& stereo );

    bool fitLineInY ( PatFwdHits& stereo, double& y0, double& sl );

    void storeTrack( PatSeedTrack& track, std::vector<LHCb::Track*>& outputTracks );

    void processTracks( std::string name, std::vector<LHCb::Track*>& outputTracks );

    bool isIsolated(PatFwdHits::const_iterator it,
		const Tf::TStationHitManager<PatForwardHit>::HitRange& range);

    // two methods to narrow down given search windows using the information
    // in the state
    void updateSearchWindowX(double zref,
		double& xmin, double& xmax, const LHCb::State* state);
    void updateSearchWindowY(double zref, double xref, double xreftol,
		double cosT, double sinT,
		double& xmin, double& xmax, const LHCb::State* state);

  private:
    std::string      m_inputTracksName;

    double m_zMagnet;
    double m_xMagTol;
    double m_tolCollectOT;
    double m_tolCollectIT;
    double m_initialArrow;
    double m_zReference;
    std::vector<double> m_zOutputs;
    double m_dRatio;
    double m_tolExtrapolate;

    int    m_minXPlanes;
    double m_curveTol;
    double m_commonXFraction;
    double m_maxRangeOT;
    double m_maxRangeIT;
    double m_maxChi2HitOT;
    double m_maxChi2HitIT;
    int m_minTotalPlanes;
    double m_maxTrackChi2;
    double m_maxFinalChi2;
    double m_maxFinalTrackChi2;
    double m_maxYAtOrigin;
    double m_yCorrection;

    double m_stateErrorX2;
    double m_stateErrorY2;
    double m_stateErrorTX2;
    double m_stateErrorTY2;
    double m_stateErrorP;
    double m_momentumScale;
    std::vector< double > m_momentumParams;
    double m_minMomentum;

    double m_tolCollect;
    double m_maxChi2Hit;
    double m_zForYMatch;

    Tf::TStationHitManager <PatForwardHit> *  m_tHitManager;    ///< Tool to provide hits

    PatSeedTool*       m_seedTool;

    double m_wireVelocity;
    double m_driftVelocity;
    double m_maxImpact;

    bool m_measureTime;
    ISequencerTimerTool* m_timer;
    int m_timeInit;
    int m_timeX;
    int m_timeStereo;
    int m_timeItOt;
    bool m_useForward;
    std::vector<std::vector<LHCb::LHCbID> > m_foundIds;

    bool m_printing;

    // turns on some tuning for B field off
    bool m_fieldOff;

    static const unsigned int m_nSta = Tf::RegionID::OTIndex::kNStations;
    static const unsigned int m_nLay = Tf::RegionID::OTIndex::kNLayers;
    static const unsigned int m_nReg = Tf::RegionID::OTIndex::kNRegions +
    Tf::RegionID::ITIndex::kNRegions;
    static const unsigned int m_nOTReg = Tf::RegionID::OTIndex::kNRegions;
    static const unsigned int m_nITReg = Tf::RegionID::ITIndex::kNRegions;

    // maximum distance of two tracks to be considered for pattern reco
    // clone killing
    double m_cloneMaxXDist;

    // chi^2 cutoff for tracks with few OT hits
    double m_maxTrackChi2LowMult;

    // weighting for quality variable:
    // first element is for number of coordinates
    // second element is for chi^2
    std::vector<double> m_qualityWeights;
    
    // maximum number of missed layers during x search
    int m_maxMisses;

    // shall we enforce isolation criteria?
    bool m_enforceIsolation;
    // hits/clusters must be isolated in a window around their mean position
    // (window extends for m_[IO]TIsolation in both directions)
    double m_OTIsolation;
    double m_ITIsolation;
    // true (default) if tracks found by a previous instance of PatSeedingTool
    // should be reused by this instance
    bool m_reusePatSeeding;

  };

#endif // PATSEEDINGTOOL_H
