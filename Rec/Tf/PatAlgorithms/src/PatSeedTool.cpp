// Include files

#include <cmath>
#include <array>

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"


// local
#include "PatSeedTool.h"

#include "TfKernel/RegionID.h"

#include "PatSeedFits.h"

using namespace PatSeedFits;

//-----------------------------------------------------------------------------
// Implementation file for class : PatSeedTool
//
// 2006-10-23 : Olivier Callot
//-----------------------------------------------------------------------------


// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( PatSeedTool )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatSeedTool::PatSeedTool( const std::string& type,
    const std::string& name,
    const IInterface* parent )
: GaudiTool ( type, name , parent )
{
  declareInterface<PatSeedTool>(this);
  declareProperty("AmbigFromLargestDrift", m_ambigFromLargestDrift = false);
  declareProperty("AmbigFromPitchResiduals", m_ambigFromPitchResiduals = true);
}
//=============================================================================
// Destructor
//=============================================================================
PatSeedTool::~PatSeedTool() {}

template <class FIT, bool xOnly>
bool PatSeedTool::removeHitsWhileChi2TooLarge(
    PatSeedTrack& track, const double maxChi2,
    const unsigned minPlanes, const bool debug) const
{
  /// little helper to make things a bit more readable
  struct WorstHitFinder {
    typename FIT::value_type m_chi2;
    typename FIT::value_type m_worstchi2;
    PatFwdHits::iterator m_worst;
    WorstHitFinder(const FIT& fit, PatSeedTrack& track) :
      m_chi2(0), m_worstchi2(0), m_worst(track.coordBegin())
    {
      for (auto it = m_worst; track.coordEnd() != it; ++it) {
	const auto hit = *it;
	if (!hit->isSelected()) continue;
	if (xOnly && !hit->hit()->isX()) continue;
	const auto chi2hit = fit.chi2(track, hit);
	m_chi2 += chi2hit;
	if (chi2hit > m_worstchi2) m_worstchi2 = chi2hit, m_worst = it;
      }
    }
  };

  // keep track of number of degrees of freedom ourselves...
  int nHitsX = -3, nHitsStereo = xOnly ? 0 : -2;
  for (const PatFwdHit* hit : track.m_coords) {
    if (!hit->isSelected()) continue;
    if (xOnly || hit->hit()->isX()) ++nHitsX;
    else if (!xOnly) ++nHitsStereo;
  }
  // ... and stop early if we don't have enough hits
  if (0 > nHitsX || 0 > nHitsStereo) return false;

  FIT fit(track, track.m_coords);
  if (!fit) return false;

  bool okay;
  do {
    WorstHitFinder worst(fit, track);

    if (debug) {
      MsgStream& msg = info();
      for (const PatFwdHit* hit: track.coords()) {
	if (!hit->isSelected()) continue;
	printTCoord(msg, track, hit);
	if (*worst.m_worst == hit) msg << " -- worst";
	msg << endmsg;
      }
    }
    okay = (!(0. < maxChi2) || maxChi2 >= worst.m_worstchi2) &&
      minPlanes <= track.nPlanes() && 0 <= nHitsX && 0 <= nHitsStereo;
    // if we're above thresholds, and can remove hits, do so, and refit
    if (!okay) {
      if (debug) {
	info() << " Remove hit and try again " << endmsg;
      }
      // keep track of what we're about to remove...
      if (xOnly || (*worst.m_worst)->hit()->isX()) --nHitsX;
      else if (!xOnly) --nHitsStereo;
      // ... remove ...
      track.removeCoord(worst.m_worst);
      // ... and refit only if we know it should work...
      const bool expectFitOkay = 0 <= nHitsX && 0 <= nHitsStereo && 0 < fit.ndf()
	&& minPlanes <= track.nPlanes();
      if (expectFitOkay) {
	// full refit because of OT!
	fit.refit(track, track.m_coords);
      }

      if (!expectFitOkay || !fit) {
	if (debug) info() << " Abandon: Only " << track.nPlanes()
	  << " planes, min " << minPlanes  << " highestChi2 "
	    << worst.m_worstchi2 << " status " << fit.status() << " nDoF "
	    << fit.ndf() << endmsg;
	return false;
      }
      // force next iteration
      continue;
    }
    // okay, we're done (either below chi^2 threshold, or not enough planes)
    if ( debug ) info() << ".. done with " << track.nPlanes()
      << " planes, min " << minPlanes << " highestChi2 "
	<< worst.m_worstchi2 << endmsg;
    // set chi2/ndf, but avoid division by 0
    track.setChi2((0 < fit.ndf()) ?
	(worst.m_chi2 / typename FIT::value_type(fit.ndf())) :
	std::numeric_limits<typename FIT::value_type>::max());
  } while (!okay);
  return okay;
}

