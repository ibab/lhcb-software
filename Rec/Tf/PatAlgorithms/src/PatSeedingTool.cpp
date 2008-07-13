// $Id: PatSeedingTool.cpp,v 1.18 2008-07-13 22:05:48 mschille Exp $
// Include files

#include <cmath>
#include <algorithm>
#include <functional>

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "OTDet/DeOTDetector.h"
// from TrackEvent
#include "Event/StateParameters.h"
// from boost
#include <boost/assign/list_of.hpp>
#include <boost/array.hpp>
#include <boost/foreach.hpp>
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
DECLARE_TOOL_FACTORY( PatSeedingTool );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatSeedingTool::PatSeedingTool(  const std::string& type,
				 const std::string& name,
				 const IInterface* parent)
  : GaudiTool ( type, name , parent )
{

  declareInterface<IPatSeedingTool>(this);
  declareInterface<ITracksFromTrack>(this);

  //------------------------------------------------------------------------
  // hit level stuff
  //------------------------------------------------------------------------
  declareProperty( "reusePatSeeding",		m_reusePatSeeding	= true );
  declareProperty( "UseForward",		m_useForward		= false       );
  declareProperty( "InputTracksName",		m_inputTracksName	= LHCb::TrackLocation::Forward  );
  declareProperty( "DriftRadiusRange",		m_driftRadiusRange	=
		  boost::assign::list_of(-0.6 * Gaudi::Units::mm)(2.8 * Gaudi::Units::mm) );

  //------------------------------------------------------------------------
  // track model
  //------------------------------------------------------------------------
  declareProperty( "dRatio",			m_dRatio		= -3.2265e-4);
  declareProperty( "InitialArrow",		m_initialArrow		= 4.25307e-09);
  declareProperty( "MomentumScale",		m_momentumScale		= 44.1416);
  declareProperty( "zReference",		m_zReference		=  StateParameters::ZMidT);

  //------------------------------------------------------------------------
  // options for track search in xz projection
  //------------------------------------------------------------------------
  // curvature cut
  declareProperty( "MinMomentum",		m_minMomentum		=  500. * Gaudi::Units::MeV );
  declareProperty( "CurveTol",			m_curveTol		=    5. * Gaudi::Units::mm );
  // center of magnet compatibility
  declareProperty( "zMagnet",			m_zMagnet		= 5383.17 * Gaudi::Units::mm );
  declareProperty( "xMagTol",			m_xMagTol		= 2000. * Gaudi::Units::mm );
  // window to collect hits from
  declareProperty( "TolCollectOT",		m_tolCollectOT		=    3. * Gaudi::Units::mm );
  declareProperty( "TolCollectIT",		m_tolCollectIT		=    .3 * Gaudi::Units::mm );
  // clone killing in xz projection
  declareProperty( "MinXPlanes",		m_minXPlanes		=    5                     );
  declareProperty( "CloneMaxXDist",		m_cloneMaxXDist		=   10. * Gaudi::Units::mm );
  declareProperty( "CommonXFraction",		m_commonXFraction	=    0.7                   );
  declareProperty( "QualityWeights",		m_qualityWeights	= boost::assign::list_of(1.0)(-0.2) );

  //------------------------------------------------------------------------
  // options for track search in stereo layers
  //------------------------------------------------------------------------
  // projection plane for stereo hits
  declareProperty( "zForYMatch",		m_zForYMatch		= 9000. * Gaudi::Units::mm );
  // spread of run of stereo hits in projection plane
  declareProperty( "MaxRangeOT",		m_maxRangeOT		=  150. * Gaudi::Units::mm );
  declareProperty( "MaxRangeIT",		m_maxRangeIT		=   50. * Gaudi::Units::mm );
  // pointing criterium in y
  declareProperty( "yCorrection",		m_yCorrection		= 4.73385e-15);
  declareProperty( "MaxYAtOrigin",		m_maxYAtOrigin		=  400. * Gaudi::Units::mm );
  declareProperty( "MaxYAtOriginLowQual",	m_maxYAtOriginLowQual	= 1500. * Gaudi::Units::mm );

  //------------------------------------------------------------------------
  // track search starting with quartett in IT
  //------------------------------------------------------------------------
  declareProperty( "TolExtrapolate",		m_tolExtrapolate	=    4. * Gaudi::Units::mm );

  //------------------------------------------------------------------------
  // internal fit, outlier removal, minimum number of planes
  //------------------------------------------------------------------------
  declareProperty( "MaxChi2HitOT",		m_maxChi2HitOT		=   30.                    );
  declareProperty( "MaxChi2HitIT",		m_maxChi2HitIT		=   10.                    );
  declareProperty( "MaxTrackChi2",		m_maxTrackChi2     	=   20.                    );
  declareProperty( "MaxFinalChi2",		m_maxFinalChi2     	=   15.                    );
  declareProperty( "MaxFinalTrackChi2",		m_maxFinalTrackChi2	=   10.                    );
  declareProperty( "MaxTrackChi2LowMult",	m_maxTrackChi2LowMult	=    5.			   );
  declareProperty( "MinTotalPlanes",		m_minTotalPlanes	=   10                     );
  declareProperty( "MaxMisses",			m_maxMisses		=    1			   );
  declareProperty( "OTNHitsLowThresh",		m_otNHitsLowThresh	=   17			   );

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
  declareProperty( "FastMomentumToolName",	m_fastMomentumToolName	= "FastMomentumEstimate" );
  declareProperty( "ZOutput",			m_zOutputs		=
		  boost::assign::list_of(StateParameters::ZBegT)(StateParameters::ZMidT)(StateParameters::ZEndT));

  //------------------------------------------------------------------------
  // options concerning copying the T station part of forward tracks
  //------------------------------------------------------------------------
  // should we copy T station part of forward tracks (default: no)
  declareProperty( "UseForwardTracks",		m_useForwardTracks	= false       );
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
}
//=============================================================================
// Destructor
//=============================================================================
PatSeedingTool::~PatSeedingTool() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode PatSeedingTool::initialize() {
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiTool

  m_tHitManager    = tool<Tf::TStationHitManager<PatForwardHit> >("PatTStationHitManager");

  m_seedTool = tool<PatSeedTool>( "PatSeedTool" );
  m_fastMomentumTool = tool<IFastMomentumEstimate>( m_fastMomentumToolName );

  //== Max impact: first term is due to arrow ->curvature by / (mm)^2 then momentum to impact at z=0
  //== second term is decay of Ks: 210 MeV Pt, 2000 mm decay distance.
  // protect against division by near-zero
  if (1e-42 > fabs(m_minMomentum) || 1e-42 > fabs(m_momentumScale) ||
      1e-42 > fabs(m_initialArrow)) m_maxImpact = HUGE_VAL;
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
	    "lower limit <= upper limit" << endreq;
    std::swap(m_driftRadiusRange[0], m_driftRadiusRange[1]);
  }

  return StatusCode::SUCCESS;
}

