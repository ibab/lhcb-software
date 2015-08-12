// Include files

#include <cmath>
#include <limits>
#include <algorithm>
#include <functional>
#include <unordered_map>
#include <array>

// from Gaudi
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/SystemOfUnits.h"

// from TrackEvent
#include "Event/StateParameters.h"
#include "Event/ProcStatus.h"

// local
#include "PatSeedingTool.h"
#include "PatFwdFitLine.h"
#include "PatFwdPlaneCounter.h"

#include "TfKernel/RegionID.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatSeedingTool
//
// 2006-10-13 : Olivier Callot
//-----------------------------------------------------------------------------


// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( PatSeedingTool )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatSeedingTool::PatSeedingTool(const std::string& type,
    const std::string& name, const IInterface* parent)
  : base_class(type, name , parent),
  m_magFieldSvc(nullptr), m_tHitManager(nullptr), m_seedTool(nullptr),
  m_timer(nullptr), m_momentumTool(nullptr), m_online(false)
{

  declareInterface<IPatSeedingTool>(this);
  declareInterface<ITracksFromTrack>(this);

  // NOTE: some properties are deliberately set to an unreasonable value
  // (-1e42) because we want to supply defaults later in initialize
  // if the user did not specify the values via job options
  // we have to wait until initialize because we need the field service
  // in an initialized state to decide which defaults to fill in

  //------------------------------------------------------------------------
  // hit level stuff
  //------------------------------------------------------------------------
  declareProperty( "reusePatSeeding",		m_reusePatSeeding	= true );
  declareProperty( "InputTracksName",		m_inputTracksName	= LHCb::TrackLocation::Forward  );
  declareProperty( "UsedLHCbIDToolName", m_LHCbIDToolName = "");
  declareProperty( "DriftRadiusRange",		m_driftRadiusRange	= {-0.6 * Gaudi::Units::mm,2.8 * Gaudi::Units::mm});

  //------------------------------------------------------------------------
  // track model
  //------------------------------------------------------------------------
  declareProperty( "dRatio",			m_dRatio		= -1e42);
  declareProperty( "InitialArrow",		m_initialArrow		= -1e42);
  declareProperty( "MomentumScale",		m_momentumScale		= -1e42);
  declareProperty( "zReference",		m_zReference		=  StateParameters::ZMidT);

  //------------------------------------------------------------------------
  // options for track search in xz projection
  //------------------------------------------------------------------------
  // curvature cut
  declareProperty( "MinMomentum",		m_minMomentum		=  500. * Gaudi::Units::MeV );
  declareProperty( "CurveTol",			m_curveTol		=    3. * Gaudi::Units::mm );
  // center of magnet compatibility
  declareProperty( "zMagnet",			m_zMagnet		= -1e42 );
  declareProperty( "xMagTol",			m_xMagTol		= 2000. * Gaudi::Units::mm );
  // window to collect hits from
  declareProperty( "TolCollectOT",		m_tolCollectOT		=    3. * Gaudi::Units::mm );
  declareProperty( "TolCollectIT",		m_tolCollectIT		=    .3 * Gaudi::Units::mm );
  declareProperty( "TolCollectITOT",		m_tolCollectITOT	=    .6 * Gaudi::Units::mm );
  // clone killing in xz projection
  declareProperty( "MinXPlanes",		m_minXPlanes		=    5                     );
  declareProperty( "CommonXFraction",		m_commonXFraction	=    0.7                   );
  declareProperty( "QualityWeights",		m_qualityWeights	= { 1.0,-0.2});
  // demands that NDblOTHitsInXSearch out of three layers used for the initial
  // parabola have a hits in both monolayers
  declareProperty( "NDblOTHitsInXSearch",	m_nDblOTHitsInXSearch	= 0 );

  //------------------------------------------------------------------------
  // options for track search in stereo layers
  //------------------------------------------------------------------------
  // projection plane for stereo hits
  declareProperty( "zForYMatch",		m_zForYMatch		= 9000. * Gaudi::Units::mm );
  // spread of run of stereo hits in projection plane
  declareProperty( "MaxRangeOT",		m_maxRangeOT		=  150. * Gaudi::Units::mm );
  declareProperty( "MaxRangeIT",		m_maxRangeIT		=   15. * Gaudi::Units::mm );
  // pointing criterium in y
  declareProperty( "yCorrection",		m_yCorrection		= -1e42);
  declareProperty( "MaxYAtOrigin",		m_maxYAtOrigin		=  400. * Gaudi::Units::mm );
  declareProperty( "MaxYAtOriginLowQual",	m_maxYAtOriginLowQual	= 1500. * Gaudi::Units::mm );
  // tolerance to check if a hit is within the sensitive y area of straw/sensor
  declareProperty( "yTolSensArea",		m_yTolSensArea		=   40. * Gaudi::Units::mm );
  // y hit requirements during per-region stereo search
  declareProperty( "MinStHitsPerStaOT",		m_minStHitsPerStaOT	= 1			   );
  declareProperty( "MinTotStHitsOT",		m_minTotStHitsOT	= 6			   );

  //------------------------------------------------------------------------
  // track search starting with quartett in IT
  //------------------------------------------------------------------------
  declareProperty( "TolExtrapolate",		m_tolExtrapolate	=    4. * Gaudi::Units::mm );

  //------------------------------------------------------------------------
  // internal fit, outlier removal, minimum number of planes
  //------------------------------------------------------------------------
  declareProperty( "MaxChi2HitOT",		m_maxChi2HitOT		=   30.                    );
  declareProperty( "MaxChi2HitIT",		m_maxChi2HitIT		=   10.                    );
  declareProperty( "MaxTrackChi2",		m_maxTrackChi2     	=   15.                    );
  declareProperty( "MaxFinalChi2",		m_maxFinalChi2     	=   12.25                  );
  declareProperty( "MaxFinalTrackChi2",		m_maxFinalTrackChi2	=    9.                    );
  declareProperty( "MaxTrackChi2LowMult",	m_maxTrackChi2LowMult	=    6.			   );
  declareProperty( "MinTotalPlanes",		m_minTotalPlanes	=    9			   );
  declareProperty( "MaxMisses",			m_maxMisses		=    1			   );
  declareProperty( "OTNHitsLowThresh",		m_otNHitsLowThresh	=   17			   );

  declareProperty( "MinPlanesPerStation",	m_minPlanesPerStation	=    1			   );
  declareProperty( "MaxHoles",			m_maxHoles		=    2			   );
  declareProperty( "ITStubLooseChi2",		m_itStubLooseChi2	= 1500.			   );
  declareProperty( "ITStubTightChi2",		m_itStubTightChi2	=   80.			   );

  //------------------------------------------------------------------------
  // final track selection
  //------------------------------------------------------------------------
  // maximum fraction of used hits
  declareProperty( "MaxUsedFractPerRegion",	m_maxUsedFractPerRegion	=  0.30			   );
  declareProperty( "MaxUsedFractITOT",		m_maxUsedFractITOT	=  0.15			   );
  declareProperty( "MaxUsedFractLowQual",	m_maxUsedFractLowQual	=  0.05   		   );

  //------------------------------------------------------------------------
  // reference states for subsequent refitting using Kalman filter
  //------------------------------------------------------------------------
  declareProperty( "StateErrorX2",		m_stateErrorX2		=   4. );
  declareProperty( "StateErrorY2",		m_stateErrorY2		= 400. );
  declareProperty( "StateErrorTX2",		m_stateErrorTX2		= 6e-5 );
  declareProperty( "StateErrorTY2",		m_stateErrorTY2		= 1e-4 );
  declareProperty( "MomentumToolName",	        m_momentumToolName	= "FastMomentumEstimate" );
  declareProperty( "ZOutput",			m_zOutputs		= {StateParameters::ZBegT,StateParameters::ZMidT,StateParameters::ZEndT});
  
  //------------------------------------------------------------------------
  // options concerning copying the T station part of forward tracks
  //------------------------------------------------------------------------
  // should we copy T station part of forward tracks (default: no)
  declareProperty( "UseForwardTracks",		m_useForwardTracks	= false       );
  declareProperty( "ForwardMaxChi2",		m_forwardMaxChi2	= std::numeric_limits<double>::max() );
  // maximal difference in track parameters for two tracks to be considered clones
  declareProperty( "ForwardCloneMaxXDist",	m_forwardCloneMaxXDist	=   10.0 * Gaudi::Units::mm );
  declareProperty( "ForwardCloneMaxYDist",	m_forwardCloneMaxYDist	=   50.0 * Gaudi::Units::mm );
  declareProperty( "ForwardCloneMaxTXDist",	m_forwardCloneMaxTXDist	=    0.005 );
  // if clones defined in the sense above share more than a fraction of
  // ForwardCloneMaxSharedof hits, only the longer track survives
  declareProperty( "ForwardCloneMaxShared",	m_forwardCloneMaxShared = 0.3);
  // if clones defined in the sense above share less than above fraction
  // of hits, this property determines if they will be merged into a single
  // T station track (as PatForward is tuned for efficiency, T station track
  // segments contain about 5% clones which are due to a T station track
  // being "split" to match several Velo tracks)
  declareProperty( "ForwardCloneMergeSeg",	m_forwardCloneMergeSeg	= true );

  //------------------------------------------------------------------------
  // anything that doesn't fit above
  //------------------------------------------------------------------------
  // per-stage timing measurements
  declareProperty( "MeasureTime",		m_measureTime		= false       );
  // special cuts for magnetic field off
  declareProperty( "FieldOff",			m_fieldOff		= false	      );
  // special isolation cuts for low ghost fraction (efficiency drops!)
  declareProperty( "EnforceIsolation",		m_enforceIsolation	= false );
  declareProperty( "ITIsolation",		m_ITIsolation		=   15. * Gaudi::Units::mm );
  declareProperty( "OTIsolation",		m_OTIsolation		=   20. * Gaudi::Units::mm );
  declareProperty( "Cosmics",			m_cosmics		= false );
  // maximal occupancy
  declareProperty( "MaxITHits",			   m_maxITHits        = 3000 );
  declareProperty( "MaxOTHits",			   m_maxOTHits        = 10000 );
  declareProperty( "AbortOnVeloAbort", m_abortOnVeloAbort = true );

  declareProperty( "ActivateHltPropertyHack", m_activateHLTPropertyHack = true ); 
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode PatSeedingTool::initialize()
{
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiTool

  m_magFieldSvc = svc<ILHCbMagnetSvc>("MagneticFieldSvc", true);
  m_tHitManager = tool<Tf::TStationHitManager<PatForwardHit> >("PatTStationHitManager");
  m_seedTool = tool<PatSeedTool>("PatSeedTool");
  m_momentumTool = tool<ITrackMomentumEstimate>(m_momentumToolName);
  m_usedLHCbIDTool = !m_LHCbIDToolName.empty() ? tool<IUsedLHCbID>(m_LHCbIDToolName, this) 
                                               : nullptr ;
  m_online = false;
  if (context() == "HLT" || context() == "Hlt") {
    if (msgLevel(MSG::DEBUG)) {
      debug() << "Running in HLT context: Online = true" << endmsg;
    }
    m_online = true;
  } else if (context() != "" && context() != "Offline") {
    Warning( "Unexpected context '" + context() +
	"'. Assuming offline mode, please check !" ).ignore();
  }

  const bool activateHLTPropertyHack = m_online && m_activateHLTPropertyHack;
  if (activateHLTPropertyHack) {
    char buf[8192];
    debug() << "HLT property hack about to be activated" << endmsg <<
      "dumping values of potentially affected properties before hack:" << endmsg;
    std::snprintf(buf, 8191, "\nInitialArrow % 24.17e\n"
	"MomentumScale % 24.17e\n"
	"dRatio % 24.17e\n"
	"zMagnet % 24.17e\n"
	"yCorrection % 24.17e\n",
	m_initialArrow, m_momentumScale, m_dRatio, m_zMagnet, m_yCorrection);
    buf[8191] = 0;
    debug() << buf << endmsg << "applying HLT property hack" << endmsg;
  }
  if (m_magFieldSvc->useRealMap()) {
    if (activateHLTPropertyHack || -1e42 == m_initialArrow)
      m_initialArrow  = 4.25307e-09;
    if (activateHLTPropertyHack || -1e42 == m_momentumScale)
      m_momentumScale = 35.31328;
    if (activateHLTPropertyHack || -1e42 == m_dRatio)
      m_dRatio = -3.2265e-4;
    if (activateHLTPropertyHack || -1e42 == m_zMagnet)
      m_zMagnet = 5383.17 * Gaudi::Units::mm;
    if (activateHLTPropertyHack || -1e42 == m_yCorrection)
      m_yCorrection = 4.73385e-15;
  } else {
    if (activateHLTPropertyHack || -1e42 == m_initialArrow)
      m_initialArrow = 4.21826e-09;
    if (activateHLTPropertyHack || -1e42 == m_momentumScale)
      m_momentumScale = 40.3751;
    if (activateHLTPropertyHack || -1e42 == m_dRatio)
      m_dRatio = -3.81831e-4;
    if (activateHLTPropertyHack || -1e42 == m_zMagnet)
      m_zMagnet = 5372.1 * Gaudi::Units::mm;
    if (activateHLTPropertyHack || -1e42 == m_yCorrection)
      m_yCorrection = 8.6746e-15;
  }
  if (activateHLTPropertyHack) {
    char buf[8192];
    debug() << "HLT property hack has been activated" << endmsg <<
      "dumping values of potentially affected properties after hack:" << endmsg;
    std::snprintf(buf, 8191, "\nInitialArrow % 24.17e\n"
	"MomentumScale % 24.17e\n"
	"dRatio % 24.17e\n"
	"zMagnet % 24.17e\n"
	"yCorrection % 24.17e\n",
	m_initialArrow, m_momentumScale, m_dRatio, m_zMagnet, m_yCorrection);
    buf[8191] = 0;
    debug() << buf << endmsg << "end of HLT property hack handling" << endmsg;
  }


  //== Max impact: first term is due to arrow ->curvature by / (mm)^2 then momentum to impact at z=0
  //== second term is decay of Ks: 210 MeV Pt, 2000 mm decay distance.
  // protect against division by near-zero
  if (1e-42 > std::abs(m_minMomentum) || 1e-42 > std::abs(m_momentumScale) ||
      1e-42 > std::abs(m_initialArrow)) m_maxImpact = HUGE_VAL;
  else m_maxImpact =
    1. / ( m_momentumScale * m_initialArrow * m_minMomentum ) +
    2000. * 210. / m_minMomentum;

  if ( m_measureTime ) {
    // apparently, the global version of the tool does not print statistics
    // for our tool, so we get a private copy instead
    m_timer = tool<ISequencerTimerTool>( "SequencerTimerTool", this );
    // add a dummy timer entry
    m_timer->addTimer(name());
    m_timer->increaseIndent();
    m_timeInit      = m_timer->addTimer( "prepare hits" );
    m_timeReuseTracks=m_timer->addTimer( "reuse tracks" );
    m_timePerRegion = m_timer->addTimer( "find all regions" );
    m_timeX         = m_timer->addTimer( "find X projections" );
    m_timer->increaseIndent();
    m_timeXIT	    = m_timer->addTimer( "IT X projections" );
    m_timeXOT	    = m_timer->addTimer( "OT X projections" );
    m_timer->decreaseIndent();
    m_timeStereo    = m_timer->addTimer( "find stereo" );
    m_timeItOt      = m_timer->addTimer( "find IT+OT" );
    m_timeLowQual   = m_timer->addTimer( "find low quality tracks" );
    m_timer->decreaseIndent();
  }

  if (m_qualityWeights.size() < 2)
    return Error ( "Not enough qualityWeights values" );
  if (m_driftRadiusRange.size() < 2)
    return Error ( "Not enough DriftRadiusRange values" );
  if (m_driftRadiusRange[0] > m_driftRadiusRange[1]) {
    warning() << "swapping values in DriftRadiusRange such that "
      "lower limit <= upper limit" << endmsg;
    std::swap(m_driftRadiusRange[0], m_driftRadiusRange[1]);
  }

  if (m_nDblOTHitsInXSearch > 3)
    return Error("nDoubleHitsOT must be at most 3!");

  return StatusCode::SUCCESS;
}

StatusCode PatSeedingTool::finalize()
{
  // if we don't implement finalize, our local timers (m_measureTime == true)
  // will get printed without providing a clear picture of what algorithm the
  // printout belongs to (without finalize() implemented here, printout is
  // triggered in some base class at a time when the algorithm name is no
  // longer available)
  if (m_measureTime) {
    info() << name() << " finalizing now..." << endmsg;
  }
  return GaudiTool::finalize();
}

unsigned PatSeedingTool::prepareHits()
{
  m_printing = msgLevel( MSG::DEBUG );

  m_tHitManager->prepareHits();

  if ( m_measureTime ) m_timer->start( m_timeInit );

  // mark hits used if:
  // - used by a different algorithm (and user asked us to ignore them)
  // - drift radius out of range
  const PatRange driftRadRange(m_driftRadiusRange[0], m_driftRadiusRange[1]);
  HitRange range = hits();
  for( PatFwdHit* hit: range ) {
    hit->setSelected( true );
    hit->setIgnored( false );
    //make sure we set all hits to false
    hit->setIsUsed(false);
    if (hit->hit()->ignore()){
      hit->setIsUsed( true );
      continue;
    }
    if ( m_usedLHCbIDTool ) {
      //make sure we set all hits to false
      if (m_usedLHCbIDTool->used(hit->hit()->lhcbID())) {
        hit->setIsUsed(true);
        continue;
      }
    }
    if ( !m_reusePatSeeding ) {
      if ( hit->hit()->testStatus(Tf::HitBase::UsedByPatSeeding) ) {
        hit->setIsUsed(true);
        continue;
      }
    }
    const Tf::OTHit* otHit = hit->hit()->othit();
    if ( otHit ) {
      const double driftRad = otHit->untruncatedDriftDistance();
      if ( !driftRadRange.isInside(driftRad) ) {
        hit->setIsUsed( true );
        continue;
      }
    }
  }

  // update region cache: we need some information not directly available
  // from the Tf::-framework
  for (unsigned sta = 0; sta < m_nSta; ++sta) {
    for (unsigned lay = 0; lay < m_nLay; ++lay) {
      for (unsigned reg = 0; reg < m_nReg; ++reg) {
	unsigned idx = (sta * m_nLay + lay) * m_nReg + reg;
	m_RCtanT[idx] = region(sta,lay,reg)->sinT() /
	  region(sta,lay,reg)->cosT();
	const Gaudi::XYZVector& vy(region(sta, lay, reg)->vy());
	m_RCdzdy[idx] = vy.z() / vy.y();
	const Gaudi::XYZVector& vx(region(sta, lay, reg)->vx());
	m_RCdzdx[idx] = vx.z() / vx.x();
	// return zmid at x = 0, y = 0 (instead of x = xmid, y = ymid)
	m_RCz0[idx] = region(sta,lay,reg)->z() -
	  region(sta,lay,reg)->y() * m_RCdzdy[idx] -
	  region(sta,lay,reg)->x() * m_RCdzdx[idx];
      }
    }
  }

  // if someone wants a printout, do so now
  if ( msgLevel( MSG::VERBOSE ) ) {
    for (unsigned reg = 0; reg < m_nReg; ++reg) {
      for (unsigned sta = 0; sta < m_nSta; ++sta) {
        for (unsigned lay = 0; lay < m_nLay; ++lay) {
          HitRange range1 = hits(sta, lay, reg);
          for( const PatFwdHit* hit: range1 ) {
            if ( ! hit->isUsed() ) continue;
            debugFwdHit( hit, verbose() );
          }
        }
      }
    }
  }

  if ( m_measureTime ) m_timer->stop( m_timeInit );

  return hits().size();
}

void PatSeedingTool::killClonesAndStore(
                                        std::vector<PatSeedTrack*>& finalSelection,
                                        std::vector<LHCb::Track*>& outputTracks,
                                        double maxUsedFraction) const
{
  if (finalSelection.empty()) return;
  // make sure there's space in the output
  outputTracks.reserve(outputTracks.size() + finalSelection.size());
  //== Sort tracks according to their chi2
  if (finalSelection.size() > 1)
    std::stable_sort( finalSelection.begin(), finalSelection.end(),
               PatSeedTrack::decreasingQuality() );

  bool debug = msgLevel( MSG::DEBUG );
  //== Keep only those with less than maxUsedFraction used clusters
  for (const PatSeedTrack* track: finalSelection) {
    // this is a funny way to implement the cut on the fraction of used
    // hits, but the advantage is that we stop counting early, if the
    // track does not pass the cut
    int maxUsed = int(std::ceil(maxUsedFraction * track->nCoords()));
    unsigned nOT = 0, nIT = 0;
    for( const PatFwdHit* hit: track->coords() ) {
      if (hit->isOT()) ++nOT;
      else ++nIT;
      if (hit->isUsed() && (0 > --maxUsed)) break;
    }
    if ( 0 > maxUsed ) continue;

    if (nOT && (double(std::abs(track->otMonoAsym()))/double(nOT) > 0.25 
	  && std::abs(track->otMonoAsym()) > 4)) continue;
    if (!nIT && double(track->nbOnSide()) / double(2 * track->nPlanes()) > 1.4) continue;
    if (track->minPlanesPerStation() < m_minPlanesPerStation) continue;
    if (track->nHoles() > m_maxHoles) continue;

    if ( debug ) {
      const double scaleFactor = m_magFieldSvc->signedRelativeCurrent();
      const double denom = m_momentumScale * track->curvature();
      double p;
      // avoid dividing by zero
      if (std::abs(denom) < 1e-42)
        p = ((track->curvature() < 0.) ? -HUGE_VAL : HUGE_VAL);
      else
        p = scaleFactor *(-1) / denom;
      info() << "** Store track, estimated P " << p << " nCoord "
             << track->nCoords() << " chi2 " << track->chi2() << endmsg;
      for( const PatFwdHit* hit: track->coords() )
        debugFwdHit( hit, info() );
    }
    storeTrack( *track, outputTracks );
  }
}

StatusCode PatSeedingTool::performTracking(
                                           std::vector<LHCb::Track*>& outputTracks,
                                           const LHCb::State* state )
{
  // Create a ProcStatus if it does not already exist
  LHCb::ProcStatus* procStat =
    getOrCreate<LHCb::ProcStatus,LHCb::ProcStatus>(LHCb::ProcStatusLocation::Default);

  // Do not process if Velo has aborted
  if( m_abortOnVeloAbort && procStat->subSystemAbort("VELO") ) {
    Warning("Skipping event aborted by Velo",StatusCode::SUCCESS,0).ignore();
    return StatusCode::SUCCESS;
  }

  // protect against very hot events
  if( m_maxITHits < 999999 || m_maxOTHits < 999999 ) {
    unsigned nHitsIT = 0, nHitsOT = 0;
    for (unsigned sta = 0; sta < m_nSta; ++sta) {
      for (unsigned lay = 0; lay < m_nLay; ++lay) {
        for (unsigned reg = 0; reg < m_nReg; ++reg) {
          // keep track of number of hits
          if (isRegionOT(reg)) nHitsOT += hits(sta, lay, reg).size();
          else nHitsIT += hits(sta, lay, reg).size();
        }
      }
    }
    if (nHitsIT > m_maxITHits || nHitsOT > m_maxOTHits) {
      // in L0 confirmation context, we just return to save time
      if (0 == state) return StatusCode::SUCCESS;

      Warning("Skipping very hot event!",StatusCode::SUCCESS,1).ignore();
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
        debug() << "Skipping very hot event! (" << nHitsIT << " IT hits "
                << nHitsOT << " OT hits)" << endmsg;
      // give some indication that we had to skip this event
      // (ProcStatus returns zero status for us in cases where we don't
      // explicitly add a status code)
      procStat->addAlgorithmStatus( name(), "Tracking", "TooManyHits",
                                    ETooManyHits, true );

      return StatusCode::SUCCESS;
    }
  }

  try {
    // if we are to extract the seed part from Forward tracks, do so now
    if (m_useForwardTracks)
      processForwardTracks(m_inputTracksName, outputTracks);

    std::vector<PatSeedTrack>::iterator itS;
    // the idea is to have two memory pools which grow when needed:
    //
    // pool holds all candidates in a region
    //
    // lowQualCandidates holds all candidates which are not good enough
    // to be considered in the first pass, but might be good enough to
    // make it to the output after the "easy" tracks have been found
    //
    // finalSelection is an array of pointers to tracks which is used for
    // sorting of the final selection of tracks in a short amount of time
    //
    // we create the vectors outside the loop to avoid frequent re-growing
    std::vector<PatSeedTrack> pool;
    std::vector<PatSeedTrack> lowQualTracks;
    std::vector<PatSeedTrack*> finalSelection;
    if (0 == state) pool.reserve(1024);

    // collect all tracks inside a single region
    collectPerRegion(pool, lowQualTracks, finalSelection,
	outputTracks, state);

    //== Find remaining points in IT
    // skip in trigger context if state indicates a track with std::abs(y)>70cm at T1
    if ( 0 == state || 0.7e3 >
	std::abs(state->y()+state->ty()*(StateParameters::ZBegT-state->z()))) {
      // first pass to pick up tracks going from IT to OT
      collectITOT(pool, finalSelection, outputTracks, state);
      // second pass to pick up hits going through several IT/OT regions
      collectITOT(pool, finalSelection, outputTracks, state, true);
    }

    // collect OT tracks for high std::abs(y) which are almost straight
    // skip in trigger context, unless state indicates std::abs(y) > 1m at T1
    if ( 0 == state || 1e3 <
	std::abs(state->y()+state->ty()*(StateParameters::ZBegT-state->z()))) {
      double initialArrow = 0.0, maxUsedFractPerRegion = 0.05;
      std::swap(m_initialArrow, initialArrow);
      std::swap(m_maxUsedFractPerRegion, maxUsedFractPerRegion);
      collectPerRegion(pool, lowQualTracks, finalSelection,
	  outputTracks, state, true);
      std::swap(m_initialArrow, initialArrow);
      std::swap(m_maxUsedFractPerRegion, maxUsedFractPerRegion);
    }

    // re-check the low quality tracks we saved earlier
    // skip for trigger context, as these are mostly low momentum tracks
    if (0 == state)
      collectLowQualTracks(lowQualTracks, finalSelection, outputTracks, state);
  } catch (const std::length_error& e) {
      Warning("Skipping very hot event (FwdHits too small)!",StatusCode::SUCCESS,1).ignore();
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
	debug() << "Skipping very hot event because FwdHits size is too small"
	  << endmsg;
      // give some indication that we had to skip this event
      // (ProcStatus returns zero status for us in cases where we don't
      // explicitly add a status code)
      procStat->addAlgorithmStatus( name(), "Tracking", "TooManyHits",
                                    ETooManyHits, true );
  }

  return StatusCode::SUCCESS;
}

void PatSeedingTool::collectPerRegion(
                                      std::vector<PatSeedTrack>& pool,
                                      std::vector<PatSeedTrack>& lowQualTracks,
                                      std::vector<PatSeedTrack*>& finalSelection,
                                      std::vector<LHCb::Track*>& outputTracks,
                                      const LHCb::State* state,
                                      bool OTonly)
{
  std::vector<PatSeedTrack>::iterator itS;
  FwdHits stereo;

  if (m_measureTime) m_timer->start(m_timePerRegion);
  for (unsigned reg = 0; reg < m_nReg; ++reg) {
    if (OTonly && !isRegionOT(reg)) continue;
    if (0 != state) {
      // in trigger context, make sure we only run over regions in which the
      // track can have enough hits according to the state given
      int nPlanesMax = m_nSta * m_nLay;
      for (unsigned sta = 0; m_nSta > sta; ++sta) {
	for (unsigned lay = 0; m_nLay > lay; ++lay) {
	  double dz = regionZ0(sta, lay, reg) - state->z();
	  dz += regionDzDx(sta,lay,reg) * (state->x() + state->tx() * dz) +
	    regionDzDy(sta,lay,reg) * (state->y() + state->ty() * dz);
	  const double ytol = std::sqrt(propagateEY2(dz,state->covariance()));
	  const double y = propagateY(dz, state->stateVector());
	  if (!regionY(sta,lay,reg, ytol).isInside(y))
	    if (--nPlanesMax < m_minTotalPlanes) break;
	  const double xtol = std::sqrt(propagateEX2(dz,state->covariance()));
	  const double x = propagateX(dz, state->stateVector());
	  if (!regionX(sta,lay,reg, xtol).isInside(x))
	    if (--nPlanesMax < m_minTotalPlanes) break;
	}
	if (nPlanesMax < m_minTotalPlanes) break;
      }
      if (nPlanesMax < m_minTotalPlanes) continue;
    }

    m_tolCollect = isRegionOT(reg) ? m_tolCollectOT : m_tolCollectIT;
    m_maxChi2Hit = isRegionOT(reg) ? m_maxChi2HitOT : m_maxChi2HitIT;
    // loop over X layer combinations in 3 stations (8 of them: 2 X layers in
    // three stations - findXCandidates decodes the bit mask)
    // in the IT, we need to build our initial parabola from three hits
    // from three stations which are in any of the two x layers because
    // the IT has less redundancy than the OT with its two monolayers
    // per OT module
    // for the OT, it is sufficient to take three hits from the first three x
    // layers and the last three x layers of each station
    for ( unsigned lay = 0, nlay = 0; 8 > nlay; ++nlay, lay = (lay - 1) & 7) {
      // in the OT, we only do two combinations (unless we reconstruct cosmics)
      if (isRegionOT(reg) && 2 <= nlay && !m_cosmics) break;
      // if we reconstruct cosmics, we need to extend the x search into
      // more than one region (cosmics almost certainly migrate from one
      // region to another)
      // reginc encodes a per station region increment for stations 1 and 2
      // (we count starting from m_nReg*m_nReg downwards and encode it on the
      // fly)
      // for station 0, we stick to region reg
      for ( unsigned reginc = m_cosmics?(m_nReg*m_nReg):1; reginc--; ) {
        pool.clear();

        if ( m_measureTime ) {
	  m_timer->start( m_timeX );
	  if (isRegionOT(reg)) m_timer->start(m_timeXOT);
	  else if (isRegionIT(reg)) m_timer->start(m_timeXIT);
	}

        const unsigned reginc1 = (m_nReg - 1u - (reginc / m_nReg));
        const unsigned reginc2 = (m_nReg - 1u - (reginc % m_nReg));
        findXCandidates(lay, reg | (reginc1 << 3u) | (reginc2 << 6u),
                        pool, state);

        if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
          debug() << "Found " << pool.size()
                  << " x candidates" << endmsg;

        if ( m_measureTime ) {
	  if (isRegionOT(reg)) m_timer->stop(m_timeXOT);
	  else if (isRegionIT(reg)) m_timer->stop(m_timeXIT);
          m_timer->stop( m_timeX );
          if ( pool.empty() ) continue;
          m_timer->start( m_timeStereo );
        } else {
	  if ( pool.empty() ) continue;
	}

        lowQualTracks.reserve(lowQualTracks.size() + pool.size());
        finalSelection.clear();
        finalSelection.reserve(pool.size());
        for( PatSeedTrack& track: pool ) {
          // skip tracks for which x part is already invalid
          if ( !track.valid() ) continue;
          // unless y turns out to be ok, default to not valid
          track.setValid( false );

          m_printing = msgLevel( MSG::DEBUG );

          if ( m_printing ) {
            // if we're debugging, we can be slow in determining where
            // othertrack is in pool
            int nTrack = 0;
            for( const PatSeedTrack& tmp: pool ) {
              if (&tmp == &track) break;
              ++nTrack;
            }
            info() << "--- X candidate " << nTrack << endmsg;
            for( const PatFwdHit* hit: track.coords() ) {
              info() << format( "dist %7.3f ", track.distance( hit ) );
              debugFwdHit( hit, info() );
            }
          }

          //== Collect stereo hits
          collectStereoHits( track, reg, stereo, state );

          int minNbPlanes = m_minTotalPlanes - track.nPlanes();

          if ( m_printing ) {
            info() << "--- Initial list of stereo hits " << endmsg;
            for( PatFwdHit* hit: stereo ) {
              if ( hit->isIgnored() ) continue;
              restoreCoordinate(hit);
              debugFwdHit( hit, info() );
            }
          }
          if ( stereo.size() < unsigned(minNbPlanes) ) continue;

          bool ok = (m_cosmics ?
                     findBestRangeCosmics( reg, minNbPlanes, stereo ) :
                     findBestRange( reg, minNbPlanes, stereo ) );

	  if ( !ok ) continue;
	  
          if ( m_printing ) {
            info() << "--- After filtering of stereo hits " << endmsg;
            for( PatFwdHit* hit: stereo ) {
              if ( hit->isIgnored() ) continue;
              restoreCoordinate(hit);
              debugFwdHit( hit, info() );
            }
          }

          //== restore real coordinate
          double y0, sl;
          ok = fitLineInY( stereo, y0, sl );

          if ( !ok ) {
            if ( m_printing ) info() << "  -- fitLineInY failed" << endmsg;
            continue;
          }
          if ( 0 != state && !isGoodSlope(state, false, sl) ) {
            if ( m_printing ) info() << "  -- ty not compatible with state given" << endmsg;
            continue;
          }
          double yAtOrigin = y0;
          double yCorr     = sl * sl * track.curvature() * track.curvature() / m_yCorrection;
          yAtOrigin = y0 - std::copysign(yCorr, sl);

          if ( m_printing ) {
            info() << format( "  Y0 %10.1f corr %10.1f yAtOrigin%10.1f  sl%10.7f  curv*E6 %8.4f",
                              y0, yCorr, yAtOrigin, sl, 1.e6*track.curvature() ) << endmsg;
          }

          if ( m_maxYAtOrigin < std::abs( yAtOrigin ) ) {
            if ( m_printing ) info() << "  -- yAtOrigin too big : " << yAtOrigin << endmsg;
            // we might want to keep OT tracks for a second pass
            if (!isRegionOT(reg)) continue;
            // unless they point nowhere near the vertex
            if (m_maxYAtOriginLowQual < std::abs(yAtOrigin)) continue;
	    try {
	      track.addCoord(std::begin(stereo), std::end(stereo),
		  [] (const PatFwdHit* hit) { return !hit->isIgnored(); });
	    } catch (const std::length_error& e) {
	      if ( m_printing ) info() << "  -- trying to add too many hits" << endmsg;
	      continue;
	    }
            track.setYParams( y0, sl );
            lowQualTracks.push_back(track);
            continue;  // non pointing track -> garbage
          }

          if ( m_printing ) {
            info() << "Fitted line y0 " << y0 << " sl " << sl << endmsg;
            for( const PatFwdHit* hit: stereo ) {
              if ( hit->isIgnored() ) continue;
              info() << format( "dist %7.3f ",  hit->projection() - y0 - hit->z() * sl );
              debugFwdHit( hit, info() );
            }
          }
          //== Check for enough OT if not near boundary
          if ( isRegionOT(reg) ) {
            double y = y0 + m_zForYMatch * sl;
            if ( m_centralYOT < std::abs(y) ) {
              std::array<int, 3> nInStation = { {0, 0, 0} };
              for( const PatFwdHit* hit: stereo)
                if ( !hit->isIgnored() )
                  nInStation[ hit->hit()->station() ]++;
              if (m_minStHitsPerStaOT >
		  std::min(nInStation[0], std::min(nInStation[1], nInStation[2])) ||
		  m_minTotStHitsOT > nInStation[0] + nInStation[1] + nInStation[2]) {
                if ( m_printing )
                  info() << "Not enough OT stereo in some station : "
                         << nInStation[0] << " "
                         << nInStation[1] << " "
                         << nInStation[2] << endmsg;
                continue;
              }
            }
          }

	  try {
	    track.addCoord(std::begin(stereo), std::end(stereo),
		[] (const PatFwdHit* hit) { return !hit->isIgnored(); });
	  } catch (const std::length_error& e) {
	    if ( m_printing ) info() << "  -- trying to add too many hits" << endmsg;
	    continue;
	  }

          if (m_cosmics) {
            // for cosmics, we were more permissive when collecting hits
            // as we may have picked up some way off in y, discard them now
            // (track parameters in y are known by now)
            for (auto it = track.coordBegin();
                 track.coordEnd() != it; ++it) {
              const PatFwdHit* hit = *it;
              const PatRange yRange(hit->hit()->yMin() - m_maxRangeOT,
                                    hit->hit()->yMax() + m_maxRangeOT);
              const double yHit = y0 + sl * hit->z();
              if (!yRange.isInside(yHit)) it = track.removeCoord(it);
            }
          }

	  if ( isRegionOT(reg) && m_otNHitsLowThresh > track.nCoords() &&
	      2 * (track.nCoords()-track.nPlanes()+1) < track.nbOnSide() ) {
	    if ( m_printing ) info()  << "Too many on side " << track.nbOnSide() << " with only "
	      << track.nCoords() << " coordinates" << endmsg;
	    continue;
	  }

          track.setYParams( y0, sl );

          track.sort();
          if ( m_printing ) info() << "-- Before fit, nPlanes = " << track.nPlanes() << endmsg;
	  if (isRegionOT(reg)) {
	    ok = m_seedTool->fitTrack<PatSeedTool::TrackType::OTOnly>( track, m_maxFinalChi2, m_minTotalPlanes, false, m_printing );
	  } else {
	    ok = m_seedTool->fitTrack<PatSeedTool::TrackType::ITOnly>( track, m_maxFinalChi2, m_minTotalPlanes, false, m_printing );
	  }

	  if ( m_printing ) info() << "After fit status "<< ok << " chi2 " << track.chi2()
	    << ", nPlanes = " << track.nPlanes() << endmsg;
	  if ( isRegionOT(reg) && m_otNHitsLowThresh > track.nCoords() &&  // short track -> not too many neighbours
	      2 * (track.nCoords() - track.nPlanes() +1 ) < track.nbOnSide() ) {
	    if ( m_printing ) {
	      info()  << "Too many on side " << track.nbOnSide() << " with only "
		<< track.nCoords() << " coordinates" << endmsg;
	      for( const PatFwdHit* hit: track.coords() )
		debugFwdHit( hit, info() );
	    }
	    continue;
	  }

          if ( m_maxFinalTrackChi2 < track.chi2() || !ok ) {
            if ( m_printing ) info() << "Chi2 too big" << endmsg;
            continue;
          }

          if ( 0 != state && !isGoodSlope(state, false, track.ySlope(0.)) ) {
            if ( m_printing ) info() << "  -- ty not compatible with state given" << endmsg;
            continue;
          }

          // if a track makes it to this point, it's valid
          track.setValid( true );
          finalSelection.push_back(&track);
        } // end loop over pool
        if ( m_measureTime ) m_timer->stop( m_timeStereo );

        //== Keep only best with >50% unused clusters (in the current pass...)
        killClonesAndStore(finalSelection, outputTracks, m_maxUsedFractPerRegion);
      } // reginc
    } // layer
  } // reg
  if (m_measureTime) m_timer->stop(m_timePerRegion);
}

void PatSeedingTool::collectITOT(
                                 std::vector<PatSeedTrack>& pool,
                                 std::vector<PatSeedTrack*>& finalSelection,
                                 std::vector<LHCb::Track*>& outputTracks,
                                 const LHCb::State* state,
                                 bool doOverlaps)
{
  if ( m_measureTime ) m_timer->start( m_timeItOt );
  // table in which to collect hits around a stub
  // first index is station in which we have the stub, second are remaining
  // stations (prefer to start close to the stub and at high z because of
  // lower track multiplicity)
  const unsigned stationorder[3][3] = { { 1, 2, 0 }, { 2, 0, 1 }, { 1, 0, 2 } };

  if ( m_printing ) info() << "=== remaining IT coord ====" << endmsg;

  for ( unsigned reg = m_nOTReg; reg < m_nReg; ++reg ) {
    finalSelection.clear();
    pool.clear();
    m_tolCollect = m_tolCollectITOT;
    // form track segments from unused hits in all IT regions
    for ( unsigned sta = m_nSta; sta--; ) {
      double zScaling = ( regionZ0(sta,3,reg) - regionZ0(sta,0,reg)) /
        ( regionZ0(sta, 0, reg) - m_zMagnet );

      // get hits in current region (restricting selection if we have a state)
      HitRange rangeH0 = (0 == state) ? hits(sta,0,reg) :
        hitsInRange(predictXinRegion(state, sta, 0, reg), sta, 0, reg);

      for ( HitRange::const_iterator itH0 = rangeH0.begin();
            rangeH0.end() != itH0; ++itH0 ) {
        PatFwdHit* hit0 = *itH0;
        if ( hit0->isUsed() ) continue;
        if (m_enforceIsolation && !isIsolated(itH0, rangeH0)) continue;

        m_printing = msgLevel( MSG::VERBOSE );
        if ( m_printing ) info() << "*IT* hit sta " << sta << " lay 0 x = " <<
          hit0->hit()->xAtYEq0() << endmsg;

        // open search window into layer 3
        PatRange x3Range = magnetRange(hit0->hit()->xAtYEq0(), zScaling);
        // restrict search window if we have a state
        if ( 0 != state )
          x3Range.intersect(predictXinRegion(state, sta,3,reg));

        HitRange rangeH3 = hitsInRange( x3Range,sta,3,reg );
        for ( HitRange::const_iterator itH3 = rangeH3.begin();
              rangeH3.end() != itH3; ++itH3 ) {
          PatFwdHit* hit3 = *itH3;
          if ( hit3->isUsed() ) continue;
          if ( m_enforceIsolation && !isIsolated(itH3, rangeH3) ) continue;
          double z0 = hit0->hit()->zAtYEq0();
          double x0 = hit0->hit()->xAtYEq0();
          double sl = (hit3->hit()->xAtYEq0() - x0) /
            (hit3->hit()->zAtYEq0() - z0);
          // check against slope from state if available
          if (state && !isGoodSlope(state, true, sl))
            continue;

          // cache tangens of stereo angle
          const double tanT1 = regionTanT(sta,1,reg);
          const double tanT2 = regionTanT(sta,2,reg);

	  // open search window into first stereo layer
	  PatRange x1Range = regionY(sta,1,reg) *= tanT1;
	  x1Range += x0 + ( regionZ0(sta,1,reg) + regionDzDx(sta,1,reg) *
	      (x0 + sl * (regionZ0(sta,1,reg) - z0)) +
	      regionDzDy(sta,1,reg) * region(sta,1,reg)->y() - z0 ) * sl;
	  x1Range.widen(1.5 * m_tolCollect);
	  // restrict search window if we have a state
	  if (0 != state)
	    x1Range.intersect(predictXinRegion(state, sta, 1, reg));

          if ( m_printing ) info() << "     hit sta " << sta << " lay 3 x = "
                                   << hit3->hit()->xAtYEq0() << " x1Min " << x1Range.min() << " x1Max "
                                   << x1Range.max() << endmsg;

          HitRange rangeH1 = hitsInRange( x1Range, sta, 1,reg );
          for ( HitRange::const_iterator itH1 = rangeH1.begin();
                rangeH1.end() != itH1; ++itH1 ) {
            PatFwdHit* hit1 = *itH1;
            if ( hit1->isUsed() ) continue;
            if ( m_enforceIsolation && !isIsolated(itH1, rangeH1) ) continue;

	    // open search window into second stereo layer
	    const double y = ((x0 + (hit1->hit()->zAtYEq0() - z0) * sl) -
		hit1->hit()->xAtYEq0()) / hit1->hit()->dxDy();
	    if (!hit1->hit()->isYCompatible(y, m_yTolSensArea)) continue;
	    // check slope in y if we have a state
	    if (state && !isGoodSlope(state, false, y / hit1->hit()->zAtYEq0()))
	      continue;
	    const double z2 = regionZ0(sta,2,reg) +
	      regionDzDx(sta,2,reg) * (x0 + sl * (regionZ0(sta,2,reg) - z0)) +
	      regionDzDy(sta,2,reg) * (y * regionZ0(sta,2,reg) / hit1->hit()->zAtYEq0());
	    // assume straight line through hit1 and origin to derive y at
	    // second layer
	    const double pred = (x0 + (z2 - z0) * sl) +
	      z2 / hit1->hit()->zAtYEq0() * y * tanT2;
	    PatRange x2Range = symmetricRange(pred, 1.5 * m_tolCollect);
	    // restrict search window if we have a state
	    if (0 != state)
	      x2Range.intersect(predictXinRegion(state, sta, 2, reg));

            if ( m_printing ) info() << "     hit sta " << sta << " lay 1 x = "
                                     << hit1->hit()->xAtYEq0() << " x2Min " << x2Range.min() << " x2Max "
                                     << x2Range.max() << endmsg;

            HitRange rangeH2 = hitsInRange( x2Range, sta, 2, reg );
            for ( HitRange::const_iterator itH2 = rangeH2.begin();
                  rangeH2.end() != itH2; ++itH2 ) {
              PatFwdHit* hit2 = *itH2;
              if ( hit2->isUsed() ) continue;
              if ( m_enforceIsolation && !isIsolated(itH2, rangeH2) ) continue;
	      if (!hit2->hit()->isYCompatible(z2 / hit1->hit()->zAtYEq0() * y,
		    m_yTolSensArea)) continue;

              // restore coordinates
              restoreCoordinate(hit0);
              restoreCoordinate(hit1);
              restoreCoordinate(hit2);
              restoreCoordinate(hit3);

              if ( m_printing )
                info() << "     hit sta " << sta << " lay 2 x = " << hit2->x() << endmsg;

	      // ok, we have a seed
	      pool.push_back(
		  PatSeedTrack( hit0, hit1, hit2, hit3,
		    m_zReference, m_dRatio, m_initialArrow ) );
	      PatSeedTrack& track = pool.back();

              // Add neighbouring hits
              addNeighbour(track, hit0, itH0, rangeH0);
              addNeighbour(track, hit1, itH1, rangeH1);
              addNeighbour(track, hit2, itH2, rangeH2);
              addNeighbour(track, hit3, itH3, rangeH3);

	      if (3 > track.nbHighThreshold()) {
		if ( m_printing ) info() << "   less than 3 with high threshold" << endmsg;
		// unfortunately not good enough, so pop it off again
		pool.pop_back();
		continue;
	      }

	      // try to kill the seed extremely early by doing a very
	      // loose red. chi^2 cut even before we attempt a refit
	      track.updateHits<PatSeedTrack::IT>();
	      double chi2 = 0.;
	      for( const PatFwdHit* hit: track.coords() )
		chi2 += track.chi2Hit(hit);
	      if (chi2 / double(track.nCoords() - 3) > m_itStubLooseChi2) {
		if ( m_printing )
		  info() << "   seed prefit chi^2/ndf too large: " <<
		    chi2 / double(track.nCoords() - 3) << endmsg;
		pool.pop_back();
		continue;
	      }
	      // refit, using fixed curvature and slope ty fixed to y0/z0
	      if (!m_seedTool->refitStub(track, m_initialArrow)) {
		if ( m_printing )
		  info() << "   refitStub: matrix singular or no convergence" << endmsg;
		pool.pop_back();
		continue;
	      }
	      chi2 = 0.;
	      for( const PatFwdHit* hit: track.coords() )
		chi2 += track.chi2Hit(hit);
	      // if the reduced chi^2 is still bad, we also kill the seed
	      if (chi2 / double(track.nCoords() - 3) > m_itStubTightChi2) {
		if ( m_printing )
		  info() << "   seed chi^2/ndf too large: " <<
		    chi2 / double(track.nCoords() - 3) << endmsg;
		pool.pop_back();
		continue;
	      }

	      if ( m_printing ) {
		info() << "--- IT point -- " << endmsg;
		for( const PatFwdHit* hit: track.coords() )
		  debugFwdHit( hit, info() );
	      }
	    }
	  }
	}
      }
    }

    // follow these track segments into OT and IT and collect more hits there
    finalSelection.reserve(pool.size());
    if ( m_printing ) info() << "===== Candidates Type " << reg << endmsg;
    for (auto itT = pool.begin(); pool.end() != itT; ++itT) {
      PatSeedTrack& track = *itT;
      track.updateHits<PatSeedTrack::IT>();
      track.setValid(false);
      bool centralYIT = false;
      bool fromInsideBeampipe = false;
      //      bool hasOT = false;
      bool fitOK = false, hasOT = false;
      m_printing = msgLevel( MSG::DEBUG );

      if ( m_printing ) info() << "   candidate " << itT - pool.begin()
                               << " y(0) = " << track.yAtZ( 0. )
                               << " ySlope " << track.ySlope( 0. )
                               << endmsg;
      PatFwdHit* firstHit = *track.coordBegin();

      unsigned ITSta = firstHit->hit()->station();
      unsigned otTypMin = (track.yAtZ(firstHit->z()) > 0.0) ? 1 : 0;
      unsigned otTypMax = otTypMin + 1;
      bool dirty = false;
      /* near y = 0 or when doing overlaps, try both OT halves */
      if (std::abs(track.yAtZ(m_zForYMatch)) < 45. || doOverlaps)
	otTypMin = 0, otTypMax = m_nOTReg;

      try {
	//== Some data in OT ??
	for (unsigned ista = 0; ista < 3; ++ista ) {
	  const unsigned sta = stationorder[ITSta][ista];
	  // skip station used to produce the seed (we used all hits there)
	  if ( sta == ITSta ) continue;
	  unsigned nbPlanes = track.nPlanes();

	  for ( unsigned lay = 0 ; 4 > lay ; ++lay ) {
	    for (unsigned otTyp = otTypMin; otTypMax > otTyp; ++otTyp) {
	      // skip layers which are too far away in y
	      const double zPred = regionZ0(sta,lay,otTyp) +
		regionDzDy(sta,lay,otTyp) * track.yAtZ(regionZ0(sta,lay,otTyp)) +
		regionDzDx(sta,lay,otTyp) * track.xAtZ(regionZ0(sta,lay,otTyp));
	      const double yPred = track.yAtZ(zPred);
	      const double xPred = track.xAtZ(zPred);
	      if (std::abs(yPred) < 120. && std::abs(xPred) < 120.)
		fromInsideBeampipe = true;
	      if (!regionY(sta,lay,otTyp, 10.0 * m_tolExtrapolate).isInside(yPred))
		continue;
	      if (!regionX(sta,lay,otTyp, 2.0 * m_tolExtrapolate).isInside(xPred))
		continue;

	      // ok, check if we can pick up more hits
	      const double tanT = regionTanT(sta,lay,otTyp);
	      PatRange xRange = symmetricRange(xPred + yPred * tanT,
		  m_tolExtrapolate + 3.);
	      if (0 != state)
		xRange.intersect(predictXinRegion(state,sta,lay,otTyp));
	      if ( m_printing ) {
		info() << "** Search in sta " << sta << " lay " << lay <<
		  " xPred " << (xPred + yPred * tanT) << endmsg;
	      }

	      HitRange rangeXX = hitsInRange( xRange,sta,lay,otTyp );
	      HitRange::iterator it = rangeXX.begin(), end = rangeXX.end();
	      for ( ; end != it; ++it ) {
		PatFwdHit* hit = *it;
		if ( hit->isUsed() ) continue;
		updateHitForTrack( hit, track.yAtZ(hit->hit()->zAtYEq0()),0);
		if ( m_tolExtrapolate < std::abs(track.distance(hit)) ) continue;
		if (!hit->hit()->isYCompatible(yPred, m_yTolSensArea)) continue;
		if ( m_enforceIsolation && !isIsolated(it, rangeXX) ) continue;
		if ( m_printing ) {
		  info() << "Add coord ";
		  debugFwdHit( hit, info() );
		}
		track.addCoord( hit );
		dirty = true;
	      }
	    }
	  }
	  if ( dirty ) {
	    hasOT = true;
	    const double maxchi2hit = hasOT ? m_maxChi2HitOT : m_maxChi2HitIT;
	    const unsigned minPlanes = (track.nPlanes() <= 8) ?
	      8 : m_minTotalPlanes;
	    fitOK = m_seedTool->fitTrack<PatSeedTool::TrackType::Mixed>(track, maxchi2hit, minPlanes, false, m_printing);
	    dirty = (nbPlanes != track.nPlanes());
	    if ( m_printing ) info() << "*** refitted track candidate status " << fitOK << endmsg;
	  }
	}

	// Try in IT again if needed
	if ( (3 > track.nStations()) || doOverlaps ) {
	  unsigned minReg = (doOverlaps) ? (m_nOTReg) : (reg);
	  unsigned maxReg = (doOverlaps) ? (m_nReg) : (reg + 1);
	  for (unsigned ista = 0; ista < 3; ++ista) {
	    const unsigned sta = stationorder[ITSta][ista];
	    for ( unsigned testReg = minReg; maxReg > testReg; ++testReg) {
	      if ( sta == ITSta && reg == testReg) continue;
	      for ( unsigned lay = 0 ; 4 > lay ; ++lay ) {
		const double zPred = regionZ0(sta,lay,testReg) +
		  regionDzDy(sta,lay,testReg) * track.yAtZ(regionZ0(sta,lay,testReg)) +
		  regionDzDx(sta,lay,testReg) * track.xAtZ(regionZ0(sta,lay,testReg));
		const double yPred = track.yAtZ( zPred );
		if ( m_centralYIT > std::abs(yPred) && doOverlaps )
		  centralYIT = true;
		const double xPred = track.xAtZ( zPred );
		if (std::abs(yPred) < 120. && std::abs(xPred) < 120.)
		  fromInsideBeampipe = true;
		if (!regionY(sta, lay, testReg, 10.0 * m_tolExtrapolate).isInside(yPred))
		  continue;
		if (!regionX(sta, lay, testReg, 2.0 * m_tolExtrapolate).isInside(xPred))
		  continue;
		const double tanT = regionTanT(sta, lay, testReg);
		PatRange xRange = symmetricRange(xPred + yPred * tanT, m_tolExtrapolate);
		// if we have more information from a state, use it
		if (0 != state)
		  xRange.intersect(predictXinRegion(state,sta,lay,testReg));
		if ( m_printing ) {
		  info() << "** Search in IT sta " << sta << " lay " << lay << " region " << testReg
		    << " xPred " << xPred + yPred * tanT << endmsg;
		}

		HitRange rangeYY = hitsInRange( xRange, sta, lay, testReg );
		HitRange::iterator it = rangeYY.begin(), end = rangeYY.end();
		for ( ; end != it; ++it ) {
		  PatFwdHit* hit = *it;
		  if ( hit->isUsed() ) continue;
		  updateHitForTrack( hit, track.yAtZ(hit->hit()->zAtYEq0()), 0);
		  if ( m_tolExtrapolate < std::abs(track.distance(hit)) ) continue;
		  if (!hit->hit()->isYCompatible(yPred, m_yTolSensArea)) continue;
		  if ( m_enforceIsolation && !isIsolated(it, rangeYY) ) continue;
		  if ( m_printing ) {
		    info() << "Add coord ";
		    debugFwdHit( hit, info() );
		  }
		  track.addCoord( hit );
		  dirty = true;
		}
	      }
	      if ( dirty ) {
		const unsigned nSta = track.nStations();
		const unsigned minPlanes = (track.nPlanes() <= 8) ?
		  8 : m_minTotalPlanes;
		const double maxchi2hit = hasOT ? m_maxChi2HitOT : m_maxChi2HitIT;
		if (hasOT) {
		  fitOK = m_seedTool->fitTrack<PatSeedTool::TrackType::Mixed>(track, maxchi2hit, minPlanes, false, m_printing);
		} else {
		  fitOK = m_seedTool->fitTrack<PatSeedTool::TrackType::ITOnly>(track, maxchi2hit, minPlanes, false, m_printing);
		}
		dirty = nSta != track.nStations();
	      }
	    }
	  }
	}
      } catch (const std::length_error& e) {
	if ( m_printing ) info() << "  -- trying to add too many hits" << endmsg;
	continue;
      }

      if ( m_printing ) {
	info() << " --- We have found " << track.nStations() << " stations, fitOK " <<
	  fitOK << "." << endmsg;
	for( const PatFwdHit* hit: track.coords() )
	  debugFwdHit( hit, info() );
      }

      if ( m_fieldOff ) {
        // cut on track curvature when running without magnetic field
        double dz = 0.5 * ((*track.coordBegin())->z() -
                           (*(track.coordBegin() + (track.nCoords() - 1)))->z());
        if ( m_tolCollect / dz / dz < std::abs( track.curvature() ) ) {
          if ( m_printing ) info() << "    magnet off: curvature too high: "
                                   << track.curvature() << endmsg;
          continue;
        }
      }
      
      if (dirty) {
	const unsigned minPlanes = (track.nPlanes() <= 8) ?
	  8 : m_minTotalPlanes;
	const double maxchi2hit = hasOT ? m_maxFinalChi2 : m_maxChi2HitIT;
	if (hasOT) {
	  fitOK = m_seedTool->fitTrack<PatSeedTool::TrackType::Mixed>(track, maxchi2hit, minPlanes, false, m_printing);
	} else {
	  fitOK = m_seedTool->fitTrack<PatSeedTool::TrackType::ITOnly>(track, maxchi2hit, minPlanes, false, m_printing);
	}
      }
      if (!fitOK) continue;
      if (track.chi2() > m_maxTrackChi2LowMult) continue;

      unsigned nbSta = track.nStations();
      if (!(3 == nbSta || (2 == nbSta && (centralYIT || fromInsideBeampipe ||
		PatSeedTrack::IT == track.trackRegion()))))
	continue;
      track.setValid(true);
      finalSelection.push_back(&track);
    }
    // remove clones from final selection and store tracks
    killClonesAndStore(finalSelection, outputTracks, m_maxUsedFractITOT);
  }

  if ( m_measureTime ) m_timer->stop( m_timeItOt );
}

void PatSeedingTool::collectLowQualTracks(
                                          std::vector<PatSeedTrack>& lowQualTracks,
                                          std::vector<PatSeedTrack*>& finalSelection,
                                          std::vector<LHCb::Track*>& outputTracks,
                                          const LHCb::State* state)
{
  if (lowQualTracks.empty()) return;

  std::vector<PatSeedTrack>::iterator itS;
  FwdHits stereo;

  if ( m_measureTime ) m_timer->start( m_timeLowQual );

  finalSelection.clear();
  finalSelection.reserve(lowQualTracks.size());
  for (itS = lowQualTracks.begin(); lowQualTracks.end() != itS; ++itS) {
    PatSeedTrack& track = *itS;
    // default to not valid
    track.setValid(false);

    // check that not too many hits are used by now
    // remove up to three used hits from the track
    int nUsed = 0;
    int maxUsed = int(std::ceil(m_maxUsedFractLowQual * track.nCoords()));
    for (auto it = track.coordBegin();
         track.coordEnd() != it; ++it) {
      if ((*it)->isUsed()) {
        if (++nUsed <= 3) it = track.removeCoord(it);
        else if (0 >= maxUsed--) break;
      }
    }
    if (maxUsed < 0) continue;
    if (track.nPlanes() < unsigned(m_minTotalPlanes)) continue;
    bool isOT = (*track.coordBegin())->isOT();
    m_tolCollect = isOT ? m_tolCollectOT : m_tolCollectIT;
    m_maxChi2Hit = isOT ? m_maxChi2HitOT : m_maxChi2HitIT;

    // restore coordinates
    track.setYParams(0., 0.);
    track.sort();
    stereo.clear();
    try {
      for(PatFwdHit* hit: track.coords()) {
	hit->setSelected( true );
	hit->setIgnored( false );
	if (!hit->hit()->isX()) {
	  const double x = hit->hit()->xAtYEq0();
	  const double z = hit->hit()->zAtYEq0();
	  const double y = (track.xAtZ(z)- x) / hit->hit()->dxDy();
	  hit->setProjection(y * m_zForYMatch / z);
	  stereo.push_back(hit);
	}
      }
    } catch (const GaudiException& e) {
      throw std::length_error("static storage of FwdHits exhausted.");
    }
    // refit in yz
    double y0, sl;
    bool ok = fitLineInY( stereo, y0, sl );
    if (!ok) {
      if ( m_printing ) info() << "  -- fitLineInY failed" << endmsg;
      continue;
    }
    track.setYParams(y0, sl);

    // check that pointing constraint still holds
    double yAtOrigin = y0;
    double yCorr     = sl * sl * track.curvature() * track.curvature() / m_yCorrection;
    yAtOrigin = y0 - std::copysign(yCorr, sl);
    if ( m_maxYAtOriginLowQual < std::abs(yAtOrigin) ) continue;

    // check for compatibility with state (if we have one)
    if ( 0 != state && !isGoodSlope(state, false, sl) ) {
      if ( m_printing ) info() << "  -- ty not compatible with state given" << endmsg;
      continue;
    }

    //== Check for enough OT if not near boundary
    if ( isOT ) {
      double y = y0 + m_zForYMatch * sl;
      if ( m_centralYOT < std::abs(y) ) {
        std::array<int, 3> nInStation = { {0, 0, 0} };
        for(const PatFwdHit* hit: track.coords()) {
          if ( hit->hit()->isX() ) continue;
          if ( hit->isIgnored() ) continue;
          nInStation[ hit->hit()->station() ]++;
        }
	if (m_minStHitsPerStaOT >
	    std::min(nInStation[0], std::min(nInStation[1], nInStation[2])) ||
	    m_minTotStHitsOT > nInStation[0] + nInStation[1] + nInStation[2]) {
          if ( m_printing ) info() << "Not enough OT stereo in some station : "
                                   << nInStation[0] << " "
                                   << nInStation[1] << " "
                                   << nInStation[2] << endmsg;
          continue;
        }
      }
    }

    if ( isOT && m_otNHitsLowThresh > track.nCoords() &&
         2 * (track.nCoords()-track.nPlanes()+1) < track.nbOnSide() ) {
      if ( m_printing ) info()  << "Too many on side " << track.nbOnSide() << " with only "
                                << track.nCoords() << " coordinates" << endmsg;
      continue;
    }

    track.sort();
    if ( m_printing ) info() << "-- Before fit, nPlanes = " << track.nPlanes() << endmsg;
    ok = m_seedTool->fitTrack( track, m_maxFinalChi2, m_minTotalPlanes - 1, false, m_printing );

    if ( m_printing ) info() << "After fit status "<< ok << " chi2 " << track.chi2()
                             << ", nPlanes = " << track.nPlanes() << endmsg;
    if ( isOT && m_otNHitsLowThresh > track.nCoords() &&  // OT short track -> not too many neighbours
         2 * (track.nCoords() - track.nPlanes() +1 ) < track.nbOnSide() ) {
      if ( m_printing ) {
        info()  << "Too many on side " << track.nbOnSide() << " with only "
                << track.nCoords() << " coordinates" << endmsg;
        for( const PatFwdHit* hit: track.coords() )
          debugFwdHit( hit, info() );
      }
      continue;
    }

    if ( m_maxFinalTrackChi2 < track.chi2() || !ok) {
      if ( m_printing ) info() << "Chi2 too big" << endmsg;
      continue;
    }
    // if a track makes it up to this point, it's valid, and we add it
    // to the final selection
    track.setValid( true );
    finalSelection.push_back(&track);
  }
  killClonesAndStore(finalSelection, outputTracks, m_maxUsedFractLowQual);

  if ( m_measureTime ) m_timer->stop( m_timeLowQual );
}

//=========================================================================
// Store the track in the final container
//=========================================================================
void PatSeedingTool::storeTrack ( const PatSeedTrack& track,
                                  std::vector<LHCb::Track*>& outputTracks ) const
{
  LHCb::Track* out = new LHCb::Track();
  out->setType( LHCb::Track::Ttrack );
  out->setHistory( LHCb::Track::PatSeeding );
  out->setPatRecStatus( LHCb::Track::PatRecIDs );

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() << "==== Storing track " << outputTracks.size() << endmsg;
  for( PatFwdHit* hit: track.coords() ) {
    out->addToLhcbIDs( hit->hit()->lhcbID() );
    //== Tag used coordinates
    hit->hit()->setStatus(Tf::HitBase::UsedByPatSeeding);
    hit->setIsUsed(true);
  }
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) {
    for( const PatFwdHit* hit: track.coords() )
      debugFwdHit( hit, debug() );
  }
  // put the chi^2 and NDF of our track fit onto the track (e.g. for use in
  // the trigger before the track is fitted with the Kalman filter)
  // we save chi^2/ndf on our track, so compensate
  unsigned ndf = track.nCoords() - 5;
  out->setChi2AndDoF(double(ndf) * track.chi2(), ndf);
  
  // get momentum estimate from track using the momentum estimation tool
  LHCb::State temp(
                   Gaudi::TrackVector(track.xAtZ(m_zReference), track.yAtZ(m_zReference),
                                      track.xSlope(m_zReference), track.ySlope(m_zReference), 0.),
                   m_zReference, LHCb::State::AtT);
  double qOverP, sigmaQOverP;
  if (m_momentumTool->calculate(&temp, qOverP, sigmaQOverP, true).isFailure())
  {
    // if our momentum tool doesn't succeed, we have to try ourselves
    // so get q/p from curvature
    const double scaleFactor = m_magFieldSvc->signedRelativeCurrent();
    if (std::abs(scaleFactor) < 1e-4) {
      // return a track of 1 GeV, with a momentum uncertainty which makes it
      // compatible with a 1 MeV track
      qOverP = ((track.curvature() < 0) ? -1. : 1.) *
        ((scaleFactor < 0.) ? -1. : 1.) / Gaudi::Units::GeV;
      sigmaQOverP = 1. / Gaudi::Units::MeV;
    } else {
      // get qOverP from curvature
      qOverP = track.curvature() * m_momentumScale / (-1*scaleFactor);
      sigmaQOverP = 0.5 * qOverP; // be conservative
    }
  }
  temp.setQOverP(qOverP);
  //== overestimated covariance matrix, as input to the Kalman fit
  Gaudi::TrackSymMatrix& cov = temp.covariance();
  cov(0,0) = m_stateErrorX2;
  cov(1,1) = m_stateErrorY2;
  cov(2,2) = m_stateErrorTX2;
  cov(3,3) = m_stateErrorTY2;
  cov(4,4) = sigmaQOverP * sigmaQOverP;

  for( const double z: m_zOutputs ) {
    temp.setX(track.xAtZ(z));
    temp.setY(track.yAtZ(z));
    temp.setZ(z);
    temp.setTx(track.xSlope(z));
    temp.setTy(track.ySlope(z));
    out->addToStates( temp );
  }

  outputTracks.push_back( out );
}
//=========================================================================
//  Debug one coordinate
//=========================================================================
void PatSeedingTool::debugFwdHit ( const PatFwdHit* coord, MsgStream& msg ) const
{
  msg << format( " Z %10.2f Xp %10.2f X%10.2f  St%2d lay%2d typ%2d used%2d",
                 coord->z(),
                 coord->projection(),
                 coord->x(),
                 coord->hit()->station(),
                 coord->hit()->layer(),
                 coord->hit()->region(),
                 coord->isUsed() );
  if ( coord->isOT() ) {
    msg << format( " P%2d N%2d RL%2d Drift %7.3f",
                   coord->hasPrevious(),
                   coord->hasNext(),
                   coord->rlAmb(),
                   coord->driftDistance() );
  }
  //else {
  //msg << format( " High Threshold %2d         ", coord->highThreshold() );
  //}

  //for ( std::vector<int>::const_iterator itM = coord->MCKeys().begin();
  //    coord->MCKeys().end() != itM; ++itM ) {
  //msg << " " << *itM;
  //}
  msg << endmsg;
}