//=========================================================================
//  Fit a Seed track
//=========================================================================
template <PatSeedTool::TrackType hittype>
bool PatSeedTool::fitTrack( PatSeedTrack& track,
    double maxChi2, unsigned minPlanes, bool xOnly, bool forceDebug ) const
{
  // if we don't know if we have only OT, only IT or mixed hit content, figure
  // it out
  if (TrackType::Unknown == hittype) {
    // see if we have an OT ot IT only track
    const unsigned nTot = std::count_if(track.coordBegin(),
	track.coordEnd(), [] (const PatFwdHit* h) {
	return h->isSelected(); });
    const unsigned nOT = std::count_if(track.coordBegin(),
	track.coordEnd(), [] (const PatFwdHit* h) {
	return h->isSelected() && h->isOT(); });
    if (nTot == nOT) return fitTrack<TrackType::OTOnly>(track, maxChi2, minPlanes, xOnly, forceDebug);
    else if (0 == nOT) return fitTrack<TrackType::ITOnly>(track, maxChi2, minPlanes, xOnly, forceDebug);
    else return fitTrack<TrackType::Mixed>(track, maxChi2, minPlanes, xOnly, forceDebug);
  } else {
    const bool isDebug = msgLevel( MSG::DEBUG ) || forceDebug;

    //== get enough planes fired
    if ( minPlanes > track.nPlanes() ) return false;

    if ( isDebug ) info() << "+++ Track fit, planeCount " << track.nPlanes()
      << " size " << track.coordEnd() - track.coordBegin() << endmsg;

    if (TrackType::OTOnly == hittype || TrackType::Mixed == hittype) {
      // special treatment for OT hits: try to fix ambiguities
      // (the for loop is a safety harness, in normal operation, the body will
      // execute only once!)
      workspace wksp;
      for (auto it = track.coordBegin(); track.coordEnd() > it;
	  it += wksp.size()) {
	// get selected OT hits
	auto range = as_range(std::begin(wksp),
	    std::copy_if(it, std::min(it + wksp.size(), track.coordEnd()),
	      std::begin(wksp), [] (const PatFwdHit* hit) {
	      return hit->isSelected() && hit->isOT();
	      }));
	// stop if no OT hits
	if (std::end(range) == std::begin(range)) continue;
	// reset ambiguities for OT hits
	for (auto hit: range) hit->setRlAmb(0);
	// resolve ambiguities from pitch residuals where possible
	if (m_ambigFromPitchResiduals) resAmbFromPitchRes(track, range);
	if (m_ambigFromLargestDrift) resAmbFromLargestDrift(track, range, isDebug);
      }
    }
    if (xOnly) {
      if (TrackType::OTOnly == hittype) {
	XFit<true, 1, false, false, false, HitType::OT> xfit(track, track.m_coords);
	if (!xfit) return false;
	else return removeHitsWhileChi2TooLarge<
	  XFit<false, 10, false, false, true, HitType::OT>, true>(
	      track, maxChi2, minPlanes, isDebug);
      } else if (TrackType::ITOnly == hittype) {
	XFit<true, 1, false, false, true, HitType::IT> xfit(track, track.m_coords);
	if (!xfit) return false;
	else return removeHitsWhileChi2TooLarge<
	  XFit<false, 10, false, false, true, HitType::IT>, true>(
	      track, maxChi2, minPlanes, isDebug);
      } else { // Mixed
	XFit<true, 1, false, false, false> xfit(track, track.m_coords);
	if (!xfit) return false;
	else return removeHitsWhileChi2TooLarge<XFit<false, 10>, true>(
	    track, maxChi2, minPlanes, isDebug);
      }
    } else {
      track.updateHits();
      if (TrackType::OTOnly == hittype) {
	XYFit<true, 1, false, HitType::OT> xyfit(track, track.m_coords);
	if (!xyfit) return false;
	else return removeHitsWhileChi2TooLarge<
	  XYFit<false, 10, true, HitType::OT>, false>(
	      track, maxChi2, minPlanes, isDebug);
      } else if (TrackType::ITOnly == hittype) {
	XYFit<true, 1, false, HitType::IT> xyfit(track, track.m_coords);
	if (!xyfit) return false;
	else return removeHitsWhileChi2TooLarge<
	  XYFit<false, 10, true, HitType::IT>, false>(
	      track, maxChi2, minPlanes, isDebug);
      } else {
	XYFit<true, 1, false> xyfit(track, track.m_coords);
	if (!xyfit) return false;
	else return removeHitsWhileChi2TooLarge<XYFit<false, 10>, false>(
	    track, maxChi2, minPlanes, isDebug);
      }
    }
  }
  // should never get here
  return false;
}