StatusCode PatSeedingTool::finalize() {
  if (m_measureTime) {
    info() << name() << " finalizing now..." << endreq;

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
  const double driftRadLow = m_driftRadiusRange[0];
  const double driftRadHigh = m_driftRadiusRange[1];
  HitRange range = hits();
  BOOST_FOREACH( PatFwdHit* hit, range ) {
    hit->setSelected( true );
    hit->setIgnored( false );
    if ( m_useForward )
      if ( hit->hit()->testStatus(Tf::HitBase::UsedByPatForward) ) {
	hit->setIsUsed(true);
	continue;
      }
    if ( !m_reusePatSeeding )
      if ( hit->hit()->testStatus(Tf::HitBase::UsedByPatSeeding) ) {
	hit->setIsUsed(true);
	continue;
      }
    const Tf::OTHit* otHit = hit->hit()->othit();
    if ( otHit ) {
      const double driftRad = otHit->untruncatedDriftDistance();
      if ( driftRad > driftRadHigh || driftRad < driftRadLow ) {
	hit->setIsUsed( true );
	continue;
      }
    }
    hit->setIsUsed(false);
  }

  // update region cache: we need some information not directly available
  // from the Tf::-framework
  for (unsigned sta = 0; sta < m_nSta; ++sta) {
    for (unsigned lay = 0; lay < m_nLay; ++lay) {
      for (unsigned reg = 0; reg < m_nReg; ++reg) {
	unsigned idx = (sta * m_nLay + lay) * m_nReg + reg;
	m_RCtanT[idx] = region(sta,lay,reg)->sinT() /
	  region(sta,lay,reg)->cosT();
	// because the IT is at small abs(y), we can neglect dy/dz
	// for the OT, we use the fact that the division of the OT into upper
	// and lower half is an artificial one
	// we can't use the naive dz/dy=(zmax - zmin)/(ymax - ymin) because
	// of the finite thickness of OT modules
	m_RCdzdy[idx] = isRegionIT(reg) ? 0.0 :
	  ((region(sta,lay,reg)->zmin() - region(sta,lay,reg^1)->zmin()) /
	   (region(sta,lay,reg)->ymin() - region(sta,lay,reg^1)->ymin()));
	// return zmid at y = 0 (instead of y = ymid)
	// no correction for IT, see above
	m_RCz0[idx] = region(sta,lay,reg)->z() - region(sta,lay,reg)->y() *
	  m_RCdzdy[idx];
      }
    }
  }

  // if someone wants a printout, do so now
  if ( msgLevel( MSG::VERBOSE ) ) {
    HitRange::const_iterator itH;
    for (unsigned reg = 0; reg < m_nReg; ++reg) {
      for (unsigned sta = 0; sta < m_nSta; ++sta) {
	for (unsigned lay = 0; lay < m_nLay; ++lay) {
	  HitRange range = hits(sta, lay, reg);
	  BOOST_FOREACH( const PatFwdHit* hit, range ) {
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
  // make sure there's space in the output
  outputTracks.reserve(outputTracks.size() + finalSelection.size());
  //== Sort tracks according to their chi2
  if ( finalSelection.size() > 1 )
    std::sort( finalSelection.begin(), finalSelection.end(),
		    PatSeedTrack::decreasingQuality() );

  bool debug = msgLevel( MSG::DEBUG );
  //== Keep only those with less than maxUsedFraction used clusters
  BOOST_FOREACH( const PatSeedTrack* track, finalSelection) {
    // this is a funny way to implement the cut on the fraction of used
    // hits, but the advantage is that we stop counting early, if the
    // track does not pass the cut
    int maxUsed = int(std::ceil(maxUsedFraction * track->nCoords()));
    BOOST_FOREACH( const PatFwdHit* hit, track->coords() )
      if (hit->isUsed() && (0 > --maxUsed)) break;
    if ( 0 > maxUsed ) continue;

    if ( debug ) {
      const double p = 1. / (m_momentumScale * track->curvature());
      info() << "** Store track, estimated P " << p << " nCoord "
	<< track->nCoords() << " chi2 " << track->chi2() << endreq;
      BOOST_FOREACH( const PatFwdHit* hit, track->coords() )
	debugFwdHit( hit, info() );
    }
    storeTrack( *track, outputTracks );
  }
}

StatusCode PatSeedingTool::performTracking(
		std::vector<LHCb::Track*>& outputTracks,
		const LHCb::State* state )
{
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
  // skip in trigger context if state indicates a track with fabs(y)>70cm at T1
  if ( 0 == state || 0.7e3 >
      fabs(state->y()+state->ty()*(StateParameters::ZBegT-state->z()))) {
    // first pass to pick up tracks going from IT to OT
    collectITOT(pool, finalSelection, outputTracks, state);
    // second pass to pick up hits going through several IT/OT regions
    collectITOT(pool, finalSelection, outputTracks, state, true);
  }

  // collect OT tracks for high fabs(y) which are almost straight
  // skip in trigger context, unless state indicates fabs(y) > 1m at T1
  if ( 0 == state || 1e3 <
      fabs(state->y()+state->ty()*(StateParameters::ZBegT-state->z()))) {
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
  PatFwdHits::const_iterator itH;
  PatFwdHits stereo;
  stereo.reserve(256);

  if (m_measureTime) m_timer->start(m_timePerRegion);
  // for cosmics, only seed in the OT (but collect in the IT as well)
  // for ordinary tracks seed in all regions
  for (unsigned reg = 0; reg < (m_cosmics?m_nOTReg:m_nReg); ++reg) {
    if (OTonly && !isRegionOT(reg)) continue;
    if (0 != state) {
      // in trigger context, make sure we only run over regions in which the
      // track can have enough hits according to the state given
      int nPlanesMax = m_nSta * m_nLay;
      for (unsigned sta = 0; m_nSta > sta; ++sta) {
	for (unsigned lay = 0; m_nLay > lay; ++lay) {
	  double dz = region(sta, lay, reg)->z() - state->z();
	  double ytol = std::sqrt(state->covariance()(1, 1)
	      + 2.0 * dz * state->covariance()(1, 3)
	      + dz * dz * state->covariance()(3,3));
	  double y = state->y() + state->ty() * dz;
	  if ( region(sta,lay,reg)->ymin() > y + ytol  ||
	      region(sta,lay,reg)->ymax() < y - ytol )
	    if (--nPlanesMax < m_minTotalPlanes) break;
	  double xtol = std::sqrt(state->covariance()(0, 0)
	      + 2.0 * dz * state->covariance()(0, 2)
	      + dz * dz * state->covariance()(2,2));
	  double x = state->x() + state->tx() * dz;
	  if ( region(sta,lay,reg)->xmin() > x + xtol  ||
	      region(sta,lay,reg)->xmax() < x - xtol )
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
      // if we reconstruct cosmics, we need to extend the x search into
      // more than one region (cosmics almost certainly migrate from one
      // region to another)
      // regmask encodes if the second and third hit used in findXCandidates
      // are to be searched in the same or a different OT region
      for ( unsigned regmask = m_cosmics?4:1; regmask--; ) {
	// in the OT, we only do two combinations
	if (isRegionOT(reg) && 2 <= nlay) break;
	pool.clear();

	if ( m_measureTime ) m_timer->start( m_timeX );

	findXCandidates( lay, reg ^ (regmask << 3), pool, state );

	if ( msgLevel( MSG::DEBUG ) )
	  debug() << "Found " << pool.size()
	    << " x candidates" << endreq;

	if ( m_measureTime ) {
	  m_timer->stop( m_timeX );
	  if ( pool.empty() ) continue;
	  m_timer->start( m_timeStereo );
	}
	if ( pool.empty() ) continue;

	lowQualTracks.reserve(lowQualTracks.size() + pool.size());
	finalSelection.clear();
	finalSelection.reserve(pool.size());
	BOOST_FOREACH( PatSeedTrack& track, pool ) {
	  // skip tracks for which x part is already invalid
	  if ( !track.valid() ) continue;
	  // unless y turns out to be ok, default to not valid
	  track.setValid( false );

	  m_printing = msgLevel( MSG::DEBUG );

	  if ( m_printing ) {
	    // if we're debugging, we can be slow in determining where
	    // othertrack is in pool
	    int nTrack = 0;
	    BOOST_FOREACH( const PatSeedTrack& tmp, pool ) {
	      if (&tmp == &track) break;
	      ++nTrack;
	    }
	    info() << "--- X candidate " << nTrack << endreq;
	    BOOST_FOREACH( const PatFwdHit* hit, track.coords() ) {
	      info() << format( "dist %7.3f ", track.distance( hit ) );
	      debugFwdHit( hit, info() );
	    }
	  }

	  //== Collect stereo hits
	  collectStereoHits( track, reg, stereo, state );

	  int minNbPlanes = m_minTotalPlanes - track.nPlanes();

	  if ( m_printing ) {
	    info() << "--- Initial list of stereo hits " << endreq;
	    BOOST_FOREACH( const PatFwdHit* hit, stereo ) {
	      if ( hit->isIgnored() ) continue;
	      debugFwdHit( hit, info() );
	    }
	  }
	  if ( stereo.size() < unsigned(minNbPlanes) )  continue;

	  bool ok = (m_cosmics ?
	      findBestRangeCosmics( reg, minNbPlanes, stereo ) :
	      findBestRange( reg, minNbPlanes, stereo ) );

	  if ( !ok ) continue;

	  if ( m_printing ) {
	    info() << "--- After filtering of stereo hits " << endreq;
	    BOOST_FOREACH( const PatFwdHit* hit, stereo ) {
	      if ( hit->isIgnored() ) continue;
	      debugFwdHit( hit, info() );
	    }
	  }

	  //== restore real coordinate
	  double y0, sl;
	  ok = fitLineInY( stereo, y0, sl );

	  if ( !ok ) {
	    if ( m_printing ) info() << "  -- fitLineInY failed" << endreq;
	    continue;
	  }
	  if ( 0 != state && fabs(sl - state->ty()) > std::sqrt(state->errTy2()) ) {
	    if ( m_printing ) info() << "  -- ty not compatible with state given" << endreq;
	    continue;
	  }
	  double yAtOrigin = y0;
	  double yCorr     = sl * sl * track.curvature() * track.curvature() / m_yCorrection;
	  if ( sl > 0 ) yAtOrigin = y0 - yCorr;
	  else yAtOrigin = y0 + yCorr;

	  if ( m_printing ) {
	    info() << format( "  Y0 %10.1f corr %10.1f yAtOrigin%10.1f  sl%10.7f  curv*E6 %8.4f",
		y0, yCorr, yAtOrigin, sl, 1.e6*track.curvature() ) << endreq;
	  }

	  if ( m_maxYAtOrigin < fabs( yAtOrigin ) ) {
	    if ( m_printing ) info() << "  -- yAtOrigin too big : " << yAtOrigin << endreq;
	    // we might want to keep OT tracks for a second pass
	    if (!isRegionOT(reg)) continue;
	    // unless they point nowhere near the vertex
	    if (m_maxYAtOriginLowQual < fabs(yAtOrigin)) continue;
	    for ( itH = stereo.begin(); stereo.end() != itH; ++itH )
	      if ( !(*itH)->isIgnored() ) track.addCoord( *itH );
	    track.setYParams( y0, sl );
	    lowQualTracks.push_back(track);
	    continue;  // non pointing track -> garbage
	  }

	  if ( m_printing ) {
	    info() << "Fitted line y0 " << y0 << " sl " << sl << endreq;
	    BOOST_FOREACH( const PatFwdHit* hit, stereo ) {
	      if ( hit->isIgnored() ) continue;
	      info() << format( "dist %7.3f ",  hit->projection() - y0 - hit->z() * sl );
	      debugFwdHit( hit, info() );
	    }
	  }
	  //== Check for enough OT if not near boundary
	  if ( isRegionOT(reg) ) {
	    double y = y0 + m_zForYMatch * sl;
	    if ( m_centralYOT < fabs(y) ) {
	      boost::array<int, 3> nInStation = { 0, 0, 0 };
	      for ( itH = stereo.begin(); stereo.end() != itH; ++itH ) {
		if ( (*itH)->isIgnored() ) continue;
		nInStation[ (*itH)->hit()->station() ]++;
	      }
	      if ( 2 > nInStation[0] || 2 > nInStation[1] || 2 > nInStation[2] ){
		if ( m_printing )
		  info() << "Not enough OT stereo in some station : "
		    << nInStation[0] << " "
		    << nInStation[1] << " "
		    << nInStation[2] << endreq;
		continue;
	      }
	    }
	  }

	  BOOST_FOREACH( PatFwdHit* hit, stereo ) {
	    if ( hit->isIgnored() ) continue;
	    track.addCoord( hit );
	  }
	  
	  if (m_cosmics) {
	    // for cosmics, we were more permissive when collecting hits
	    // as we may have picked up some way off in y, discard them now
	    // (track parameters in y are known by now)
	    for (PatFwdHits::iterator itH = track.coordBegin();
		track.coordEnd() != itH; ++itH) {
	      const double yMin = (*itH)->hit()->yMin() - m_maxRangeOT;
	      const double yMax = (*itH)->hit()->yMax() + m_maxRangeOT;
	      const double yHit = y0 + sl * (*itH)->z();
	      if ( yHit < yMin || yHit > yMax) track.removeCoord(itH);;
	    }
	  }

	  if ( isRegionOT(reg) &&
	      m_otNHitsLowThresh > track.nCoords() &&
	      2 * (track.nCoords()-track.nPlanes()+1) < track.nbOnSide() ) {
	    if ( m_printing ) info()  << "Too many on side " << track.nbOnSide() << " with only "
	      << track.nCoords() << " coordinates" << endreq;
	    continue;
	  }

	  track.setYParams( y0, sl );

	  track.sort();
	  if ( m_printing ) info() << "-- Before fit, nPlanes = " << track.nPlanes() << endreq;
	  ok = m_seedTool->fitTrack( track, m_maxFinalChi2, m_minTotalPlanes, false, m_printing );

	  if ( m_printing ) info() << "After fit status "<< ok << " chi2 " << track.chi2()
	    << ", nPlanes = " << track.nPlanes() << endreq;
	  if ( isRegionOT(reg) &&   // OT
	      m_otNHitsLowThresh > track.nCoords() &&  // short track -> not too many neighbours
	      2 * (track.nCoords() - track.nPlanes() +1 ) < track.nbOnSide() ) {
	    if ( m_printing ) {
	      info()  << "Too many on side " << track.nbOnSide() << " with only "
		<< track.nCoords() << " coordinates" << endreq;
	      BOOST_FOREACH( const PatFwdHit* hit, track.coords() )
		debugFwdHit( hit, info() );
	    }
	    continue;
	  }

	  if ( m_maxFinalTrackChi2 < track.chi2() || !ok ) {
	    if ( m_printing ) info() << "Chi2 too big" << endreq;
	    continue;
	  }

	  // if a track makes it to this point, it's valid
	  track.setValid( true );
	  finalSelection.push_back(&track);
	} // end loop over pool
	if ( m_measureTime ) m_timer->stop( m_timeStereo );

	//== Keep only best with >50% unused clusters (in the current pass...)
	killClonesAndStore(finalSelection, outputTracks, m_maxUsedFractPerRegion);
      } // regmask
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
  HitRange::const_iterator itH;
  if ( m_measureTime ) m_timer->start( m_timeItOt );

  if ( m_printing ) info() << "=== remaining IT coord ====" << endreq;
  m_tolCollect = m_tolCollectIT;

  for ( unsigned reg = m_nOTReg; reg < m_nReg; ++reg ) {
    // form track segments from unused hits in all IT regions
    finalSelection.clear();
    pool.clear();
    for ( unsigned sta = 0; sta < m_nSta; ++sta ) {
      double zScaling = ( regionZ0(sta,3,reg) - regionZ0(sta,0,reg)) /
	( regionZ0(sta, 0, reg) - m_zMagnet );

      double x0Min = -1e42, x0Max = 1e42;
      if (0 != state)
	updateSearchWindowX(regionZ0(sta,0,reg), x0Min, x0Max, state);
      HitRange rangeH0 = (0 == state) ? rangeH0 = hits(sta,0,reg) :
	hitsWithMinX( x0Min, sta, 0, reg);

      for ( HitRange::const_iterator itH0 = rangeH0.begin();
	  rangeH0.end() != itH0; ++itH0 ) {
	PatFwdHit* hit0 = *itH0;
	if ( 0 != state ) {
	  if ( x0Max < hit0->hit()->xAtYEq0() ) break;
	  if ( x0Min > hit0->hit()->xAtYEq0() ) continue;
	}
        if ( hit0->isUsed() ) continue;
	if (m_enforceIsolation && !isIsolated(itH0, rangeH0)) continue;

        m_printing = msgLevel( MSG::VERBOSE );
	if ( m_printing ) info() << "*IT* hit sta " << sta << " lay 0 x = " <<
	  hit0->hit()->xAtYEq0() << endreq;

	// open search window into layer 3
	double x3Min = hit0->hit()->xAtYEq0() +
	  (hit0->hit()->xAtYEq0() - m_xMagTol) * zScaling;
	double x3Max = hit0->hit()->xAtYEq0() +
	  (hit0->hit()->xAtYEq0() + m_xMagTol) * zScaling;
	// restrict search window if we have a state
	if ( 0 != state )
	  updateSearchWindowX(regionZ0(sta,3,reg), x3Min, x3Max, state);

        HitRange rangeH3 = hitsWithMinX( x3Min,sta,3,reg );
        for ( HitRange::const_iterator itH3 = rangeH3.begin();
	    rangeH3.end() != itH3; ++itH3 ) {
          PatFwdHit* hit3 = *itH3;
          if ( hit3->hit()->xAtYEq0() > x3Max ) break;
          if ( hit3->isUsed() ) continue;
          if ( hit3->hit()->xAtYEq0() < x3Min ) continue;
	  if ( m_enforceIsolation && !isIsolated(itH3, rangeH3) ) continue;
          double z0 = hit0->hit()->zAtYEq0();
          double x0 = hit0->hit()->xAtYEq0();
          double sl = (hit3->hit()->xAtYEq0() - x0) /
	    (hit3->hit()->zAtYEq0() - z0);
	  // check against slope from state if available
	  if (state && fabs(sl - state->tx()) > std::sqrt(state->errTx2()))
	    continue;

	  // cache tangens of stereo angle
	  double tanT1 = regionTanT(sta,1,reg);
	  double tanT2 = regionTanT(sta,2,reg);

	  // open search window into first stereo layer
	  double x1Min = region(sta,1,reg)->ymin() * tanT1;
	  double x1Max = region(sta,1,reg)->ymax() * tanT1;
	  if ( x1Min > x1Max ) std::swap(x1Min, x1Max);
	  double xref = (x0 + ( regionZ0(sta,1,reg) - z0 ) * sl);
	  x1Min += xref - m_tolCollect;
	  x1Max += xref + m_tolCollect;
	  // restrict search window if we have a state
	  if (0 != state) updateSearchWindowY(regionZ0(sta,1,reg),
	      xref, m_tolCollect, tanT1, x1Min, x1Max, state);

	  if ( m_printing ) info() << "     hit sta " << sta << " lay 3 x = "
	    << hit3->hit()->xAtYEq0() << " x1Min " << x1Min << " x1Max "
	      << x1Max << endreq;

          HitRange rangeH1 = hitsWithMinX( x1Min, sta, 1,reg );
          for ( HitRange::const_iterator itH1 = rangeH1.begin();
	      rangeH1.end() != itH1; ++itH1 ) {
            PatFwdHit* hit1 = *itH1;
            if ( x1Max < hit1->hit()->xAtYEq0() ) break;
            if ( hit1->isUsed() ) continue;
            if ( x1Min > hit1->hit()->xAtYEq0() ) continue;
	    if ( m_enforceIsolation && !isIsolated(itH1, rangeH1) ) continue;

	    // open search window into second stereo layer
            double y = ((x0 + (hit1->hit()->zAtYEq0() - z0) * sl) -
		hit1->hit()->xAtYEq0()) / hit1->hit()->dxDy();
	    // check slope in y if we have a state
	    if (state && fabs(y / hit1->hit()->zAtYEq0()) >
		std::sqrt(state->errTy2()))
	      continue;
	    double z2 = regionZ0(sta, 2, reg);
	    // assume straight line through hit1 and origin to derive y at
	    // second layer
            double pred = (x0 + (z2 - z0) * sl) +
	      z2 / hit1->hit()->zAtYEq0() * y * tanT2;
            double x2Min = pred - 1.5 * m_tolCollect;
            double x2Max = pred + 1.5 * m_tolCollect;

            if ( m_printing ) info() << "     hit sta " << sta << " lay 1 x = "
	      << hit1->hit()->xAtYEq0() << " x2Min " << x2Min << " x2Max "
		<< x2Max << endreq;

            HitRange rangeH2 = hitsWithMinX( x2Min, sta, 2, reg );
            for ( HitRange::const_iterator itH2 = rangeH2.begin();
		rangeH2.end() != itH2; ++itH2 ) {
              PatFwdHit* hit2 = *itH2;
              if ( x2Max < hit2->hit()->xAtYEq0() ) break;
              if ( hit2->isUsed() ) continue;
              if ( x2Min > hit2->hit()->xAtYEq0() ) continue;
	      if ( m_enforceIsolation && !isIsolated(itH2, rangeH2) ) continue;

	      // restore coordinates
	      restoreCoordinate(hit0);
	      restoreCoordinate(hit1);
	      restoreCoordinate(hit2);
	      restoreCoordinate(hit3);

	      if ( m_printing )
                info() << "     hit sta " << sta << " lay 2 x = " << hit2->x() << endreq;

	      // ok, we have a seed
              pool.push_back(
		  PatSeedTrack( hit0, hit1, hit2, hit3,
		    m_zReference, m_dRatio, m_initialArrow ) );
	      PatSeedTrack& track = pool.back();
              if ( 3 > track.nbHighThreshold() ) {
                if ( m_printing ) info() << "   less than 3 with high threshold" << endreq;
		// unfortunately not good enough, so pop it off again
		pool.pop_back();
                continue;
              }

              // Add neighbouring hits
	      addNeighbour(track, hit0, itH0, rangeH0);
	      addNeighbour(track, hit1, itH1, rangeH1);
	      addNeighbour(track, hit2, itH2, rangeH2);
	      addNeighbour(track, hit3, itH3, rangeH3);

              if ( m_printing ) {
                info() << "--- IT point -- " << endreq;
		BOOST_FOREACH( const PatFwdHit* hit, track.coords() )
                  debugFwdHit( hit, info() );
              }
            }
          }
        }
      }
    }

    // follow these track segments into OT and IT and collect more hits there
    if ( m_printing ) info() << "===== Candidates Type " << reg << endreq;
    finalSelection.reserve(pool.size());
    for ( std::vector<PatSeedTrack>::iterator itT = pool.begin();
          pool.end() != itT; ++itT ) {
      PatSeedTrack& track = *itT;
      track.setValid(false);
      bool centralYIT = false;
      bool fitOK = false;
      m_printing = msgLevel( MSG::DEBUG );
     
      if ( m_printing ) info() << "   candidate " << itT - pool.begin()
                               << " y(0) = " << track.yAtZ( 0. )
                               << " ySlope " << track.ySlope( 0. )
                               << endreq;
      PatFwdHit* firstHit = *track.coordBegin();

      unsigned ITSta = firstHit->hit()->station();
      unsigned nbSta = 1;
      unsigned otTypMin = (track.yAtZ(firstHit->z()) > 0.0) ? 1 : 0;
      unsigned otTypMax = otTypMin + 1;
      if (doOverlaps) otTypMin = 0, otTypMax = m_nOTReg;

      //== Some data in OT ??
      for ( unsigned sta = 0; 3 > sta; ++sta ) {
	// skip station used to produce the seed (we used all hits there)
        if ( sta == ITSta ) continue;
        int nbPlanes = track.nPlanes();

        for ( unsigned lay = 0 ; 4 > lay ; ++lay ) {
	  for (unsigned otTyp = otTypMin; otTypMax > otTyp; ++otTyp) {
	    // skip layers which are too far away in y
	    if ( track.yAtZ( regionZ0(sta,lay,otTyp) ) <
		region(sta,lay,otTyp)->ymin() - m_tolExtrapolate ||
		track.yAtZ( regionZ0(sta,lay,otTyp) ) >
		region(sta,lay,otTyp)->ymax() + m_tolExtrapolate    ) continue;

	    // ok, check if we can pick up more hits
	    double tanT = regionTanT(sta,lay,otTyp);
	    double zPred = regionZ0(sta,lay,otTyp) +
	      regionDzDy(sta,lay,otTyp) * track.yAtZ(regionZ0(sta,lay,otTyp));
	    double xPred = track.xAtZ( zPred ) + track.yAtZ( zPred ) * tanT;
	    if ( m_printing ) {
	      info() << "** Search in sta " << sta << " lay " << lay << " xPred " << xPred << endreq;
	    }

	    HitRange rangeXX = hitsWithMinX( xPred - m_tolExtrapolate - 3.,sta,lay,otTyp );
	    for ( itH = rangeXX.begin(); rangeXX.end() != itH; ++itH ) {
	      if ( (*itH)->isUsed() ) continue;
	      PatFwdHit* hit = *itH;
	      updateHitForTrack( hit, track.yAtZ(hit->z()),0);
	      if ( - m_tolExtrapolate > track.distance( hit ) ) continue;
	      if (   m_tolExtrapolate < track.distance( hit ) ) break;
	      if ( m_enforceIsolation && !isIsolated(itH, rangeXX) ) continue;
	      if ( m_printing ) {
		info() << "Add coord ";
		debugFwdHit( hit, info() );
	      }
	      track.addCoord( hit );
	    }
	  }
	}
        if ( 3 <= track.nPlanes() - nbPlanes  ) {
          nbSta++;
          int minPlanes = 3 * nbSta + 2;
          fitOK = m_seedTool->fitTrack( track, m_maxChi2HitOT, minPlanes, false, m_printing );
          if ( m_printing ) info() << "*** refitted track candidate status " << fitOK << endreq;
        }
      }
      // Try in IT again if needed
      if ( 3 > nbSta ) {
	unsigned minReg = doOverlaps ? (m_nOTReg) : (reg);
	unsigned maxReg = doOverlaps ? (m_nReg) : (reg + 1);
	for ( unsigned sta = 0; 3 > sta; ++sta ) {
	  for ( unsigned testReg = minReg; maxReg > testReg; ++testReg) {
	    if ( sta == ITSta && reg == testReg) continue;
	    int nbAdded = 0;
	    for ( unsigned lay = 0 ; 4 > lay ; ++lay ) {
	      double zPred = regionZ0(sta,lay,testReg);
	      double yPred = track.yAtZ( zPred );
	      if ( m_centralYIT > fabs(yPred) && doOverlaps ) centralYIT = true;
	      if ( yPred < region(sta, lay, testReg)->ymin() - m_tolExtrapolate ||
		  yPred > region(sta, lay, testReg)->ymax() + m_tolExtrapolate    ) continue;
	      double tanT = regionTanT(sta, lay, testReg);
	      double xPred = track.xAtZ( zPred ) + yPred * tanT;
	      if ( xPred < region(sta, lay, testReg)->xmin() - m_tolExtrapolate ||
		  xPred > region(sta, lay, testReg)->xmax() + m_tolExtrapolate    ) continue;
	      if ( m_printing ) {
		info() << "** Search in IT sta " << sta << " lay " << lay << " region " << testReg
		  << " xPred " << xPred << endreq;
	      }

	      HitRange rangeYY = hitsWithMinX( xPred - m_tolExtrapolate, sta, lay, testReg );
	      for ( itH = rangeYY.begin(); rangeYY.end() != itH; ++itH ) {
		if ( (*itH)->isUsed() ) continue;
		PatFwdHit* hit = *itH;
		updateHitForTrack( hit, track.yAtZ(hit->z()), 0);
		if ( - m_tolExtrapolate > track.distance( hit ) ) continue;
		if (   m_tolExtrapolate < track.distance( hit ) ) break;
		if ( m_enforceIsolation && !isIsolated(itH, rangeYY) ) continue;
		if ( m_printing ) {
		  info() << "Add coord ";
		  debugFwdHit( hit, info() );
		}
		track.addCoord( hit );
		++nbAdded;
	      }
	    }
	    if ( 3 < nbAdded ) {
	      nbSta++;
	      int minPlanes = 3 * nbSta + 1;
	      fitOK = m_seedTool->fitTrack( track, m_maxChi2HitOT, minPlanes, false, m_printing );
	    }
	  }
	}
      }

      if ( m_printing ) {
        info() << " --- We have found " << nbSta << " stations, fitOK " <<
		fitOK << "." << endreq;
	BOOST_FOREACH( const PatFwdHit* hit, track.coords() )
          debugFwdHit( hit, info() );
      }

      if ( m_fieldOff ) {
	// cut on track curvature when running without magnetic field
	double dz = 0.5 * ((*track.coordBegin())->z() -
		(*(track.coordBegin() + (track.nCoords() - 1)))->z());
	if ( m_tolCollect / dz / dz < fabs( track.curvature() ) ) {
	  if ( m_printing ) info() << "    magnet off: curvature too high: "
		  << track.curvature() << endreq;
	  continue;
	}
      }

      if ( (3 == nbSta || (centralYIT && 2 == nbSta)) && fitOK ) {
	track.setValid(true);
	finalSelection.push_back(&track);
      }
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
  PatFwdHits stereo;
  PatFwdHits::iterator itH;

  if ( m_measureTime ) m_timer->start( m_timeLowQual );

  finalSelection.clear();
  finalSelection.reserve(lowQualTracks.size());
  stereo.reserve(32);
  for (itS = lowQualTracks.begin(); lowQualTracks.end() != itS; ++itS) {
    PatSeedTrack& track = *itS;
    // default to not valid
    track.setValid(false);

    // check that not too many hits are used by now
    // remove up to three used hits from the track
    int nUsed = 0;
    int maxUsed = int(std::ceil(m_maxUsedFractLowQual * track.nCoords()));
    for (itH = track.coordBegin(); track.coordEnd() != itH; ++itH) {
      if ((*itH)->isUsed()) {
	if (++nUsed <= 3) track.removeCoord(itH);
	else if (0 >= maxUsed--) break;
      }
    }
    if (maxUsed < 0) continue;
    bool isOT = isRegionOT((*track.coordBegin())->hit()->region());
    m_tolCollect = isOT ? m_tolCollectOT : m_tolCollectIT;
    m_maxChi2Hit = isOT ? m_maxChi2HitOT : m_maxChi2HitIT;

    // restore coordinates
    track.setYParams(0., 0.);
    track.sort();
    stereo.clear();
    for (itH = track.coordBegin(); track.coordEnd() != itH; ++itH) {
      PatFwdHit* hit = *itH;
      restoreCoordinate(hit);
      hit->setSelected( true );
      hit->setIgnored( false );
      if (! hit->hit()->isX()) {
	double y = ( track.xAtZ( hit->z() ) - hit->x() ) / hit->hit()->dxDy();
	y *= m_zForYMatch / hit->z();
	hit->setProjection(y);
	stereo.push_back(hit);
      }
    }
    // refit in yz
    double y0, sl;
    bool ok = fitLineInY( stereo, y0, sl );
    if (!ok) continue;
    track.setYParams(y0, sl);

    // check that pointing constraint still holds
    double yAtOrigin = y0;
    double yCorr     = sl * sl * track.curvature() * track.curvature() / m_yCorrection;
    if ( sl > 0 ) yAtOrigin = y0 - yCorr;
    else yAtOrigin = y0 + yCorr;
    if ( m_maxYAtOriginLowQual < fabs(yAtOrigin) ) continue;

    // check for compatibility with state (if we have one)
    if ( 0 != state && fabs(sl - state->ty()) > std::sqrt(state->errTy2()) ) {
      if ( m_printing ) info() << "  -- ty not compatible with state given" << endreq;
      continue;
    }

    //== Check for enough OT if not near boundary
    if ( isOT ) {
      double y = y0 + m_zForYMatch * sl;
      if ( m_centralYOT < fabs(y) ) {
	// we use a C style array here instead of std::vector because we
	// know exactly how large the array needs to be, and we do not
	// want to buy the additional cost of dynamically allocating
	// memory for std::vector's storage
	boost::array<int, 3> nInStation = { 0, 0, 0 };
        for ( itH = track.coordBegin(); track.coordEnd() != itH; ++itH ) {
	  if ( (*itH)->hit()->isX() ) continue;
          if ( (*itH)->isIgnored() ) continue;
	  nInStation[ (*itH)->hit()->station() ]++;
	}
	int nStationsBelowTwoStereoHits =
	  ((2 > nInStation[0])?1:0) +
	  ((2 > nInStation[1])?1:0) +
	  ((2 > nInStation[2])?1:0);
	if ( nStationsBelowTwoStereoHits > 0 ) {
          if ( m_printing ) info() << "Not enough OT stereo in some station : "
		  << nInStation[0] << " "
			  << nInStation[1] << " "
	    		  << nInStation[2] << endreq;
	  continue;
	}
      }
    }

    if ( isOT && m_otNHitsLowThresh > track.nCoords() &&
	2 * (track.nCoords()-track.nPlanes()+1) < track.nbOnSide() ) {
      if ( m_printing ) info()  << "Too many on side " << track.nbOnSide() << " with only "
	      << track.nCoords() << " coordinates" << endreq;
      continue;
    }

    track.sort();
    if ( m_printing ) info() << "-- Before fit, nPlanes = " << track.nPlanes() << endreq;
    ok = m_seedTool->fitTrack( track, m_maxFinalChi2, m_minTotalPlanes - 1, false, m_printing );
	
    if ( m_printing ) info() << "After fit status "<< ok << " chi2 " << track.chi2()
	    << ", nPlanes = " << track.nPlanes() << endreq;
    if ( isOT && m_otNHitsLowThresh > track.nCoords() &&  // OT short track -> not too many neighbours
	2 * (track.nCoords() - track.nPlanes() +1 ) < track.nbOnSide() ) {
      if ( m_printing ) {
        info()  << "Too many on side " << track.nbOnSide() << " with only "
		<< track.nCoords() << " coordinates" << endreq;
	BOOST_FOREACH( const PatFwdHit* hit, track.coords() )
          debugFwdHit( hit, info() );
      }
      continue;
    }

    if ( m_maxFinalTrackChi2 < track.chi2() || !ok) {
      if ( m_printing ) info() << "Chi2 too big" << endreq;
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
  out->setHistory( LHCb::Track::PatForward );
  out->setPatRecStatus( LHCb::Track::PatRecIDs );

  debug() << "==== Storing track " << outputTracks.size() << endreq;
  BOOST_FOREACH( PatFwdHit* hit, track.coords() ) {
    out->addToLhcbIDs( hit->hit()->lhcbID() );
    //== Tag used coordinates
    hit->hit()->setStatus(Tf::HitBase::UsedByPatSeeding);
    hit->setIsUsed(true);
  }
  if ( msgLevel( MSG::DEBUG ) )
    BOOST_FOREACH( const PatFwdHit* hit, track.coords() )
      debugFwdHit( hit, debug() );

  // get momentum estimate from track using the momentum estimation tool
  LHCb::State temp(
      Gaudi::TrackVector(track.xAtZ(m_zReference), track.yAtZ(m_zReference),
	track.xSlope(m_zReference), track.ySlope(m_zReference), 0.),
      m_zReference, LHCb::State::AtT);
  double qOverP, sigmaQOverP;
  if (m_fastMomentumTool->calculate(&temp, qOverP,
	sigmaQOverP, true).isFailure()) {
    // two reasons for this - 1. never leave a StatusCode unchecked
    // 2. if we ever get a failure here, we can still revert to calculating
    // q/p from curvature
    qOverP = 1. / (track.curvature() * m_momentumScale);
    sigmaQOverP = 0.5 * qOverP; // be conservative
  }
  temp.setQOverP(qOverP);
  //== overestimated covariance matrix, as input to the Kalman fit
  Gaudi::TrackSymMatrix& cov = temp.covariance();
  cov(0,0) = m_stateErrorX2;
  cov(1,1) = m_stateErrorY2;
  cov(2,2) = m_stateErrorTX2;
  cov(3,3) = m_stateErrorTY2;
  cov(4,4) = sigmaQOverP * sigmaQOverP;

  BOOST_FOREACH( const double z, m_zOutputs ) {
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
  if ( coord->hit()->type() == Tf::RegionID::OT ) {
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
  msg << endreq;
}

//=========================================================================
//  add the new track to the vector, if better, this means longer than
//  any other track sharing >70% of hits
//=========================================================================
void PatSeedingTool::addIfBetter (PatSeedTrack& track,
   std::vector<PatSeedTrack>& pool) const
{
  // this routine assumes that z0() is the same for all tracks
  const double xTrack = track.xAtZEqZ0();

  bool printing = msgLevel ( MSG::VERBOSE );
  
  if ( printing ) {
    info() << ".. Compare new track:" << endreq;
    BOOST_FOREACH( const PatFwdHit* hit, track.coords() )
      debugFwdHit( hit, info() );
  }

  // points to invalid entry which we can replace instead of making the
  // vector bigger; is 0 if no invalid entry found
  PatSeedTrack* lastInvalidTrack = 0;
  bool stored = false;
  BOOST_FOREACH( PatSeedTrack& othertrack, pool ) {
    if ( !othertrack.valid() ) {
      lastInvalidTrack = &othertrack;
      continue;
    }
    //== compare the list of hits only if not too far...
    if ( m_cloneMaxXDist < fabs( othertrack.xAtZEqZ0() - xTrack ) ) continue;

    // this is a funny way to check if there are too many common hits,
    // but we can stop early if this is the case
    int nCommonMax = int(std::ceil(m_commonXFraction *
			    std::min(othertrack.nCoords(), track.nCoords())));
    BOOST_FOREACH( const PatFwdHit* hit1, othertrack.coords() ) {
      if ( track.coordEnd() !=
          std::find(track.coordBegin(), track.coordEnd(), hit1) )
	if (0 > --nCommonMax) break;
    }

    if ( 0 <= nCommonMax ) continue;
    //== enough common in stored track: Keep the best (longest / best chi2)
    if ( printing ) {
      int nCommonMax = int(std::ceil(m_commonXFraction *
	    std::min(othertrack.nCoords(), track.nCoords())));
      // if we're debugging, we can be slow in determining where othertrack
      // is in pool
      int nTrack = 0;
      BOOST_FOREACH( const PatSeedTrack& tmp, pool ) {
	if (&tmp == &othertrack) break;
	++nTrack;
      }
      info() << "Track " << nTrack << " size " <<  othertrack.nCoords()
	<< " chi2 " << othertrack.chi2() << " has <= " << nCommonMax
	<< " hit shared with current track of size " << track.nCoords()
	<< " chi2 " << track.chi2() << endreq;
      BOOST_FOREACH( const PatFwdHit* hit, track.coords() )
	      debugFwdHit( hit, info() );
    }
    double qual1 = 0., qual2 = 0.;
    if (0. != m_qualityWeights[0]) {
      qual1 += m_qualityWeights[0] * double(track.nCoords());
      qual2 += m_qualityWeights[0] * double(othertrack.nCoords());
    }
    if (0. != m_qualityWeights[1]) {
      qual1 += m_qualityWeights[1] * track.chi2();
      qual2 += m_qualityWeights[1] * othertrack.chi2();
    }
    if ( qual1 < qual2 ) {
      if ( printing ) info() << "  new is shorter / higher chi2 -> ignore new" << endreq;
      return;
    }
    // reuse an entry instead of erasing the old and allocating a new one,
    // if there's more than one to be erased, flag it invalid instead of
    // erasing because erasing might involve moving a lot of data around
    // to close the resulting gap...
    if ( !stored ) {
      if ( printing ) info() << "  replacing old" << endreq;
      othertrack = track;
      stored = true;
    } else {
      if ( printing ) info() << "  invalidating old" << endreq;
      othertrack.setValid(false);
      lastInvalidTrack = &othertrack;
    }
  } // end of loop over pool
  if ( !stored ) {
    if ( printing ) info() << "  Store new one" << endreq;
    // if we can reuse an invalid entry, we do so
    if (0 != lastInvalidTrack)
      *lastInvalidTrack = track;
    else
      pool.push_back( track );
  }
  if ( m_printing ) {
    info() << " ****** stored track " << endreq;
    BOOST_FOREACH( const PatFwdHit* hit, track.coords() )
      debugFwdHit( hit, info() );
  }
  return;
}

//----------------------------------------------------------------------
// return updated X search windows for the X search in case a state is
// given
// input: a pointer to a state
// 	zref - z at which the search window extensions are needed
// 	xmin, xmax - edges of window
// output: intersection of previous window and window defined by state
// the state is extrapolated linearly and a region of +/- 1 sigma
// around it is kept
// if the state pointer is 0, do nothing
//----------------------------------------------------------------------
void PatSeedingTool::updateSearchWindowX(double zref,
		double& xmin, double& xmax, const LHCb::State* state) const
{
	// how far do we need to propagate?
	double dz = zref - state->z();
	// work out expected uncertainty
	double xtol = std::sqrt(state->covariance()(0, 0)
			+ 2.0 * dz * state->covariance()(0, 2)
			+ dz * dz * state->covariance()(2,2));
	// intersect with present window
	double xmi = state->x() + state->tx() * dz - xtol;
	if ( xmi > xmin ) xmin = xmi;
	double xma = state->x() + state->tx() * dz + xtol;
	if ( xma < xmax ) xmax = xma;
}

void PatSeedingTool::updateSearchWindowY(double zref, double xref,
		double xreftol, double tanT,
		double& xmin, double& xmax, const LHCb::State* state) const
{
	// how far do we need to propagate?
	double dz = zref - state->z();
	// work out expected uncertainty
	double ytol = std::sqrt(state->covariance()(1, 1)
			+ 2.0 * dz * state->covariance()(1, 3)
			+ dz * dz * state->covariance()(3,3));
	double y = state->y() + state->ty() * dz;
	// intersect with present window
	double xmi = -ytol * tanT;
	double xma =  ytol * tanT;
	if (xmi > xma) std::swap(xmi, xma);
	xref += y * tanT;
	xmi += xref - xreftol;
	xma += xref + xreftol;
	if ( xmi > xmin ) xmin = xmi;
	if ( xma < xmax ) xmax = xma;
}

//=========================================================================
//  Find the x candidates in the specified layer and region. Results in candidates
//  returns false if there are too few hits to form a track in the region in question
//=========================================================================
void PatSeedingTool::findXCandidates ( unsigned lay, unsigned reg,
		std::vector<PatSeedTrack>& pool,
		const LHCb::State* state)
{
  HitRange::const_iterator itH, itBeg, itH0, itH1, itH2;

  // depending on the region, we have to correct for the OT tilt because it
  // causes a z displacement (negligible for the IT):
  // we do not know the absolute z for a hit, but for tracks with ty = 0, the z
  // coordinates we'd get would be consistent
  // for tracks with ty != 0, the tilt causes z coordinates of hits to deviate,
  // and we account for that by widening the search windows based on the
  // expected minimal/maximal slopes ty in that OT region
  double tymin, tymax;
  getTyLimits(reg & 0x7, tymin, tymax);

  // decode layer bit mask to determine which layer to use in which station
  // to form the initial parabola
  const unsigned lay0 = (lay&1)?3:0, lay1 = (lay&2)?3:0, lay2 = (lay&4)?3:0;
  // if we should reconstruct cosmics, we can no longer rely on the fact that
  // very few tracks migrate between regions
  // thus, we "decode" reg in a way similar to what is done for layers above
  // (only in the cosmic case)
  const unsigned reg0 = reg & 0x07;
  const unsigned reg1 = ((m_cosmics && (reg & 0x08))?(reg0 ^ 1):reg0);
  const unsigned reg2 = ((m_cosmics && (reg & 0x10))?(reg0 ^ 1):reg0);
  
  const double zScaling  = ( regionZ0(2,lay2,reg0) - regionZ0(0,lay0,reg0) ) /
    ( regionZ0(0,lay0,reg0) - m_zMagnet );
  const double zScaling2 = ( regionZ0(2,lay2,reg0) - regionZ0(0,lay0,reg0) ) /
    regionZ0(0,lay0,reg0);

  bool matchIn0 = false;
  bool matchIn2 = false;

  // if we have a state, we don't need hits from the whole region
  double x0Min = -1e42, x0Max = 1e42;
  if (0 != state)
    updateSearchWindowX(regionZ0(0,lay0,reg0), x0Min, x0Max, state);
  HitRange range = (0 == state) ? hits(0, lay0, reg0) :
    hitsWithMinX( x0Min, 0, lay0, reg0);
  // NB: no correction for the tilt for a single point

  for ( itH0 = range.begin(); range.end() != itH0; ++itH0 ) {
    PatFwdHit* hit0 = *itH0;
    if ( 0 != state ) {
      if ( x0Max < hit0->hit()->xAtYEq0() ) break;
      if ( x0Min > hit0->hit()->xAtYEq0() ) continue;
    }
    if ( hit0->isUsed() )  continue;

    matchIn0 = msgLevel( MSG::VERBOSE );
    
    double x0 = hit0->hit()->xAtYEq0();
    double z0 = hit0->hit()->zAtYEq0();

    if ( hit0->hasNext() && ! (*(itH0 + 1))->isUsed() ) {
      hit0 = *(++itH0);
      if ( matchIn0 ) info() << "*** Hit0 " << x0 << " + " << hit0->x();
      combineCluster(*(itH0 - 1), *itH0, x0, z0);
    } else if ( matchIn0 ) {
      info() << "*** Hit0 " << hit0->x();
    }

    // isolation in station 0?
    if ( m_enforceIsolation && !isIsolated(itH0, range) )
	    continue;

    m_printing = matchIn0;

    // derive search windows based on center of magnet compatibility and
    // (if applicable) minimum momentum, and take the tightest combination
    double x2Min = x0 + std::max((x0 - m_xMagTol) * zScaling,
	(x0 - m_maxImpact) * zScaling2);
    double x2Max = x0 + std::min((x0 + m_xMagTol) * zScaling,
	(x0 + m_maxImpact) * zScaling2);
    // no need to correct for tilt here, the windows are wide anyway

    // if we know more because we have a state, make use of that knowledge
    if (0 != state)
      updateSearchWindowX(regionZ0(2,lay2,reg2), x2Min, x2Max, state);

    if ( m_printing ) info() << " x2Min " << x2Min << " x2Max " << x2Max << endreq;

    HitRange rangeX2 = hitsWithMinX( x2Min, 2, lay2, reg2 );
    for ( itH2 = rangeX2.begin(); rangeX2.end() != itH2; ++itH2 ) {
      PatFwdHit* hit2 = *itH2;
      if ( x2Max < hit2->hit()->xAtYEq0() ) break;
      if ( hit2->isUsed() ) continue;
      if ( x2Min > hit2->hit()->xAtYEq0() ) continue;
      double x2 = hit2->hit()->xAtYEq0();
      double z2 = hit2->hit()->zAtYEq0();

      m_printing = matchIn0;
      matchIn2 = false;
      
      if ( hit2->hasNext() && ! (*(itH2 + 1))->isUsed() ) {
        hit2 = *(++itH2);
	if ( m_printing ) info() << "    hit2 " << x2 << " + " << hit2->x();
	combineCluster(*(itH2 - 1), *itH2, x2, z2);
      } else if ( m_printing ) {
        info() << "    hit2 " <<  hit2->hit()->xAtYEq0();
      }

      // isolation in station 2?
      if ( m_enforceIsolation && !isIsolated(itH2, rangeX2) )
	    continue;

      //== Check center of magnet compatibility
      double slope = ( x2 - x0 ) / ( z2 - z0 );
      // check for compatibility with slope
      if (0 != state && fabs(slope - state->tx()) >
		      std::sqrt(state->errTx2()))
	      continue;

      // first tilt correction: slope ty (still unknown) gives a correction
      // to z at first layer
      double dz = regionZ0(1,lay1,reg1) - z0;
      double x1Min = slope * regionDzDy(1,lay1,reg1) * tymin * dz;
      double x1Max = slope * regionDzDy(1,lay1,reg1) * tymax * dz;
      if (x1Min > x1Max) std::swap(x1Min, x1Max);
      // work out intercept
      double intercept =  x0 - z0 * slope;
      // and use it to correct for curvature in prediction of x1
      double x1Pred = x0 + dz * (slope + m_initialArrow * intercept * dz);
      x1Min += x1Pred - m_curveTol;
      x1Max += x1Pred + m_curveTol;
      // if we know more because we have a state, make use of that knowledge
      if (0 != state)
	updateSearchWindowX(regionZ0(1,lay1,reg1), x1Min, x1Max, state);

      if ( m_printing ) info() << " x1Pred " << x1Pred << " tol " << m_curveTol
                               << " linear " << x0 + dz * slope << endreq;

      HitRange rangeX1 = hitsWithMinX( x1Min, 1, lay1, reg1 );
      for ( itH1 = rangeX1.begin(); rangeX1.end() != itH1; ++itH1 ) {
        PatFwdHit* hit1 = *itH1;

        if ( x1Max < hit1->hit()->xAtYEq0() ) break;
        if ( hit1->isUsed() ) continue;
        if ( x1Min > hit1->hit()->xAtYEq0() ) continue;

        double x1 = hit1->hit()->xAtYEq0();
        double z1 = hit1->hit()->zAtYEq0();
        if ( hit1->hasNext()  && ! (*(itH1 + 1))->isUsed() ) {
          hit1 = *(++itH1);
          if ( m_printing ) info() << "         ==> found hit1 " << x1 << " + " << hit1->x() << endreq;
	  combineCluster(*(itH1 - 1), *itH1, x1, z1);
        } else if ( m_printing ) {
          info() << "         ==> found hit1 " << x1 << endreq;
        }

	// isolation in station 1?
	if (m_enforceIsolation && !isIsolated(itH1, rangeX1))
		continue;

        PatSeedTrack track( x0, x1, x2, z0, z1, z2, m_zReference, m_dRatio );
	// one more slope check to make sure our third hit is ok
	if (0 != state && fabs(track.xSlope(z1) - state->tx()) >
			std::sqrt(state->errTx2()))
		continue;
        track.setYParams( 0, 0);
        int nbMissed = 0;

	// if we reconstruct cosmics, we need to look in all regions for hits
	// (cosmic IT hits are rarely in different stations)
	for (unsigned reginc = 0; reginc < (m_cosmics?m_nReg:1); ++reginc) {
	  unsigned regc = (reg0 + reginc) % m_nReg; // current region
	  // start with the other 3 layers
	  unsigned olay = lay0 ^ 3; // x layers have numbers 0 and 3
	  for ( unsigned nLay = 2; nLay-- ; olay ^= 3 ) {
	    for ( unsigned sta = 0; sta < m_nSta; ++sta ) {
	      // work out the range in x in which to look for hits
	      // apply correction for the tilt
	      double dz = regionZ0(sta,olay,regc) - m_zReference;
	      double dzmin = dz * tymin * regionDzDy(sta,olay,regc);
	      double dzmax = dz * tymax * regionDzDy(sta,olay,regc);
	      double xPredMin = track.xAtZ( regionZ0(sta,olay,regc) + dzmin );
	      double xPredMax = track.xAtZ( regionZ0(sta,olay,regc) + dzmax );
	      if ( xPredMin > xPredMax ) std::swap( xPredMin, xPredMax );
	      if (m_cosmics) {
		// cosmic case: skip if the track does not come close to the
		// region in question
		if (region(sta, olay, regc)->xmin() > xPredMax ||
		    region(sta, olay, regc)->xmax() < xPredMin) continue;
	      }

	      HitRange rangeX2 = hitsWithMinX( xPredMin - 2.0 * m_tolCollect,
		  sta, olay, regc );
	      int found = 0;
	      BOOST_FOREACH( PatFwdHit* hit, rangeX2 ) {
		if ( xPredMax + 2.0 * m_tolCollect < hit->hit()->xAtYEq0() )
		  break;
		if ( hit->isUsed() ) continue;
		//== restore default measure
		restoreCoordinate(hit);
		// here, we know z of the hit at y = 0, so we can compare to
		// a more accurate x prediction of the track
		dz = hit->hit()->zAtYEq0() - m_zReference;
		dzmin = dz * tymin * regionDzDy(sta,olay,regc);
		dzmax = dz * tymax * regionDzDy(sta,olay,regc);
		xPredMin = track.xAtZ( hit->hit()->zAtYEq0() + dzmin );
		xPredMax = track.xAtZ( hit->hit()->zAtYEq0() + dzmax );
		if ( xPredMin > xPredMax ) std::swap( xPredMin, xPredMax );
		if ( xPredMax + m_tolCollect + hit->driftDistance() <
		    hit->hit()->xAtYEq0() ) break;
		if ( xPredMin - m_tolCollect - hit->driftDistance() >
		    hit->hit()->xAtYEq0() ) continue;
		if ( m_enforceIsolation && !isIsolated(itH, rangeX2) )
		  continue;
		track.addCoord( hit );
		++found;
	      }
	      if ( 0 == found && reg0 == regc) nbMissed++;
	      if ( m_printing ) {
		info() << format( "         --- sta%2d lay%2d xPred%8.2f "
		    "found%2d nbMissed%2d", sta, olay,
		    track.xAtZ( regionZ0(sta,olay,regc) ), found,  nbMissed )
		  << endreq;
	      }
	      if ( m_maxMisses < nbMissed ) break;
	    }
	    if ( m_maxMisses < nbMissed ) break;
	  }
	  if ( m_maxMisses < nbMissed ) break;
	}
	if ( m_maxMisses < nbMissed ) continue;

        //== Fit the track
        if ( m_printing ) info() << " -- start fitting, nCoord = " << track.nCoords() << endreq;
        track.sort();
        bool fitOK = m_seedTool->fitTrack( track, m_maxChi2Hit, m_minXPlanes, true, m_printing );
        if ( !fitOK ) {
          if ( m_printing ) info() << "    -- fit failed" << endreq;
          continue;
        }
        if ( m_maxTrackChi2 < track.chi2() ) {
          if ( m_printing ) info() << "    -- global chi2 too high " << track.chi2() << endreq;
          continue;
        }

	if (m_fieldOff) {
	  // cut on curvature when running without magnetic field
	  double dz = 0.5 * (z2 - z0);
	  if (m_tolCollect / dz / dz < fabs(track.curvature())) {
	    if ( m_printing ) info() << "    magnet off: curvature too high: "
		    << track.curvature() << endreq;
	    continue;
	  }
	}

        //== Limited OT multiplicity -> check if compatible with in the central Y region
        if ( isRegionOT(reg) && 8 > track.nCoords() ) {
          fitOK = m_seedTool->fitTrack( track, m_maxFinalChi2, m_minXPlanes, true, m_printing );
          if ( !fitOK ) {
            if ( m_printing ) info() << "    -- re-fit with final Chi2 failed" << endreq;
            continue;
          }  
          
	  if ( 2 * ( track.nCoords() - track.nPlanes() + 1 ) < track.nbOnSide() ) {
            if ( m_printing ) info() << "    -- only " << track.nCoords() << " and " << track.nbOnSide()
                                     << " neighbouring OT hits" << endreq;
            continue;
          }
          if ( m_maxTrackChi2LowMult < track.chi2() ) {
            if ( m_printing ) info() << "    -- global chi2 too high " << track.chi2() << endreq;
            continue;
          }
        }

        if ( isRegionIT(reg) && 3 > track.nbHighThreshold() ) {
          if ( m_printing ) info() << "    -- only " << track.nbHighThreshold()
                                   << " high threshold IT hits" << endreq;
          continue;
        }

	// one last check for the slope - just in case something was
	// changed by the fit
	if (0 != state && fabs(track.xSlope(z1) - state->tx()) >
			std::sqrt(state->errTx2()))
		continue;
        addIfBetter( track, pool );
      }
    }
  }

  //== Final refit, as track may have been modified...
  BOOST_FOREACH( PatSeedTrack& track, pool ) {
    if (!track.valid()) continue;

    track.sort();
    m_printing = msgLevel( MSG::DEBUG );
            
    bool fitOK = m_seedTool->fitTrack( track, m_maxChi2Hit, m_minXPlanes, true, m_printing );
    // cut on chi2 per DoF
    track.setValid( fitOK && ( m_maxFinalChi2 >= track.chi2() ) );
  }
}

//=========================================================================
//  Find all stereo hits compatible with the track.
//=========================================================================
void PatSeedingTool::collectStereoHits ( PatSeedTrack& track,
		unsigned reg, PatFwdHits& stereo,
		const LHCb::State* state ) {

  HitRange::const_iterator itH;
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
	// get ty limits depending on region
	double tymin, tymax;
	getTyLimits(testRegion, tymin, tymax);

	// get tan(stereo angle of layer)
	double tanT = regionTanT(sta, sLay, testRegion);

	// work out range in x direction due to stereo angle
	double xMin = region(sta, sLay, testRegion)->ymin() * tanT;
	double xMax = region(sta, sLay, testRegion)->ymax() * tanT;
	if ( !m_cosmics &&
	    ( ( isRegionOT(testRegion) && 7 > track.nCoords() ) ||
	      ( isRegionIT(testRegion) && 4 > track.nCoords() ) ) ) {
	  // Low number of coordinates: if in OT or the IT stations to the
	  // left or the right of the beam pipe, ask Y to be close to 0...
	  switch (testRegion) {
	    case 0:
	      xMin = - m_centralYOT * tanT;
	      break;
	    case 1:
	      xMax =   m_centralYOT * tanT;
	      break;
	    case 2:
	    case 3: // regions 2 and 3 are treated alike
	      xMin = - m_centralYIT * tanT;
	      xMax =   m_centralYIT * tanT;
	    default:
	      break;
	  }
	}
	if ( xMin > xMax ) std::swap(xMin, xMax);
	// cache track position at layer's tilt-corrected z
	double dz = regionZ0(sta, sLay, testRegion) - m_zReference;
	double zTrMin = regionZ0(sta,sLay,testRegion) +
	  tymin * dz * regionDzDy(sta,sLay,testRegion);
	double zTrMax = regionZ0(sta,sLay,testRegion) +
	  tymax * dz * regionDzDy(sta,sLay,testRegion);
	double xTrMin = track.xAtZ(zTrMin);
	double xTrMax = track.xAtZ(zTrMax);
	if (xTrMin > xTrMax) std::swap(xTrMin, xTrMax);
	// shift range xMin, xMax to track x position
	xMin += xTrMin - m_tolCollect;
	xMax += xTrMax + m_tolCollect;
	// use additional info from state to narrow down the range further
	// if such info is available
	if (0 != state) updateSearchWindowY(0.5 * (zTrMin + zTrMax),
	    0.5 * (xTrMin + xTrMax), m_tolCollect, tanT, xMin, xMax, state);

	if ( m_printing )
		info() << format( "Stereo in sta%2d lay%2d region%2d xMin %7.2f xmax %7.2f",
				sta, sLay, testRegion, xMin, xMax ) << endreq;

	int found = 0;
	boost::array<unsigned char, 20> nDense;
	if ( m_enforceIsolation )
	  std::fill(nDense.begin(), nDense.end(), 0);

	HitRange rangeW = hitsWithMinX( xMin, sta, sLay, testRegion);
	BOOST_FOREACH( PatFwdHit* hit, rangeW ) {
          if ( xMax < hit->hit()->xAtYEq0() ) break;
	  if ( hit->isUsed() ) continue;
          if ( xMin > hit->hit()->xAtYEq0() ) continue;
	  restoreCoordinate(hit);
          double y = ( track.xAtZ( hit->z() ) - hit->x() ) / hit->hit()->dxDy();
	  // check if the hit is isolated
	  if ( m_enforceIsolation && !isIsolated(itH, rangeW) ) {
		  int idx = int(fabs(y) * 20. / 3e3);
		  if (idx < 0) idx = 0;
		  if (idx >= 20) idx = 19;
		  if (nDense[idx]++ > 0)
			  continue;
	  }
	  // go to projection plane
          y *= m_zForYMatch / hit->z();
          hit->setProjection( y );
          hit->setIgnored( false );
          stereo.push_back( hit );
	  ++found;
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
    unsigned reg, int minNbPlanes, PatFwdHits& stereo ) const
{
  double maxRangeSize = m_maxRangeOT;
  if ( isRegionIT(reg) ) maxRangeSize = m_maxRangeIT;

  PatFwdHits::iterator itBeg = stereo.begin();
  PatFwdHits::iterator itEnd = itBeg + minNbPlanes;

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

  PatFwdHits::iterator itBest = itBeg;
  PatFwdHits::iterator itLast = itEnd;
  double minDist = (*(itEnd-1))->projection() - (*itBeg)->projection();

  if ( m_printing ) {
    info() << format( "        range minDist %7.2f from %8.3f to %8.3f minNbPlanes %2d",
                      minDist, (*itBeg)->projection(), (*(itEnd-1))->projection(), minNbPlanes )
           << endreq;
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
	    << endreq;
	}
      }
    }
  }
  itBeg = itBest;
  itEnd = itLast;

  //== add OT hits on side, except if 6 planes and we are near y=0
  double tolSide = m_tolCollect / fabs( (*itBeg)->hit()->dxDy() );
  if ( 6 == minNbPlanes &&
      m_centralYOT > fabs( (*itBeg)->projection() ) &&
       m_centralYOT > fabs( (*(itEnd-1))->projection() ) ) tolSide = 0.;

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
           << endreq;
  }

  // remove other hits - we don't want to ignore lots of hits in fitLineInY
  // below
  PatFwdHits::iterator itH = itEnd;
  if (itBeg != stereo.begin())
    for (itH = stereo.begin(); itBeg != itEnd; ++itH, ++itBeg) *itH = *itBeg;
  if (itH != stereo.end())
    stereo.erase(itH, stereo.end());

  return true;
}

bool PatSeedingTool::findBestRangeCosmics(
    unsigned reg, int minNbPlanes, PatFwdHits& stereo ) const
{
  // cosmics do not point back to the origin, so they need special treatment
  // (this means the projective approach does not help in this case, so we
  // can't just use a sliding window in the projective plane and count the
  // number of planes inside to find our pattern)

  // check that we have a chance to fulfill the requirements
  if (minNbPlanes > int(stereo.size())) return false;
  {
    PatFwdPlaneCounter count0(stereo.begin(), stereo.end());
    if (minNbPlanes > count0.nbDifferent()) return false;
  }

  const double maxRangeSize = isRegionOT(reg)?m_maxRangeOT:m_maxRangeIT;
  PatFwdHits best, current;
  for (PatFwdHits::const_iterator it = stereo.begin();
      stereo.end() != it; ++it) {
    const PatFwdHit *h1 = *it;
    for (PatFwdHits::const_iterator jt = it + 1;
	stereo.end() != jt; ++jt) {
      const PatFwdHit *h2 = *jt;
      // only consider pairs of hits from same region, but different stations
      // (to make sure we have sufficient lever arm)
      if (h1->hit()->region() != h2->hit()->region()) continue;
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
      BOOST_FOREACH( PatFwdHit* hit, stereo ) {
	if (fabs(hit->projection() - y0 / hit->z() - ty) > maxRangeSize)
	  continue;
	current.push_back(hit);
      }

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
bool PatSeedingTool::fitLineInY ( PatFwdHits& stereo, double& y0, double& sl ) const
{
  const double maxYDist = 20. * m_tolCollect;

  // we use builtin arrays here because we know how big they need to be,
  // and we do not want the additional cost of allocating memory dynamically
  // for std::vector
  boost::array<double, 3> largestDrift = { 0., 0., 0. };
  boost::array<PatFwdHit*, 3> seeds;
  double slopeguess = 0.;
  int npoints = 0;

  BOOST_FOREACH( PatFwdHit* hit, stereo ) {
    if ( hit->isIgnored() ) continue;
    // calculate average slope as first guess for IT or OT without
    // or very small drift times
    slopeguess += hit->projection(); ++npoints;
    // go from prejection plane to y of hit
    hit->setProjection( hit->projection() * hit->z() / m_zForYMatch );

    if (hit->hit()->type() != Tf::RegionID::OT ) continue;
    hit->setRlAmb( 0 );
    const int sta = hit->hit()->station();
    if ( largestDrift[sta] < hit->driftDistance() ) {
      largestDrift[sta] =  hit->driftDistance();
      seeds[sta] = hit;
    }
  }
  slopeguess /= double(npoints);

  // place hits with largest drift dist. into seeds[0] and seeds[2]
  if ( largestDrift[1] > largestDrift[0] &&
       largestDrift[1] > largestDrift[2] ) {
    if ( largestDrift[0] < largestDrift[2] ) {
      seeds[0] = seeds[1];
    } else {
      seeds[2] = seeds[1];
    }
  } else if ( largestDrift[1] > largestDrift[0] ) {
    seeds[0] = seeds[1];
  } else if ( largestDrift[1] > largestDrift[2] ) {
    seeds[2] = seeds[1];
  }

  double bestY0 = 0., bestSl = slopeguess;
  if ( 0.1 < largestDrift[0] && 0.1 < largestDrift[2] ) {
    // try all four combinations to resolve ambiguity for seeds[0]. seeds[2]
    // and take best, fix those ambiguities
    double x0 = seeds[0]->projection();
    double x2 = seeds[2]->projection();
    double z0 = seeds[0]->z();
    double z2 = seeds[2]->z();
    double d0 = seeds[0]->driftDistance()/fabs(seeds[0]->hit()->dxDy());
    double d2 = seeds[2]->driftDistance()/fabs(seeds[2]->hit()->dxDy());
    if ( m_printing ) {
      info() << format( "  0: z %8.2f y %7.2f d %7.2f", z0, x0, d0 ) << endreq;
      info() << format( "  2: z %8.2f y %7.2f d %7.2f", z2, x2, d2 ) << endreq;
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
      BOOST_FOREACH( const PatFwdHit* hit, stereo ) {
        if ( hit->isIgnored() ) continue;
	double y = hit->projection();
        double dist = y - y0 - hit->z() * sl;
        double drift = hit->driftDistance() / hit->hit()->dxDy();
	// resolve ambiguity towards line defined by y0, sl
        dist = std::min(fabs(dist - drift), fabs(dist + drift));
        if ( m_printing ) info() << format( "    z%8.2f proj%8.2f drift%8.2f dist%8.2f",
                                            hit->z(), hit->projection(), drift, dist ) << endreq;
        chi2 += dist * dist;
      }
      if ( m_printing ) info() << format( "-- y fit: mask%2d y0 %8.2f sl%10.6f chi2 %12.2f",
                                          mask, y0, sl, chi2 ) << endreq;
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
    BOOST_FOREACH( const PatFwdHit* hit, stereo ) {
      if ( hit->isIgnored() ) continue;
      double proj = hit->projection();
      double dist = proj - y0 - hit->z() * sl;
      double drift = hit->driftDistance() / hit->hit()->dxDy();
      // resolve ambiguity towards old fit parameters
      proj += (fabs(dist - drift) < fabs(dist + drift))?(-drift):(drift);
      line.addPoint( hit->z(), proj, hit->hit()->weight() );
      ++nOK;
    }
    if ( 4 > nOK ) return false;
    line.solve();
    y0 = line.ax();
    sl = line.bx();
    // find worst hit
    double worstDist = 0.;
    PatFwdHits::iterator worst = stereo.begin(), end = stereo.end();
    for ( PatFwdHits::iterator itH = worst; end != itH; ++itH ) {
      const PatFwdHit* hit = *itH;
      if ( hit->isIgnored() ) continue;
      double proj = hit->projection();
      double dist = proj - y0 - hit->z() * sl;
      double drift = hit->driftDistance() / hit->hit()->dxDy();
      // resolve ambiguity towards new fit parameters
      dist = std::min(fabs(dist - drift), fabs(dist + drift));
      if ( dist > worstDist ) {
        worstDist = dist;
        worst = itH;
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
  if ( !exist<LHCb::Tracks>( location ) ) {
    if ( m_measureTime ) m_timer->stop(m_timeReuseTracks);
    return;
  }

  LHCb::Tracks::const_iterator itT;
  LHCb::Tracks* inputTracks  = get<LHCb::Tracks>( location );
  outputTracks.reserve(outputTracks.size() + inputTracks->size());
  std::vector<LHCb::LHCbID> ids;
  ids.reserve(64);
  // loop over input tracks
  BOOST_FOREACH( const LHCb::Track* track, *inputTracks ) {
    ids.clear();
    // copy the LHCbIDs which are either IT or OT hits
    const std::vector<LHCb::LHCbID>& allids = track->lhcbIDs();
    std::remove_copy_if( allids.begin(), allids.end(),
	std::back_inserter(ids), std::not1(isTStation()));
    //== Check for clone, i.e. similar list of Ids.
    bool match = false;
    const LHCb::State& state = track->closestState(m_zReference);
    // loop over output tracks to compare with current input track
    for ( std::vector<LHCb::Track*>::iterator itS = outputTracks.begin();
	outputTracks.end() != itS; ++itS) {
      const LHCb::Track* track2 = (*itS);
      const LHCb::State& state2 = track2->closestState(m_zReference);
      // if the tracks are too far apart, they can not be clones
      if (m_forwardCloneMaxXDist < fabs(state.x() - state2.x())) continue;
      if (m_forwardCloneMaxYDist < fabs(state.y() - state2.y())) continue;
      if (m_forwardCloneMaxTXDist < fabs(state.tx() - state2.tx())) continue;
      // (no separate cut on ty due to strong correlation of y and ty)

      // ok, tracks are close in terms of track parameters, so compare
      // on id level
      const std::vector<LHCb::LHCbID>& ids2 = track2->lhcbIDs();
      const double maxCommon = m_forwardCloneMaxShared *
	double(std::min(ids.size(), ids2.size()));
      const double maxMissed = (1. - m_forwardCloneMaxShared) *
	double(std::min(ids.size(), ids2.size()));
      for (std::vector<LHCb::LHCbID>::const_iterator itH = ids.begin();
	  ids.end() != itH; ++itH ) {
	unsigned nCommon = 0;
	unsigned nMissed = 0;
	if (ids2.end() == std::find(ids2.begin(), ids2.end(), *itH)) {
	  // if too many missed, abort the comparison.
	  if ( ++nMissed > maxMissed ) break;
	} else {
	  // if too many in common already, stop early as well
	  if ( ++nCommon > maxCommon ) {
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
	if (ids2.size() >= ids.size()) break;
	// otherwise, we reset the flag stating that we found a clone and
	// erase the short version from our output (and add the longer
	// version to the output below)
	match = false;
	delete *itS; // avoid creating a memory leak
	outputTracks.erase(itS); // and erase the pointer
	break;
      } else if (m_forwardCloneMergeSeg) {
	// id lists are not similar but track parameters are, so just combine
	// hits from both tracks
	match = true;
	for (std::vector<LHCb::LHCbID>::const_iterator itH = ids.begin();
	    ids.end() != itH; ++itH ) {
	  if (ids2.end() == std::find(ids2.begin(), ids2.end(), *itH))
	    (*itS)->addToLhcbIDs(*itH);
	}
	(*itS)->addToAncestors(track);
	break;
      }
    } // loop over output tracks
    // if the track in inputTracks matched one already found, we skip it
    if (match) continue;

    // build a track from the T station part and store it in outputTracks
    LHCb::Track* out = new LHCb::Track();
    out->addToAncestors( track );
    out->setType( LHCb::Track::Ttrack );
    out->setHistory( LHCb::Track::PatSeeding );
    out->setPatRecStatus( LHCb::Track::PatRecIDs );
    out->setLhcbIDs( ids );
    for ( unsigned i = 0; i < m_zOutputs.size(); i++ )
      out->addToStates( track->closestState(m_zOutputs[i]) );
    outputTracks.push_back( out );
  } // loop over input tracks
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
    BOOST_FOREACH( LHCb::Track* track, outvec )
      outputTracks->insert(track);
  }

  return sc;
}

//======================================================================
bool PatSeedingTool::isIsolated(HitRange::const_iterator it,
		const HitRange& range) const
{
	double isolationRange = m_OTIsolation;
	if ( (*it)->hit()->sthit() )
		isolationRange = m_ITIsolation;
	// create a window around *it which contains all hits within the
	// window from x - isolationRange to x + isolationRange
	double x = (*it)->hit()->xAtYEq0();
	HitRange::const_iterator iLo = it, iHi = it + 1;
	while ( range.begin() != iLo &&
			fabs((*(iLo - 1))->hit()->xAtYEq0() - x) < isolationRange )
		--iLo;
	if ( fabs((*iLo)->hit()->xAtYEq0() - x) > isolationRange )
		++iLo;
	while ( range.end() != iHi &&
			fabs((*iHi)->hit()->xAtYEq0() - x) < isolationRange )
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