//=========================================================================
//  add the new track to the vector, if better, this means longer than
//  any other track sharing >70% of hits
//=========================================================================
void PatSeedingTool::addIfBetter (PatSeedTrack& track,
                                  std::vector<PatSeedTrack>& pool) const
{
  bool printing = msgLevel ( MSG::VERBOSE );

  if ( printing ) {
    info() << ".. Compare new track:" << endmsg;
    for( const PatFwdHit* hit: track.coords() )
      debugFwdHit( hit, info() );
  }
  track.updateIDs();

  // points to invalid entry which we can replace instead of making the
  // vector bigger; is 0 if no invalid entry found
  PatSeedTrack* lastInvalidTrack = 0;
  bool stored = false;
  for( PatSeedTrack& othertrack: pool ) {
    if ( !othertrack.valid() ) {
      lastInvalidTrack = &othertrack;
      continue;
    }
    if ((othertrack.bloomfilter() & track.bloomfilter()).empty()) {
      // if no common hits whatsoever, go to next track in pool
      continue;
    }

    // this is a funny way to check if there are too many common hits,
    // but we can stop early if this is the case
    int nCommonMax = int(std::ceil(m_commonXFraction *
	  std::min(othertrack.nCoords(), track.nCoords())));
    int nUniqueMin = track.nCoords() + 1 - nCommonMax;
    for (const PatFwdHit* hit1: othertrack.coords()) {
      if (track.coordEnd() !=
           std::find(track.coordBegin(), track.coordEnd(), hit1)) {
        if (0 > --nCommonMax) break;
      } else {
	if (0 > --nUniqueMin) break;
      }
    }

    if (0 <= nCommonMax) continue;
    //== enough common in stored track: Keep the best (longest / best chi2)
    if ( printing ) {
      int nCommonMax2 = int(std::ceil(m_commonXFraction *
                                     std::min(othertrack.nCoords(), track.nCoords())));
      // if we're debugging, we can be slow in determining where othertrack
      // is in pool
      int nTrack = 0;
      for( const PatSeedTrack& tmp: pool ) {
        if (&tmp == &othertrack) break;
        ++nTrack;
      }
      info() << "Track " << nTrack << " size " <<  othertrack.nCoords()
             << " chi2 " << othertrack.chi2() << " has <= " << nCommonMax2
             << " hit shared with current track of size " << track.nCoords()
             << " chi2 " << track.chi2() << endmsg;
      for( const PatFwdHit* hit: track.coords() )
        debugFwdHit( hit, info() );
    }
    double qual1 = 0., qual2 = 0.;
    if (0. != m_qualityWeights[0]) {
      qual1 += m_qualityWeights[0] * double(track.nPlanes());
      qual2 += m_qualityWeights[0] * double(othertrack.nPlanes());
    }
    if (0. != m_qualityWeights[1]) {
      qual1 += m_qualityWeights[1] * track.chi2();
      qual2 += m_qualityWeights[1] * othertrack.chi2();
    }
    if ( qual1 < qual2 ) {
      if ( printing ) info() << "  new is shorter / higher chi2 -> ignore new" << endmsg;
      return;
    }
    // reuse an entry instead of erasing the old and allocating a new one,
    // if there's more than one to be erased, flag it invalid instead of
    // erasing because erasing might involve moving a lot of data around
    // to close the resulting gap...
    if ( !stored ) {
      if ( printing ) info() << "  replacing old" << endmsg;
      othertrack = track;
      stored = true;
    } else {
      if ( printing ) info() << "  invalidating old" << endmsg;
      othertrack.setValid(false);
      lastInvalidTrack = &othertrack;
    }
  } // end of loop over pool
  if ( !stored ) {
    if ( printing ) info() << "  Store new one" << endmsg;
    // if we can reuse an invalid entry, we do so
    if (0 != lastInvalidTrack)
      *lastInvalidTrack = track;
    else
      pool.push_back( track );
  }
  if ( m_printing ) {
    info() << " ****** stored track " << endmsg;
    for( const PatFwdHit* hit: track.coords() )
      debugFwdHit( hit, info() );
  }
  return;
}