//=========================================================================
//  Find the best RL solution using the hits with highest
//  drift distance in the 3 stations.
//=========================================================================
template <class Range>
bool PatSeedTool::resAmbFromLargestDrift (
		PatSeedTrack& track, Range othits, bool forceDebug ) const
{
  // we use builtin arrays because we know there are three stations, and
  // the cost of allocating memory dynamically for std::vector is too
  // high
  othits.second = std::remove_if(std::begin(othits), std::end(othits),
      [] (const PatFwdHit* hit) { return !hit->hit()->isX(); });
  std::array<double, 3> largestDrift = { { 0., 0., 0. } };
  std::array<PatFwdHit*, 3> seeds = { { nullptr, nullptr, nullptr } };
  for( PatFwdHit* hit: othits ) {
    const int sta = hit->hit()->station();
    if ( !seeds[sta] ) seeds[sta] = hit;
    if ( hit->rlAmb() ) {
      // if we have a solution from pitch residulas, we forcibly use it
      seeds[sta] = hit;
      largestDrift[sta] = 100.;
      continue;
    }
    if ( largestDrift[sta] < std::abs(hit->driftDistance()) ) {
      largestDrift[sta] = std::abs(hit->driftDistance());
      seeds[sta] = hit;
    }
  }
  if (100. == largestDrift[0]) largestDrift[0] = 0.;
  if (100. == largestDrift[1]) largestDrift[1] = 0.;
  if (100. == largestDrift[2]) largestDrift[2] = 0.;
  // make sure we can solve the fit equations below: we have at least three
  // hits for three parameters
  if (seeds[0] && seeds[1] && seeds[2]) {
    // twiddle the ambiguities of the three seed hits to find best combination
    double bestChi2 = 1e10;
    int bestMask    = 0;
    for ( unsigned mask = 8; mask--; ) {
      if (largestDrift[0] < 0.1 && mask & 1) continue;
      if (largestDrift[1] < 0.1 && mask & 2) continue;
      if (largestDrift[2] < 0.1 && mask & 4) continue;
      // fix ambiguities for current combination
      if (largestDrift[0] >= 0.1) seeds[0]->setRlAmb( ((mask << 1) & 2) - 1 );
      if (largestDrift[1] >= 0.1) seeds[1]->setRlAmb( (mask & 2) - 1 );
      if (largestDrift[2] >= 0.1) seeds[2]->setRlAmb( ((mask >> 1) & 2) - 1 );

      XFit<true, 1, true, false, false> fit(track, track.m_coords);
      if (!fit) return false;

      // determine chi^2 for current combination
      const double totChi2 = fit.chi2(track, track.m_coords);
      if ( totChi2 < bestChi2 ) {
	// if better than previous best, save
	bestChi2 = totChi2;
	bestMask = mask;
      }
      if ( forceDebug )
	info() << format( "     mask %2d chi2 %10.2f", mask, totChi2) << endmsg;
    }
    // fix ambiguities to best combination
    if (largestDrift[0] >= 0.1) seeds[0]->setRlAmb( ((bestMask << 1) & 2) - 1 );
    if (largestDrift[1] >= 0.1) seeds[1]->setRlAmb( (bestMask & 2) - 1 );
    if (largestDrift[2] >= 0.1) seeds[2]->setRlAmb( ((bestMask >> 1) & 2) - 1 );
  }
  return true;
}

