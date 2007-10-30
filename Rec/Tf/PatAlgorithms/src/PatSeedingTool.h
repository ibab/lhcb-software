// $Id: PatSeedingTool.h,v 1.2 2007-10-30 18:29:29 smenzeme Exp $
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

    virtual StatusCode performTracking (LHCb::Tracks* output);

    virtual StatusCode performTracking (std::vector<LHCb::Track*>& outputTracks);

    virtual void prepareHits();

 

  protected:
    void debugFwdHit ( PatFwdHit* coord, MsgStream& msg );

    bool addIfBetter( PatSeedTrack& track, std::vector<PatSeedTrack>& candidates );

    void findXCandidates ( unsigned int lay, unsigned int typ, std::vector<PatSeedTrack>& candidates);

    void collectStereoHits ( PatSeedTrack& track, unsigned int typ, PatFwdHits& stereo );

    bool findBestRange ( unsigned int typ, int minNbPlanes, PatFwdHits& stereo );

    bool fitLineInY ( PatFwdHits& stereo, double& y0, double& sl);

    void storeTrack( PatSeedTrack& track, std::vector<LHCb::Track*>& outputTracks );

    void processTracks( std::string name, std::vector<LHCb::Track*>& outputTracks );



  private:
    std::string      m_inputTracksName;

    double m_zMagnet;
    double m_xMagTol;
    double m_tolCollectOT;
    double m_tolCollectIT;
    double m_initialArrow;
    double m_zReference;
    double m_zOutput;
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

    static const unsigned int m_nSta = Tf::RegionID::OTIndex::kNStations;
    static const unsigned int m_nLay = Tf::RegionID::OTIndex::kNLayers;
    static const unsigned int m_nReg = Tf::RegionID::OTIndex::kNRegions +
    Tf::RegionID::ITIndex::kNRegions;
    static const unsigned int m_nOTReg = Tf::RegionID::OTIndex::kNRegions;
    static const unsigned int m_nITReg = Tf::RegionID::ITIndex::kNRegions;

  };

#endif // PATSEEDINGTOOL_H