//=========================================================================
//  Find the x candidates in the specified layer and region. Results in candidates
//  returns false if there are too few hits to form a track in the region in question
//=========================================================================
void PatSeedingTool::findXCandidates ( unsigned lay, unsigned reg,
                                       std::vector<PatSeedTrack>& pool,
                                       const LHCb::State* state)
{
  HitRange::const_iterator itBeg, itH0, itH1, itH2;

  // depending on the region, we have to correct for the OT tilt because it
  // causes a z displacement (negligible for the IT):
  // we do not know the absolute z for a hit, but for tracks with ty = 0, the z
  // coordinates we'd get would be consistent
  // for tracks with ty != 0, the tilt causes z coordinates of hits to deviate,
  // and we account for that by widening the search windows based on the
  // expected minimal/maximal slopes ty in that OT region

  // decode layer bit mask to determine which layer to use in which station
  // to form the initial parabola
  const unsigned lay0 = (lay&1)?3:0, lay1 = (lay&2)?3:0, lay2 = (lay&4)?3:0;
  // if we should reconstruct cosmics, we can no longer rely on the fact that
  // very few tracks migrate between regions
  // thus, we "decode" reg in a way similar to what is done for layers above
  // by applying a per station region increment (only in the cosmic case)
  const unsigned reg0 = reg & 7u;
  const unsigned reg1 = (reg0 + (m_cosmics?((reg >> 3) & 7u):0)) % m_nReg;
  const unsigned reg2 = (reg0 + (m_cosmics?((reg >> 6) & 7u):0)) % m_nReg;

  const double zScaling  = ( regionZ0(2,lay2,reg2) - regionZ0(0,lay0,reg0) ) /
    ( regionZ0(0,lay0,reg0) - m_zMagnet );
  const double zScaling2 = ( regionZ0(2,lay2,reg2) - regionZ0(0,lay0,reg0) ) /
    regionZ0(0,lay0,reg0);

  bool matchIn0 = false;

  // if we have a state, we don't need hits from the whole region
  HitRange rangeX0 = (0 == state) ? hits(0, lay0, reg0) :
    hitsInRange(predictXinRegion(state, 0, lay0, reg0), 0, lay0, reg0);
  // NB: no correction for the tilt for a single point
  for ( itH0 = rangeX0.begin(); rangeX0.end() != itH0; ++itH0 ) {
    PatFwdHit* hit0 = *itH0;
    if ( hit0->isUsed() )  continue;

    matchIn0 = msgLevel( MSG::VERBOSE );

    double x0 = hit0->hit()->xAtYEq0();
    double z0 = hit0->hit()->zAtYEq0();

    bool combine0 = false;
    if (hit0->hasNext() && rangeX0.end() != (itH0 + 1) && !(*(itH0 + 1))->isUsed()) {
      hit0 = *(++itH0);
      if ( matchIn0 ) info() << "*** Hit0 " << x0 << " + " << hit0->x();
      combineCluster(*(itH0 - 1), *itH0, x0, z0);
      combine0 = true;
    } else if ( matchIn0 ) {
      info() << "*** Hit0 " << hit0->x();
    }

    // isolation in station 0?
    if ( m_enforceIsolation && !isIsolated(itH0, rangeX0) )
      continue;

    m_printing = matchIn0;

    // derive search windows based on center of magnet compatibility and
    // (if applicable) minimum momentum, and take the tightest combination
    PatRange x2Range = magnetRange(x0, zScaling).intersect(
                                                           impactRange(x0, zScaling2));
    // no need to correct for tilt here, the windows are wide anyway

    // if we know more because we have a state, make use of that knowledge
    if (0 != state)
      x2Range.intersect(predictXinRegion(state, 2,lay2,reg2));

    if ( m_printing ) info() << " x2Min " << x2Range.min() <<
      " x2Max " << x2Range.max() << endmsg;

    HitRange rangeX2 = hitsInRange( x2Range, 2, lay2, reg2 );
    for ( itH2 = rangeX2.begin(); rangeX2.end() != itH2; ++itH2 ) {
      PatFwdHit* hit2 = *itH2;
      if ( hit2->isUsed() ) continue;

      double x2 = hit2->hit()->xAtYEq0();
      double z2 = hit2->hit()->zAtYEq0();

      m_printing = matchIn0;

      bool combine2 = false;
      if (hit2->hasNext() && rangeX2.end() != (itH2 + 1) && !(*(itH2 + 1))->isUsed()) {
        hit2 = *(++itH2);
        if ( m_printing ) info() << "    hit2 " << x2 << " + " << hit2->x();
        combineCluster(*(itH2 - 1), *itH2, x2, z2);
	combine2 = true;
      } else if ( m_printing ) {
        info() << "    hit2 " <<  hit2->hit()->xAtYEq0();
      }

      // isolation in station 2?
      if ( m_enforceIsolation && !isIsolated(itH2, rangeX2) )
        continue;

      //== Check center of magnet compatibility
      double slope = ( x2 - x0 ) / ( z2 - z0 );
      // check for compatibility with slope
      if (0 != state && !isGoodSlope(state, true, slope))
        continue;

      // work out by how much we need to propagate wrt z0
      const double dz = regionZ0(1,lay1,reg1) +
	regionDzDx(1,lay1,reg1) * (x0 + slope * (regionZ0(1,lay1,reg1) - z0)) - z0;
      // "track" parameters for hit position prediction in T2
      double a, b, c;
      {
	// work out intercept at z = 0
	const double intercept =  x0 - z0 * slope;
	// get curvature
	c = - m_initialArrow * intercept;
	// use it to obtain track parameter predictions
	const double dz0 = z0 - m_zReference;
	const double dz2 = z2 - m_zReference;
	const double D0 = x0 - c * dz0 * dz0 * (1. + dz0 * m_dRatio);
	const double D2 = x2 - c * dz2 * dz2 * (1. + dz2 * m_dRatio);
	const double denom = 1. / (dz2 - dz0);
	a = denom * (dz2 * D0 - dz0 * D2);
	b = denom * (D2 - D0);
      }
      double x1Pred, slopePred;
      {
	// how far is layer in T2 away from m_zReference
	const double dz1 = dz + z0 - m_zReference;
	// work out prediction for x position and slope there
	x1Pred = a + dz1 * (b + dz1 * (c * (1. + dz1 * m_dRatio)));
	slopePred = b + dz1 * c * (2. + 3. * dz1 * m_dRatio);
      }
      // obtain tilt-corrected range around x1Pred, open window of size
      // m_curveTol
      PatRange x1Range = tiltCorrectedRange(1,lay1,reg1,reg1,
                                            dz, slopePred, x1Pred, m_curveTol);
      // if we know more because we have a state, make use of that knowledge
      if (0 != state) x1Range.intersect(predictXinRegion(state, 1, lay1, reg1));

      if ( m_printing ) info() << " x1Min " << x1Range.min() << " x1Max " <<
        x1Range.max() << " linear " << x0 + dz * slope << endmsg;

      HitRange rangeX1 = hitsInRange( x1Range, 1, lay1, reg1 );
      // find best (and next-to-best if applicable) candidate hits in terms
      // of distance of hit to predicted x position in T2
      HitRange::const_iterator itH1Best = rangeX1.end(), itH1NextBest = rangeX1.end();
      double dx1best = 1e42;
      for ( itH1 = rangeX1.begin(); rangeX1.end() != itH1; ++itH1 ) {
        PatFwdHit* hit1 = *itH1;
        if ( hit1->isUsed() ) continue;
	// obtian new prediction with correct hit z
	const double dz1 = hit1->hit()->zAtYEq0() - m_zReference;
	const double x1p = a + dz1 * (b + dz1 * c * (1. + dz1 * m_dRatio));
	const double r1 = (hit1->hit()->othit())?hit1->hit()->othit()->untruncatedDriftDistance(0.):0.;
        const double dx1 = hit1->hit()->xAtYEq0() - x1p;
	const double absdx1 = std::min(std::abs(dx1-r1), std::abs(dx1 + r1));
	if (absdx1 < dx1best) {
	  itH1NextBest = itH1Best;
	  dx1best = absdx1;
	  itH1Best = itH1;
	  // if part of cluster, skip 2nd hit in cluster
	  if (hit1->hasNext() && rangeX1.end() != (itH1 + 1) &&
	      !(*(itH1 + 1))->isUsed()) {
	    ++itH1;
	    // update dx1best if better for 2nd hit in cluster
	    hit1 = *itH1;
	    const double dz1 = hit1->hit()->zAtYEq0() - m_zReference;
	    const double x1p = a + dz1 * (b + dz1 * c * (1. + dz1 * m_dRatio));
	    const double r1 = (hit1->hit()->othit())?hit1->hit()->othit()->untruncatedDriftDistance(0.):0.;
	    const double dx1 = hit1->hit()->xAtYEq0() - x1p;
	    const double absdx1 = std::min(std::abs(dx1-r1), std::abs(dx1 + r1));
	    if (absdx1 < dx1best)
	      dx1best = absdx1;
	  }
	}
      }
      // we're done if there is no best candidate hit
      if (itH1Best == rangeX1.end()) continue;
      // ok, make new array with best and next-to-best candidate hits
      StaticArray<PatFwdHit*, 4> candsT2;
      candsT2.push_back(*itH1Best);
      if ((*itH1Best)->hasNext() && rangeX1.end() != (itH1Best + 1) &&
	  !(*(itH1Best + 1))->isUsed())
	candsT2.push_back(*(itH1Best + 1));
      if (itH1NextBest != rangeX1.end()) {
	candsT2.push_back(*itH1NextBest);
        if ((*itH1NextBest)->hasNext() && rangeX1.end() != (itH1NextBest + 1) &&
	    !(*(itH1NextBest + 1))->isUsed())
	  candsT2.push_back(*(itH1NextBest + 1));
      }

      auto rangeX1a = make_range(candsT2.begin(), candsT2.end());
      for ( auto itH1a = rangeX1a.begin(); rangeX1a.end() != itH1a; ++itH1a ) {
        PatFwdHit* hit1 = *itH1a;
        if ( hit1->isUsed() ) continue;

        double x1 = hit1->hit()->xAtYEq0();
        double z1 = hit1->hit()->zAtYEq0();

	bool combine1 = false;
        if (hit1->hasNext() && rangeX1a.end() != (itH1a + 1) && !(*(itH1a + 1))->isUsed()) {
          hit1 = *(++itH1a);
          if ( m_printing ) info() << "         ==> found hit1 " << x1 << " + " << hit1->x() << endmsg;
          combineCluster(*(itH1a - 1), *itH1a, x1, z1);
	  combine1 = true;
        } else if ( m_printing ) {
          info() << "         ==> found hit1 " << x1 << endmsg;
        }

        // isolation in station 1?
        if (m_enforceIsolation && !isIsolated(itH1a, rangeX1a))
          continue;
	if (m_nDblOTHitsInXSearch && isRegionOT(reg0) &&
	    isRegionOT(reg1) && isRegionOT(reg2)) {
	  unsigned nDoubleHitsOT = 0;
	  if (combine0) ++nDoubleHitsOT;
	  if (combine2) ++nDoubleHitsOT;
	  if (combine1) ++nDoubleHitsOT;
	  if (nDoubleHitsOT < m_nDblOTHitsInXSearch) continue;
	}

        PatSeedTrack track( x0, x1, x2, z0, z1, z2, m_zReference, m_dRatio );
        // one more slope check to make sure our third hit is ok
        if (0 != state && !isGoodSlope(state, true, track.xSlope(z1)))
          continue;
        track.setYParams( 0, 0);
        int nbMissed = 0;

	try {
	  // if we reconstruct cosmics, we need to look in all regions for hits
	  // (cosmic IT hits are rarely in different stations)
	  unsigned tmplay[3] = { lay0, lay1, lay2 };
	  for (unsigned reginc = 0; reginc < (m_cosmics?m_nReg:1); ++reginc) {
	    unsigned regc = (reg0 + reginc) % m_nReg; // current region
	    for ( unsigned nLay = 2; nLay--; ) {
	      for ( unsigned sta = 0; sta < m_nSta; ++sta ) {
		// start with the other 3 layers (i.e. those not used to form
		// the initial parabola; reason is that we can potentially kill
		// the seed earlier because we exceed the permitted number of
		// misses)
		unsigned olay = tmplay[sta] ^ (nLay ? 3 : 0 ); // x layers have numbers 0 and 3
		// work out the range in x in which to look for hits
		// apply correction for the tilt
		PatRange xRange = tiltCorrectedRange(sta,olay,regc,regc,
		    track, 2. * m_tolCollect);
		if (m_cosmics) {
		  // cosmic case: skip if the track does not come close to the
		  // region in question
		  if (regionX(sta, olay, regc).intersect(xRange).empty())
		    continue;
		}
		// if we have a state, intersect with current window
		if (0 != state)
		  xRange.intersect(predictXinRegion(state,sta,olay,regc));

		HitRange range = hitsInRange( xRange, sta, olay, regc );
		unsigned nCoordsBefore = track.nCoords();
		HitRange::iterator it = range.begin(), end = range.end();
		for ( ; end != it; ++it) {
		  PatFwdHit* hit = *it;
		  if (hit->isUsed()) continue;
		  //== restore default measure (do it here, we use the drift
		  //   distance in a moment)
		  restoreCoordinate(hit);
		  // here, we know z of the hit at y = 0, so we can compare to a
		  // more accurate x prediction of the track
		  const double tol = m_tolCollect +
		    hit->isOT() * std::abs(hit->driftDistance());
		  xRange = tiltCorrectedRange(hit, regc, track, tol);
		  if (!xRange.isInside(hit->x())) continue;
		  if ( m_enforceIsolation && !isIsolated(it, range))
		    continue;
		  track.addCoord( hit );
		}
		if (track.nCoords() == nCoordsBefore && reg0 == regc) ++nbMissed;
		if ( m_printing ) {
		  info() << format( "         --- sta%2d lay%2d xPred%8.2f "
		      "found%2d nbMissed%2d", sta, olay,
		      track.xAtZ( regionZ0(sta,olay,regc) ),
		      track.nCoords() - nCoordsBefore,  nbMissed )
		    << endmsg;
		}
		if ( m_maxMisses < nbMissed ) break;
	      }
	      if ( m_maxMisses < nbMissed ) break;
	    }
	    if ( m_maxMisses < nbMissed ) break;
	  }
	} catch (const std::length_error& e) {
	  if ( m_printing ) info() << "  -- trying to add too many hits" << endmsg;
	  continue;
	}
	if ( m_maxMisses < nbMissed ) continue;

	track.sort();
	//== Fit the track
	if ( m_printing ) info() << " -- start fitting, nCoord = " << track.nCoords() << endmsg;
	bool fitOK = isRegionOT(reg0) ?
	  m_seedTool->fitTrack<PatSeedTool::TrackType::OTOnly>( track, m_maxChi2Hit, m_minXPlanes, true, m_printing ) :
	  m_seedTool->fitTrack<PatSeedTool::TrackType::ITOnly>( track, m_maxChi2Hit, m_minXPlanes, true, m_printing );
	if ( !fitOK ) {
	  if ( m_printing ) info() << "    -- fit failed" << endmsg;
	  continue;
	}
	if ( m_maxTrackChi2 < track.chi2() ) {
	  if ( m_printing ) info() << "    -- global chi2 too high " << track.chi2() << endmsg;
	  continue;
	}

        if (m_fieldOff) {
          // cut on curvature when running without magnetic field
          double dz2 = 0.5 * (z2 - z0);
          if (m_tolCollect / dz2 / dz2 < std::abs(track.curvature())) {
            if ( m_printing ) info() << "    magnet off: curvature too high: "
                                     << track.curvature() << endmsg;
            continue;
          }
        }

        //== Limited OT multiplicity -> check if compatible with in the central Y region
        if ( isRegionOT(reg0) && 8 > track.nCoords() ) {
          fitOK = m_seedTool->fitTrack<PatSeedTool::TrackType::OTOnly>( track, m_maxFinalChi2, m_minXPlanes, true, m_printing );
          if ( !fitOK ) {
            if ( m_printing ) info() << "    -- re-fit with final Chi2 failed" << endmsg;
            continue;
          }  

          if ( 2 * ( track.nCoords() - track.nPlanes() + 1 ) < track.nbOnSide() ) {
            if ( m_printing ) info() << "    -- only " << track.nCoords() << " and " << track.nbOnSide()
                                     << " neighbouring OT hits" << endmsg;
            continue;
          }
          if ( m_maxTrackChi2LowMult < track.chi2() ) {
            if ( m_printing ) info() << "    -- global chi2 too high " << track.chi2() << endmsg;
            continue;
          }
        }

        if ( isRegionIT(reg0) && 3 > track.nbHighThreshold() ) {
          if ( m_printing ) info() << "    -- only " << track.nbHighThreshold()
                                   << " high threshold IT hits" << endmsg;
          continue;
        }

        // one last check for the slope - just in case something was
        // changed by the fit
        if (0 != state && !isGoodSlope(state, true, track.xSlope(z1)))
          continue;
        addIfBetter( track, pool );
	// ok, best candidate so far passed quality cuts, discard others
	break;
      }
    }
  }

  //== Final refit, as track may have been modified...
  for( PatSeedTrack& track: pool ) {
    if (!track.valid()) continue;

    track.sort();
    m_printing = msgLevel( MSG::DEBUG );

    bool fitOK = isRegionOT(reg0) ?
      m_seedTool->fitTrack<PatSeedTool::TrackType::OTOnly>( track, m_maxChi2Hit, m_minXPlanes, true, m_printing ) :
      m_seedTool->fitTrack<PatSeedTool::TrackType::ITOnly>( track, m_maxChi2Hit, m_minXPlanes, true, m_printing );
    if (fitOK && ( m_maxFinalChi2 >= track.chi2() ))
    // cut on chi2 per DoF
    track.setValid( fitOK && ( m_maxFinalChi2 >= track.chi2() ) );
  }
}