//=============================================================================

template <class Range>
void PatSeedTool::resAmbFromPitchRes(PatSeedTrack& tr, Range hits) const
{
  // ignore hits which have their ambiguities set from elsewhere
  hits.second = std::partition(std::begin(hits), std::end(hits),
      [] (const PatFwdHit* hit) { return 0 == hit->rlAmb(); });
  // stop if nothing to do
  if (hits.second - hits.first < 2) return;
  // sort such that hits in the same module stay together and are ordered by
  // increasing x - this brings together neighbouring straws
  std::sort(std::begin(hits), std::end(hits),
      [] (const PatFwdHit* hit1, const PatFwdHit* hit2) {
        if (hit1->planeCode() < hit2->planeCode()) return true;
	else if (hit2->planeCode() < hit1->planeCode()) return false;
        else if (hit1->hit()->xAtYEq0() < hit2->hit()->xAtYEq0()) return true;
        else if (hit2->hit()->xAtYEq0() < hit1->hit()->xAtYEq0()) return false;
        else return hit1->hit()->lhcbID() < hit2->hit()->lhcbID();
      });
  // predicate which checks if two hits are in neighbouring straws in
  // different monolayers
  const auto areNeighbours = [] (const PatFwdHit* h1, const PatFwdHit* h2) {
    // require hits in same module
    const LHCb::OTChannelID ot1(h1->hit()->lhcbID().otID());
    const LHCb::OTChannelID ot2(h2->hit()->lhcbID().otID());
    if (ot1.uniqueModule() != ot2.uniqueModule()) return false;
    // check in detail: distance in z at y = 0 must be compatible with straws
    // in different monolayers and distance in x must be compatible with the
    // pitch
    const auto dz = std::abs(h1->hit()->zAtYEq0() - h2->hit()->zAtYEq0());
    const auto dx = std::abs(h1->hit()->xAtYEq0() - h2->hit()->xAtYEq0());
    typedef decltype(dz) F;
    const F dzmax = F(13) / F(2), dzmin = F(9) / F(2);
    const F dxmax = F( 7) / F(2), dxmin = F(3) / F(2);
    return dzmin <= dz && dz <= dzmax && dxmin <= dx && dx <= dxmax;
  };
  double lastpitchres = std::numeric_limits<double>::max();
  // loop over pairs of neighbouring straws in the same OT module
  for (auto end = std::end(hits), begin = std::begin(hits);
      end != begin && end != begin + 1; ++begin) {
    PatFwdHit *h1 = *begin, *h2 = *(begin + 1);
    if (!areNeighbours(h1, h2)) {
      lastpitchres = std::numeric_limits<double>::max();
      continue;
    }
    // have two neighbouring hits, calculate pitch residual
    // work out effective slope in module frame
    const auto sinT = h1->hit()->sinT(), cosT = h1->hit()->cosT();
    typedef decltype(sinT) F;
    const auto z0 = (h1->hit()->zAtYEq0() + h2->hit()->zAtYEq0()) / F(2);
    const auto txeff = tr.xSlope(z0) * cosT + tr.ySlope(z0) * sinT;
    // get 2D wire distance vector in module coordinates
    const auto dx = (h1->hit()->xAtYEq0() - h2->hit()->xAtYEq0()) * cosT;
    const auto dzdy = h1->hit()->dzDy();
    const auto dz = (h1->hit()->zAtYEq0() - h2->hit()->zAtYEq0()) /
      std::sqrt(F(1) + dzdy * dzdy);
    // calculate effective pitch (i.e. pitch seen by tr)
    const auto scale = (dx * txeff + dz) / (F(1) + txeff * txeff);
    const auto eprx = dx - scale * txeff, eprz = dz - scale;
    const auto epr = std::sqrt(eprx * eprx + eprz * eprz);
    // get radii
    const auto r1 = h1->driftDistance(), r2 = h2->driftDistance();
    // calculate pitch residual (convert back from effective pitch)
    // case 1: track passes between hits, case 2: not case 1
    const auto pr1 = std::abs(dx / epr) * (epr - r1 - r2);
    const auto pr2 = std::abs(dx / epr) * (epr - std::abs(r1 - r2));
    if (std::min(std::abs(pr1), std::abs(pr2)) > F(3)/F(2)) {
      // pitch residual too large to be believable (3/2 mm is a bit above 5
      // times the OT resolution), refuse to resolve ambiguities for this hit
      // pair
      lastpitchres = std::numeric_limits<double>::max();
      continue;
    }
    if (std::abs(pr1) <= std::abs(pr2)) {
      // set ambiguities accordingly
      HitPairAmbSetter<false>::set(h1, h2, (dx > F(0)) ? -1 : +1, pr1 < lastpitchres);
      lastpitchres = pr1;
    } else {
      // try to figure out ambiguity by comparing slope estimate obtained
      // from drift radii (assuming hits do not pass in between the wires)
      // to the (well measured) effective slope
      //
      // as the slopes are usually big, we have to correct for the fact that
      // the radius is no longer in x direction (using the measured txeff)
      const auto corr = std::sqrt(F(1) + txeff * txeff);
      const auto dslplus = (dx + (r1 - r2) * corr) - txeff * dz;
      const auto dslminus = (dx - (r1 - r2) * corr) - txeff * dz;
      HitPairAmbSetter<true>::set(h1, h2,
	  (std::abs(dslplus) < std::abs(dslminus)) ? +1 : -1, pr2 < lastpitchres);
      lastpitchres = pr2;
    }
  }
}