//=========================================================================
//  Find all stereo hits compatible with the track.
//=========================================================================
void PatSeedingTool::collectStereoHits ( PatSeedTrack& track,
                                         unsigned reg, FwdHits& stereo,
                                         const LHCb::State* state )
{
  stereo.clear();
  int minRegion = reg;
  int maxRegion = reg + 1;
  if ( isRegionOT(reg) && 7 > track.nCoords() ) {
    // central region -> try both OT halves
    minRegion = 0;
    maxRegion = 2;
  }
  // to reconstruct cosmics, we look in all regions
  if (m_cosmics) minRegion = 0, maxRegion = m_nReg;
  for ( int sta = 0; 3 > sta; ++sta ) {
    for ( int sLay = 1; 3 > sLay ; ++sLay ){
      for ( int testRegion = minRegion; maxRegion > testRegion; ++testRegion ) {
        // work out range in x direction due to stereo angle
        PatRange range = regionY(sta, sLay, testRegion);
        if ( !m_cosmics &&
             ( ( isRegionOT(testRegion) && 7 > track.nCoords() ) ||
               ( isRegionIT(testRegion) && 4 > track.nCoords() ) ) ) {
          // Low number of coordinates: if in OT or the IT stations to the
          // left or the right of the beam pipe, ask Y to be close to 0...
          switch (testRegion) {
          case 0:
            range.min() = -m_centralYOT;
            break;
          case 1:
            range.max() = m_centralYOT;
            break;
          case 2:
          case 3: // regions 2 and 3 are treated alike
            range = symmetricRange(0., m_centralYIT);
          default:
            break;
          }
        }
        range *= regionTanT(sta, sLay, testRegion); // convert to x

        // get tilt corrected range as predicted from track, open window
        // of size m_tolCollect
        range += tiltCorrectedRange(sta, sLay, testRegion, testRegion,
                                    track, m_tolCollect);
        // use additional info from state to narrow down the range further
        // if such info is available
        if (0 != state)
          range.intersect(predictXinRegion(state,sta,sLay,testRegion));

        if ( m_printing )
          info() << format( "Stereo in sta%2d lay%2d region%2d xMin %7.2f xmax %7.2f",
                            sta, sLay, testRegion, range.min(), range.max() ) << endmsg;

        std::array<unsigned char, 20> nDense;
        nDense.fill( 0);

	try {
	  HitRange rangeW = hitsInRange( range, sta, sLay, testRegion);
	  HitRange::const_iterator it = rangeW.begin(), end = rangeW.end();
	  for ( ; end != it; ++it ) {
	    PatFwdHit* hit = *it;
	    if ( hit->isUsed() ) continue;
	    const double x = hit->hit()->xAtYEq0();
	    const double z = hit->hit()->zAtYEq0();
	    const double y = (track.xAtZ(z) - x) / hit->hit()->dxDy();
	    if (!hit->hit()->isYCompatible(y, m_yTolSensArea)) continue;

	    // check if the hit is isolated
	    if ( m_enforceIsolation && !isIsolated(it, rangeW) ) {
	      int idx = int(std::abs(y) * 20. / 3e3);
	      if (idx < 0) idx = 0;
	      if (idx >= 20) idx = 19;
	      if (nDense[idx]++ > 0)
		continue;
	    }
	    // save y position in projection plane
	    hit->setProjection(y * m_zForYMatch / z);
	    hit->setIgnored( false );
	    stereo.push_back( hit );
	  }
	} catch (const GaudiException& e) {
	  throw std::length_error("static storage of FwdHits exhausted.");
	}
      }
    }
  }
  std::sort( stereo.begin(), stereo.end(), 
             Tf::increasingByProjection<PatForwardHit>());
}