template <PatSeedTool::TrackType hittype>
bool PatSeedTool::refitStub(PatSeedTrack& track, double arrow) const
{
  if (TrackType::Unknown == hittype) {
    const unsigned nTot = std::count_if(track.coordBegin(),
	track.coordEnd(), [] (const PatFwdHit* h) {
	return h->isSelected(); });
    const unsigned nOT = std::count_if(track.coordBegin(),
	track.coordEnd(), [] (const PatFwdHit* h) {
	return h->isSelected() && h->isOT(); });
    if (nTot == nOT) return refitStub<TrackType::OTOnly>(track, arrow);
    else if (0 == nOT) return refitStub<TrackType::ITOnly>(track, arrow);
    else return refitStub<TrackType::Mixed>(track, arrow);
  } else {
    if (TrackType::OTOnly == hittype || TrackType::Mixed == hittype) {
      const bool isDebug = msgLevel( MSG::DEBUG );
      // special treatment for OT hits: try to fix ambiguities
      // (the for loop is a safety harness, in normal operation, the body will
      // execute only once!)
      workspace wksp;
      for (auto it = track.coordBegin(); track.coordEnd() > it;
	  it += wksp.size()) {
	// get selected OT hits
	auto range = as_range(std::begin(wksp),
	    std::copy_if(it, std::min(it + wksp.size(), track.coordEnd()),
	      std::begin(wksp), [] (const PatFwdHit* hit) {
	      return hit->isSelected() && hit->isOT();
	      }));
	// stop if no OT hits
	if (std::end(range) == std::begin(range)) continue;
	// reset ambiguities for OT hits
	for (auto hit: range) hit->setRlAmb(0);
	// resolve ambiguities from pitch residuals where possible
	if (m_ambigFromPitchResiduals) resAmbFromPitchRes(track, range);
	if (m_ambigFromLargestDrift) resAmbFromLargestDrift(track, range, isDebug);
      }
    }
    /// we have to pass a variable into the fit, so swap it for the track's chi^2
    struct TrackFiddler {
      PatSeedTrack& m_track;
      const double m_chi2saved;
      TrackFiddler(PatSeedTrack& track, double arrow) :
	m_track(track), m_chi2saved(track.chi2())
      { track.setChi2(arrow); }
      ~TrackFiddler() { m_track.setChi2(m_chi2saved); }
    } guard(track, -arrow);

    // do the fit...
    if (TrackType::OTOnly == hittype) {
      StubFit<false, 10, true, HitType::OT> fit(track, track.m_coords);
      return fit;
    } else if (TrackType::ITOnly == hittype) {
      StubFit<false, 10, true, HitType::IT> fit(track, track.m_coords);
      return fit;
    } else { // Mixed
      StubFit<false, 10> fit(track, track.m_coords);
      return fit;
    }
  }
}