//=========================================================================
//  Find the best range and remove (tag) all other hits
//=========================================================================
bool PatSeedingTool::findBestRange (
                                    unsigned reg, int minNbPlanes, FwdHits& stereo ) const
{
  const double maxRangeSize = isRegionOT(reg) ? m_maxRangeOT : m_maxRangeIT;

  auto itBeg = stereo.begin();
  auto itEnd = itBeg + minNbPlanes;

  //== get enough planes fired
  PatFwdPlaneCounter count1( itBeg, itEnd );

  while ( itEnd != stereo.end() &&
          minNbPlanes > count1.nbDifferent() ) {
    count1.addHit( *itEnd++ );
  }
  if ( minNbPlanes > count1.nbDifferent() ) return false;

  //== Try to get the minNbPlanes to 6 if any range < maxRangeSize has it.

  if ( 6 > minNbPlanes ) {
    while ( itEnd != stereo.end() ) {
      while ( itEnd != stereo.end() &&
              maxRangeSize > (*(itEnd-1))->projection() - (*itBeg)->projection() &&
              minNbPlanes+1 > count1.nbDifferent() ) {
        count1.addHit( *itEnd++ );
      }
      if ( minNbPlanes < count1.nbDifferent() ) {  // more planes -> update
        minNbPlanes = count1.nbDifferent();
        if ( 6 == minNbPlanes ) break;
      }
      count1.removeHit( *itBeg++ );
    }
  }

  //== get enough planes fired
  itBeg = stereo.begin();
  itEnd = itBeg + minNbPlanes;
  PatFwdPlaneCounter count( itBeg, itEnd );

  while ( itEnd != stereo.end() &&
          minNbPlanes > count.nbDifferent() ) {
    PatFwdHit* hit = *itEnd++;
    count.addHit( hit );
  }

  auto itBest = itBeg, itLast = itEnd;
  double minDist = (*(itEnd-1))->projection() - (*itBeg)->projection();

  if ( m_printing ) {
    info() << format( "        range minDist %7.2f from %8.3f to %8.3f minNbPlanes %2d",
                      minDist, (*itBeg)->projection(), (*(itEnd-1))->projection(), minNbPlanes )
           << endmsg;
  }

  //== Better range ? Remove first, try to complete, measure spread...
  while ( itEnd != stereo.end() ) {
    count.removeHit( *itBeg++ );
    while ( itEnd != stereo.end() && minNbPlanes > count.nbDifferent() ) {
      PatFwdHit* hit = *itEnd++;
      count.addHit( hit );
    }
    // better means same number of layers, almost same number of hits,
    // smaller spread
    if ( minNbPlanes == count.nbDifferent()  ) {
      if ( minDist > (*(itEnd-1))->projection() - (*itBeg)->projection() ) {
        minDist = (*(itEnd-1))->projection() - (*itBeg)->projection();
        itBest = itBeg;
        itLast = itEnd;
        if ( m_printing ) {
          info() << format( " better range minDist %7.2f from %8.3f to %8.3f minNbPlanes %2d",
                            minDist, (*itBeg)->projection(), (*(itEnd-1))->projection(),
                            minNbPlanes )
                 << endmsg;
        }
      }
    }
  }
  itBeg = itBest;
  itEnd = itLast;

  //== add OT hits on side, except if 6 planes and we are near y=0
  double tolSide = m_tolCollect / std::abs( (*itBeg)->hit()->dxDy() );
  if ( 6 == minNbPlanes &&
       m_centralYOT > std::abs( (*itBeg)->projection() ) &&
       m_centralYOT > std::abs( (*(itEnd-1))->projection() ) ) tolSide = 0.;

  double minProj = (*itBeg)->projection() - tolSide;
  while ( itBeg != stereo.begin() ) {
    PatFwdHit* hit = *(--itBeg);
    if ( minProj > hit->projection() ) {
      ++itBeg;
      break;
    }
  }
  double maxProj = (*(itEnd-1))->projection() + tolSide;
  while ( itEnd != stereo.end() ) {
    PatFwdHit* hit = *itEnd;
    if ( maxProj < hit->projection() )  break;
    ++itEnd;
  }
  if ( m_printing ) {
    info() << format( "  final range minDist %7.2f from %8.3f to %8.3f minNbPlanes %2d",
                      minDist, (*itBeg)->projection(), (*(itEnd-1))->projection(), minNbPlanes )
           << endmsg;
  }

  // remove other hits - we don't want to ignore lots of hits in fitLineInY
  // below
  if (stereo.begin() != itBeg)
    itEnd = std::move(itBeg, itEnd, stereo.begin());
  if (stereo.end() != itEnd)
    stereo.erase(itEnd, stereo.end());

  return true;
}

bool PatSeedingTool::findBestRangeCosmics(
                                          unsigned reg, int minNbPlanes, FwdHits& stereo ) const
{
  // cosmics do not point back to the origin, so they need special treatment
  // (this means the projective approach does not help in this case, so we
  // can't just use a sliding window in the projective plane and count the
  // number of planes inside to find our pattern)

  // check that we have a chance to fulfill the requirements
  if (unsigned(minNbPlanes) > stereo.size()) return false;
  {
    PatFwdPlaneCounter count0(stereo.begin(), stereo.end());
    if (minNbPlanes > count0.nbDifferent()) return false;
  }

  const double maxRangeSize = isRegionOT(reg)?m_maxRangeOT:m_maxRangeIT;
  // best and current cannot overflow their statically allocated space since
  // they're filled from stereo, which would have overflowed first
  FwdHits best, current;
  for (auto it = stereo.begin(); stereo.end() != it; ++it) {
    const PatFwdHit *h1 = *it;
    for (auto jt = it + 1; stereo.end() != jt; ++jt) {
      const PatFwdHit *h2 = *jt;
      // only consider pairs of hits from same region, but different stations
      // (to make sure we have sufficient lever arm)
      if (h1->hit()->region() != h2->hit()->region() &&     // careful: region ranges for IT/OT overlap
	  h1->isOT() == h2->isOT()) continue; // need to check IT/OT as well
      if (h1->hit()->station() == h2->hit()->station()) continue;
      // for each pair of hits, work out slope and intercept
      const double z1 = h1->z();
      const double z2 = h2->z();
      const double y1 = h1->projection() / m_zForYMatch * z1;
      const double y2 = h2->projection() / m_zForYMatch * z2;
      double ty = (y2 - y1) / (z2 - z1);
      double y0 = y2 - ty * z2;

      // we don't want to multiply in the loop below
      ty *= m_zForYMatch; y0 *= m_zForYMatch;

      // now, collect hits around predicted position in projection plane
      current.clear();
      current.reserve(stereo.size());
      std::copy_if( std::begin(stereo), std::end(stereo), std::back_inserter(current), 
                    [=](const PatFwdHit* hit) { 
        return std::abs(hit->projection() - y0 / hit->z() - ty) <= maxRangeSize;
                    });

      // count number of hits - more hits win (fast check first)
      if (current.size() < best.size()) continue;
      // count number of planes
      PatFwdPlaneCounter count(current.begin(), current.end());
      // if insufficient, continue
      if (count.nbDifferent() < minNbPlanes) continue;
      // "mark" current hit collection best
      best.swap(current);
    }
  }

  // if we have a best range collection, use it, else return false
  stereo.swap(best);
  return !stereo.empty();
}