void PatSeedTool::printTCoord( MsgStream& msg,
    const PatSeedTrack& track, const PatFwdHit* hit ) const
{
  double dist = track.distance( hit );
  double chi2 = dist*dist* hit->hit()->weight();
  msg << "  Hit st " << hit->hit()->station() << " lay " << hit->hit()->layer()
    << " region " << hit->hit()->region()
    << format( " code%3d z %7.1f distWire%7.2f drift%5.2f dist%8.3f rl%2d Chi2 %8.3f",
	hit->planeCode(), hit->z(), hit->x() - track.xAtZ( hit->z() ),
	hit->driftDistance(), dist, hit->rlAmb(), chi2 );
}

// force instantiations
template
bool PatSeedTool::fitTrack<PatSeedTool::TrackType::OTOnly>(PatSeedTrack& track,
    double maxChi2, unsigned minPlanes, bool xOnly, bool forceDebug ) const;
template
bool PatSeedTool::fitTrack<PatSeedTool::TrackType::ITOnly>(PatSeedTrack& track,
    double maxChi2, unsigned minPlanes, bool xOnly, bool forceDebug ) const;
template
bool PatSeedTool::fitTrack<PatSeedTool::TrackType::Mixed>(PatSeedTrack& track,
    double maxChi2, unsigned minPlanes, bool xOnly, bool forceDebug ) const;
template
bool PatSeedTool::fitTrack<PatSeedTool::TrackType::Unknown>(PatSeedTrack& track,
    double maxChi2, unsigned minPlanes, bool xOnly, bool forceDebug ) const;
template
bool PatSeedTool::refitStub<PatSeedTool::TrackType::OTOnly>(PatSeedTrack& track, double arrow) const;
template
bool PatSeedTool::refitStub<PatSeedTool::TrackType::ITOnly>(PatSeedTrack& track, double arrow) const;
template
bool PatSeedTool::refitStub<PatSeedTool::TrackType::Mixed>(PatSeedTrack& track, double arrow) const;
template
bool PatSeedTool::refitStub<PatSeedTool::TrackType::Unknown>(PatSeedTrack& track, double arrow) const;

// vim:shiftwidth=2:tw=78