//=========================================================================
//  Fit a line in Y projection, returns the parameters.
//=========================================================================
bool PatSeedingTool::fitLineInY ( FwdHits& stereo, double& y0, double& sl ) const
{
  const double maxYDist = 20. * m_tolCollect;

  // we use builtin arrays here because we know how big they need to be,
  // and we do not want the additional cost of allocating memory dynamically
  // for std::vector
  std::array<double, 3> largestDrift = { { 0., 0., 0. } };
  std::array<PatFwdHit*, 3> seeds = { { nullptr, nullptr, nullptr } };;
  double slopeguess = 0.;
  unsigned npoints = 0;

  for( PatFwdHit* hit: stereo ) {
    if ( hit->isIgnored() ) continue;
    restoreCoordinate(hit);
    // calculate average slope as first guess for IT or OT without
    // or very small drift times
    slopeguess += hit->projection(); ++npoints;
    // go from prejection plane to y of hit
    hit->setProjection( hit->projection() * hit->z() / m_zForYMatch );

    if (!hit->isOT()) continue;
    hit->setRlAmb( 0 );
    const int sta = hit->hit()->station();
    if ( largestDrift[sta] < std::abs(hit->driftDistance()) ) {
      largestDrift[sta] = std::abs(hit->driftDistance());
      seeds[sta] = hit;
    }
  }
  // be paranoid - callers should make sure this never happens, though...
  if (!npoints) return false;
  slopeguess /= double(npoints);

  // place hits with largest drift dist. into seeds[0] and seeds[2]
  if ( largestDrift[1] > largestDrift[0] &&
       largestDrift[1] > largestDrift[2] ) {
    if ( largestDrift[0] < largestDrift[2] ) {
      seeds[0] = seeds[1];
      largestDrift[0] = largestDrift[1];

    } else {
      seeds[2] = seeds[1];
      largestDrift[2] = largestDrift[1];
    }
  } else if ( largestDrift[1] > largestDrift[0] ) {
    seeds[0] = seeds[1];
    largestDrift[0] = largestDrift[1];
  } else if ( largestDrift[1] > largestDrift[2] ) {
    seeds[2] = seeds[1];
    largestDrift[2] = largestDrift[1];
  }

  double bestY0 = 0., bestSl = slopeguess;
  if ( .1 < largestDrift[0] && .1 < largestDrift[2] ) {
    // try all four combinations to resolve ambiguity for seeds[0]. seeds[2]
    // and take best, fix those ambiguities
    double x0 = seeds[0]->projection();
    double x2 = seeds[2]->projection();
    double z0 = seeds[0]->z();
    double z2 = seeds[2]->z();
    double d0 = seeds[0]->driftDistance()/std::abs(seeds[0]->hit()->dxDy());
    double d2 = seeds[2]->driftDistance()/std::abs(seeds[2]->hit()->dxDy());
    if ( m_printing ) {
      info() << format( "  0: z %8.2f y %7.2f d %7.2f", z0, x0, d0 ) << endmsg;
      info() << format( "  2: z %8.2f y %7.2f d %7.2f", z2, x2, d2 ) << endmsg;
    }

    double minChi2 = 1e42;
    for ( int mask = 4; mask--; ) {
      int sign0 = ((mask << 1) & 2) - 1;
      int sign2 = (mask & 2) - 1;
      double locY0 = x0 + sign0 * d0;
      double locY2 = x2 + sign2 * d2;
      sl = (locY2 - locY0) / (z2 - z0);
      y0 = locY0 - sl * z0;
      double chi2 = 0.;
      for( const PatFwdHit* hit: stereo ) {
        if ( hit->isIgnored() ) continue;
        double y = hit->projection();
        double dist = y - y0 - hit->z() * sl;
        double drift = hit->driftDistance() / hit->hit()->dxDy();
        // resolve ambiguity towards line defined by y0, sl
        dist = std::min(std::abs(dist - drift), std::abs(dist + drift));
        if ( m_printing ) info() << format( "    z%8.2f proj%8.2f drift%8.2f dist%8.2f",
                                            hit->z(), hit->projection(), drift, dist ) << endmsg;
        chi2 += dist * dist;
      }
      if ( m_printing ) info() << format( "-- y fit: mask%2d y0 %8.2f sl%10.6f chi2 %12.2f",
                                          mask, y0, sl, chi2 ) << endmsg;
      if ( chi2 < minChi2 ) {
        // current mask is best combination so far, save parameters and fix
        // ambiguities
        minChi2 = chi2;
        bestSl = sl;
        bestY0 = y0;
        seeds[0]->setRlAmb( sign0 );
        seeds[2]->setRlAmb( sign2 );
      }
    }
  }

  y0 = bestY0;
  sl = bestSl;
  do {
    // bias the fit: add a point at y = z = 0, sigma = 200 mm
    PatFwdFitLine line( 0., 0., 0.25e-4 );
    int nOK = 0;
    for( const PatFwdHit* hit: stereo ) {
      if ( hit->isIgnored() ) continue;
      double proj = hit->projection();
      double dist = proj - y0 - hit->z() * sl;
      double drift = hit->driftDistance() / hit->hit()->dxDy();
      // resolve ambiguity towards old fit parameters
      proj += (std::abs(dist - drift) < std::abs(dist + drift))?(-drift):(drift);
      line.addPoint( hit->z(), proj, hit->hit()->weight() );
      ++nOK;
    }
    if ( 4 > nOK ) return false;
    if (!line.solve()) return false;
    y0 = line.ax();
    sl = line.bx();
    // find worst hit
    double worstDist = 0.;
    auto worst = stereo.begin(), end = stereo.end();
    for ( auto it = worst; end != it; ++it ) {
      const PatFwdHit* hit = *it;
      if ( hit->isIgnored() ) continue;
      double proj = hit->projection();
      double dist = proj - y0 - hit->z() * sl;
      double drift = hit->driftDistance() / hit->hit()->dxDy();
      // resolve ambiguity towards new fit parameters
      dist = std::min(std::abs(dist - drift), std::abs(dist + drift));
      if ( dist > worstDist ) {
        worstDist = dist;
        worst = it;
      }
    }
    if ( m_printing ) {
      info() << format( "Worst%7.2f ", worstDist );
      debugFwdHit( *worst, info() );
    }
    if ( worstDist > maxYDist ) {
      (*worst)->setIgnored( true );
      // do a second iteration if we removed a hit
      continue;
    }
  } while ( false );
  return true;
}

//=========================================================================
//  Extract the seed part from forward tracks, and store them
//=========================================================================
// logic:
// for all forward tracks
//   - extract T station part
//   - if T station part is not clone of other tracks found so far
//      - produce new track using T station hit content
//   - if is clone (clone definition is based on track parameters here):
//     - if many shared hits with a track found previously, keep longer one
//     - if track parameters are very similar, but few shared hits,
//       assume that we found different segments of same track, so
//       add missing hits to existing track
void PatSeedingTool::processForwardTracks ( const std::string& location,
                                            std::vector<LHCb::Track*>& outputTracks ) const
{
  if ( m_measureTime ) m_timer->start(m_timeReuseTracks);
  LHCb::Tracks* inputTracks = getIfExists<LHCb::Tracks>( location );
  if ( NULL == inputTracks ) {
    if ( m_measureTime ) m_timer->stop(m_timeReuseTracks);
    return;
  }

  outputTracks.reserve(outputTracks.size() + inputTracks->size());
  std::unordered_map<const LHCb::Track*, PatSeedTrack::XHitFingerPrint> bfs;
  bfs.reserve(inputTracks->size());
  std::vector<LHCb::LHCbID> ids;
  ids.reserve(64);
  // loop over input tracks
  for(const auto* track: *inputTracks) {
    // skip low quality tracks
    if (track->chi2() / track->nDoF() > m_forwardMaxChi2) continue;
    ids.clear();
    // copy the LHCbIDs which are either IT or OT hits
    const auto& allids = track->lhcbIDs();
    std::copy_if(allids.begin(), allids.end(),
	std::back_inserter(ids), [] (const LHCb::LHCbID id) {
       	return id.isIT() || id.isOT(); });
    //== Check for clone, i.e. similar list of Ids.
    // use the X hits to quickly exclude non-overlapping tracks
    PatSeedTrack::XHitFingerPrint bf;
    for (auto id: ids) {
      int lay = id.isIT() ? id.stID().layer() : id.otID().layer();
      lay %= 4;
      // add x hits to bf
      if (0 == lay || 3 == lay) bf.insert(id);
    }
    const auto& state = track->closestState(m_zReference);
    bool match = false, matchparam = false;
    // loop over output tracks to compare with current input track
    for (auto* track2: outputTracks) {
      if (track2->checkFlag(LHCb::Track::Invalid)) continue;
      // catch obvious clones
      if ((bf & bfs[track2]).empty()) {
	// tracks have no hit overlap, check track parameters to be sure
	const auto& state2 = track2->closestState(m_zReference);
	// if the tracks are too far apart, they can not be clones
	if (m_forwardCloneMaxXDist < std::abs(state.x() - state2.x())) continue;
	if (m_forwardCloneMaxYDist < std::abs(state.y() - state2.y())) continue;
	if (m_forwardCloneMaxTXDist < std::abs(state.tx() - state2.tx())) continue;
	// (no separate cut on ty due to strong correlation of y and ty)
	matchparam = true;
      }
      // tracks seem to be clones, so compare hit lists in detail
      const auto& ids2 = track2->lhcbIDs();
      const double maxCommon = m_forwardCloneMaxShared *
        double(std::min(ids.size(), ids2.size()));
      const double maxMissed = (1. - m_forwardCloneMaxShared) *
        double(std::min(ids.size(), ids2.size()));
      unsigned nCommon = 0, nMissed = 0;
      for (const LHCb::LHCbID id: ids) {
        if (ids2.end() == std::find(ids2.begin(), ids2.end(), id)) {
          // if too many missed, abort the comparison.
          if (++nMissed > maxMissed) break;
        } else {
          // if too many in common already, stop early as well
          if (++nCommon > maxCommon) {
            match = true;
            break;
          }
        }
      } // loop over ids
      // did we find a clone in terms of hits, or are the two tracks merely
      // segments with very few shared hits?
      if (match) {
        // ids and ids2 have more than m_maxUsedFractPerRegion common hits
        // if we already found the longer one of both, we keep it
        if (ids2.size() >= ids.size()) {
	  break;
	}
        // otherwise, we reset the flag stating that we found a clone and
        // erase the short version from our output (and add the longer
        // version to the output below)
        match = false;
	// mark that track as clone
	track2->setFlag(LHCb::Track::Invalid, true);
        break;
      } else if (m_forwardCloneMergeSeg && matchparam) {
        // id lists are not similar but track parameters are, so just combine
        // hits from both tracks
        match = true;
        for (auto id: ids) {
	  if (ids2.end() == std::find(ids2.begin(), ids2.end(), id)) {
	    track2->addToLhcbIDs(id);
	    int lay = id.isIT() ? id.stID().layer() : id.otID().layer();
	    lay %= 4;
	    if (0 == lay || 3 == lay) {
	      // add x hits to bfs[track2]
	      bfs[track2].insert(id);
	    }
	  }
        }
        track2->addToAncestors(track);
        break;
      }
    } // loop over output tracks
    // if the track in inputTracks matched one already found, we skip it
    if (match) continue;

    // build a track from the T station part and store it in outputTracks
    LHCb::Track* out = new LHCb::Track(LHCb::Track::PatSeeding,
       	LHCb::Track::Ttrack, LHCb::Track::PatRecIDs);
    out->setLhcbIDs( ids );
    out->addToAncestors( track );
    for ( unsigned i = 0; i < m_zOutputs.size(); ++i )
      out->addToStates( track->closestState(m_zOutputs[i]) );
    outputTracks.push_back(out);
    bfs[out] = bf;
  } // loop over input tracks
  // weed out the clones among the forward track T station segments
  std::transform(std::begin(outputTracks), std::end(outputTracks),
      std::begin(outputTracks),
      [] (LHCb::Track* tr) {
        bool invalid = tr->checkFlag(LHCb::Track::Invalid);
        if (invalid) delete tr; // free invalid tracks
        return invalid ? nullptr : tr; // and mark their "slots" with nullptr
      });
  outputTracks.erase(std::remove(std::begin(outputTracks),
	std::end(outputTracks), nullptr), std::end(outputTracks));
  if ( m_measureTime ) m_timer->stop(m_timeReuseTracks);
}

//=============================================================================
StatusCode PatSeedingTool::tracksFromTrack(const LHCb::Track& seed, 
                                           std::vector<LHCb::Track*>& tracks ){ 
  // now that we have the possibility to use the state inside this algorithm,
  // we use the state closest to the middle of the T stations available on
  // the track
  // - Manuel 01-28-2008
  return performTracking(tracks, &(seed.closestState(StateParameters::ZMidT)));
}

//=============================================================================
StatusCode PatSeedingTool::performTracking( LHCb::Tracks* outputTracks,
                                            const LHCb::State* state)
{ 
  std::vector<LHCb::Track*> outvec;

  StatusCode sc = performTracking(outvec, state);

  if (sc.isSuccess() && !outvec.empty()) {
    // copy tracks from outvec to output container
    outputTracks->reserve(outvec.size());
    for( LHCb::Track* track: outvec )
      outputTracks->insert(track);
  }

  return sc;
}

//======================================================================
template<class RANGE>
bool PatSeedingTool::isIsolated(typename RANGE::const_iterator it,
                                const RANGE& range) const
{
  double isolationRange = m_OTIsolation;
  if ( (*it)->hit()->sthit() )
    isolationRange = m_ITIsolation;
  // create a window around *it which contains all hits within the
  // window from x - isolationRange to x + isolationRange
  double x = (*it)->hit()->xAtYEq0();
  auto iLo = it, iHi = it + 1;
  while ( range.begin() != iLo &&
          std::abs((*(iLo - 1))->hit()->xAtYEq0() - x) < isolationRange )
    --iLo;
  if ( std::abs((*iLo)->hit()->xAtYEq0() - x) > isolationRange )
    ++iLo;
  while ( range.end() != iHi &&
          std::abs((*iHi)->hit()->xAtYEq0() - x) < isolationRange )
    ++iHi;
  // if there is nothing in there except for *it, we're done
  if ( 1 == iHi - iLo )
    return true;
  if ( (*it)->hit()->othit() ) {
    // for the OT, it's simple: two hits per layer maximum,
    // and these two must be flagged as neighbouring
    // (i.e. must be due to particle passage through the two 
    // staggered monolayers)
    if (2 < iHi - iLo )
      return false;
    if ( it == iLo ) ++it;
    // the framework flags such neighbouring hits, so we just
    // use the flags
    if ( !(*iLo)->hasNext() || !(*it)->hasPrevious() )
      return false;
    return true;
  } else {
    // recognise ladder-ladder overlaps - this will give 2 hits
    // box-box overlaps will not show up since the hits are in
    // different regions
    if ( 2 < iHi - iLo )
      return false;
    if ( it == iLo ) ++it;
    LHCb::STChannelID id1 = (*iLo)->hit()->sthit()->cluster().channelID();
    LHCb::STChannelID id2 = (*it)->hit()->sthit()->cluster().channelID();
    int idDiff = id1.uniqueSector() - id2.uniqueSector();
    // test for neighbouring ladders
    if ( 1 != abs(idDiff) )
      return false;
    return true;
  }
  // we should never arrive here
}

// vim:shiftwidth=2:tw=78
