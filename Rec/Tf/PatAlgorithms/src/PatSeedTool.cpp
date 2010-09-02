// $Id: PatSeedTool.cpp,v 1.15 2009-11-26 18:00:48 mschille Exp $
// Include files

#include <cmath>

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

// from boost
#include "boost/array.hpp"
#include "boost/foreach.hpp"

#include "Math/CholeskyDecomp.h"

// local
#include "PatFwdFitLine.h"
#include "PatFwdFitParabola.h"
#include "PatSeedTool.h"

#include "TfKernel/RegionID.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatSeedTool
//
// 2006-10-23 : Olivier Callot
//-----------------------------------------------------------------------------


// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( PatSeedTool );


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


//=========================================================================
//  Fit a Seed track
//=========================================================================
bool PatSeedTool::fitTrack( PatSeedTrack& track,
    double maxChi2, unsigned minPlanes, bool xOnly, bool forceDebug ) const
{
  bool isDebug = msgLevel( MSG::DEBUG ) || forceDebug;

  //== get enough planes fired
  if ( minPlanes > track.nPlanes() ) return false;

  if ( isDebug ) info() << "+++ Track fit, planeCount " << track.nPlanes()
    << " size " << track.coordEnd() - track.coordBegin() << endmsg;

  if ( !xOnly ) track.updateHits( );

  double highestChi2;

  if (!fitInitialXProjection( track, forceDebug )) return false;
  if (!xOnly)
    if (!fitInitialStereoProjection( track, forceDebug)) return false;

  do {
    if (!xOnly) {
      if (!fitSimultaneousXY(track, forceDebug)) return false;
    } else {
      if (!fitXProjection(track, forceDebug)) return false;
    }
    // work out chi^2 and worst outlier
    highestChi2 = 0.;
    double totChi2 = 0.;
    int nDoF  = -3; // ndf for x layers
    int nDoFS = (!xOnly)?(-2):0; // ndf for stereo layers
    PatFwdHits::iterator worst = track.coordBegin(),
      end = track.coordEnd();
    for (PatFwdHits::iterator itH = worst; end != itH; ++itH) {
      const PatFwdHit* hit = *itH;
      if ( !hit->isSelected() ) continue;
      if ( xOnly && !hit->hit()->isX() ) continue;
      const double chi2 = track.chi2Hit( hit );
      totChi2 += chi2;
      if (hit->hit()->isX()) ++nDoF;
      else ++nDoFS;
      if ( highestChi2 < chi2 )
	highestChi2 = chi2, worst = itH;
    }

    if ( msgLevel( MSG::VERBOSE ) || forceDebug ) {
      MsgStream& msg = info();
      BOOST_FOREACH( const PatFwdHit* hit, track.coords() ) {
	if ( !hit->isSelected() ) continue;
	printTCoord( msg, track, hit );
	if ( *worst == hit ) msg << " -- worst";
	msg << endmsg;
      }
    }

    if ( highestChi2 > maxChi2 && maxChi2 > 0. && track.nPlanes() >= minPlanes ) {
      // remove worst outlier if above threshold
      if ((*worst)->hit()->isX()) --nDoF;
      else --nDoFS;
      track.removeCoord( worst );
      if ( isDebug ) info() << " Remove hit and try again " << endmsg;

      if ( minPlanes > track.nPlanes() ||
	  0 > nDoF || (!xOnly && 0 > nDoFS)) {
	if ( isDebug ) info() << " Abandon: Only " << track.nPlanes()
	  << " planes, min " << minPlanes  << " highestChi2 " << highestChi2
	    << " nDoF " << nDoF << " nDoFS " << nDoFS << endmsg;
	return false;
      }
      // and force another iteration
      continue;
    }

    // catch the case where there are too few hits left to determine track
    // parameters with some redundancy (i.e. number of data points <= number
    // of fit parameters)
    if (0 > nDoF || (!xOnly && 0 > nDoFS)) {
      // not enough points to determine fit parameters: return false!
	return false;
    } else if (0 == nDoF && (xOnly || 0 == nDoFS)) {
      // have fit parameters, but ndf = 0: chi^2/ndf is infinity
      track.setChi2( HUGE_VAL );
    } else {
      // all fine
      track.setChi2( totChi2 / double(nDoF + nDoFS) );
    }
  } while ( highestChi2 > maxChi2 && maxChi2 > 0. && track.nPlanes() >= minPlanes);

  if ( isDebug ) info() << ".. OK with " << track.nPlanes() << " planes, min " << minPlanes
    << " highestChi2 " << highestChi2 << endmsg;
  if ( minPlanes > track.nPlanes() ) return false;
  return true;
}

//=========================================================================
//  Parabolic fit to projection.
//=========================================================================
bool PatSeedTool::fitXProjection ( PatSeedTrack& track, bool forceDebug ) const
{
  for ( unsigned kk = 10 ; kk-- ; ) {
    PatFwdFitParabola  parabola;
    int nHits = 0;
    BOOST_FOREACH( const PatFwdHit* hit, track.coords() ) {
      if ( !hit->isSelected() ) continue;
      const bool isOT = hit->hit()->type() == Tf::RegionID::OT;

      const double dist = track.distanceForFit( hit );
      parabola.addPoint( hit->z() - track.z0(),
			 dist,
			 hit->hit()->weight() * (isOT ? (track.cosine() * track.cosine()): 1.0) );
     
      ++nHits;
    }
    if (3 > nHits) return false;
    if (!parabola.solve()) return false;
    if (std::abs(parabola.ax()) > 1e4 || std::abs(parabola.bx()) > 5. ||
	std::abs(parabola.cx()) > 1e-3) return false;
    track.updateParameters( parabola.ax(), parabola.bx(), parabola.cx() );

    if ( msgLevel( MSG::VERBOSE ) || forceDebug  ) {
      info() << format( " dax %10.4f dbx%10.4f dcx %10.4f ",
	  parabola.ax(), 1e3 * parabola.bx(), 1e6 * parabola.cx() ) << endmsg;
    }

    // wait until stable, due to OT.
    if ( std::abs( parabola.ax() ) < 5.e-3 &&
	std::abs( parabola.bx() ) < 5.e-6 &&
	std::abs( parabola.cx() ) < 5.e-9    ) break;
  }
  return true;
}

//=========================================================================
//  Initial fit: Find the best RL solution using the hits with highest
//  drift distance in teh 3 stations.
//=========================================================================
bool PatSeedTool::fitInitialXProjection (
		PatSeedTrack& track, bool forceDebug ) const
{
  // reset ambiguities for OT hits
  unsigned nOT = 0;
  BOOST_FOREACH( PatFwdHit* hit, track.coords() ) {
    if ( !hit->isSelected() ) continue;
    if ( hit->hit()->type() != Tf::RegionID::OT ) continue;
    hit->setRlAmb(0);
    ++nOT;
  }
  if (nOT > 1) {
    // resolve ambiguities from pitch residuals where possible
    if (m_ambigFromPitchResiduals)
      resAmbFromPitchRes(track);
    if (m_ambigFromLargestDrift) {
      // we use builtin arrays because we know there are three stations, and
      // the cost of allocating memory dynamically for std::vector is too
      // high
      boost::array<double, 3> largestDrift = { { 0., 0., 0. } };
      boost::array<PatFwdHit*, 3> seeds = { { 0, 0, 0 } };
      BOOST_FOREACH( PatFwdHit* hit, track.coords() ) {
	if ( !hit->isSelected() ) continue;
	if ( !hit->hit()->isX() ) continue;
	const int sta = hit->hit()->station();
	if ( !seeds[sta] ) seeds[sta] = hit;
	if ( hit->hit()->type() != Tf::RegionID::OT ) continue;
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

	  // get track parameters for current combination
	  PatFwdFitParabola  parabola;
	  BOOST_FOREACH( const PatFwdHit* hit, seeds )
	    parabola.addPoint( hit->z() - track.z0(),
		track.distanceWithRL( hit ), hit->hit()->weight() );
	  if (!parabola.solve()) return false;
	  track.updateParameters( parabola.ax(), parabola.bx(), parabola.cx() );

	  // determine chi^2 for current combination
	  double totChi2 = 0.;
	  BOOST_FOREACH( const PatFwdHit* hit, track.coords() ) {
	    if ( !hit->isSelected() ) continue;
	    if ( !hit->hit()->isX() ) continue;
	    totChi2 += track.chi2Hit( hit );
	  }
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
    }
  }
  // work out final track parameters
  PatFwdFitParabola parabola;
  BOOST_FOREACH( const PatFwdHit* hit, track.coords() ) {
    if ( !hit->isSelected() ) continue;
    if ( !hit->hit()->isX() ) continue;
    const bool isOT = hit->hit()->type() == Tf::RegionID::OT;
    parabola.addPoint( hit->z() - track.z0(),
	track.distanceWithRL( hit ), hit->hit()->weight() *
	(isOT ? (track.cosine() * track.cosine()) : 1.0) );
  }
  if (!parabola.solve()) return false;
  track.updateParameters( parabola.ax(), parabola.bx(), parabola.cx() );
  return true;
}

//=========================================================================
//  Initial fit: Use the RL flags set in fitLineInY.
//=========================================================================
bool PatSeedTool::fitInitialStereoProjection (
		PatSeedTrack& track, bool forceDebug ) const
{
  int nStereo = 0;
  PatFwdFitLine firstLine;
  BOOST_FOREACH( const PatFwdHit* hit, track.coords() ) {
    if ( hit->hit()->isX() ) continue;
    if ( !hit->isSelected() ) continue;
    const bool isOT = hit->hit()->type() == Tf::RegionID::OT;
    firstLine.addPoint( hit->z(),
	-track.distanceWithRL( hit ) / hit->hit()->dxDy(),
       	hit->hit()->weight() * (isOT? (track.cosine() * track.cosine()) : 1.) );
    ++nStereo;
  }

  if ( 2 > nStereo ) return false;
  if (!firstLine.solve()) return false;
  if ( forceDebug ) info() << "    day " << firstLine.ax() << " dby "
    << firstLine.bx() << " initial" << endmsg;
  track.updateYParameters( firstLine.ax(), firstLine.bx() );
  return true;
}
//=============================================================================

bool PatSeedTool::isNeighbour(const PatFwdHit* h1, const PatFwdHit* h2) const
{
  // skip IT hits
  if (h2->hit()->type() != Tf::RegionID::OT) return false;
  // skip hits with predetermined ambiguity
  if (h2->rlAmb()) return false;
  if (!h2->isSelected()) return false;
  const LHCb::OTChannelID ot1(h1->hit()->lhcbID().otID());
  const LHCb::OTChannelID ot2(h2->hit()->lhcbID().otID());
  // require hits in same module
  if (ot1.uniqueModule() != ot2.uniqueModule()) return false;
  // check in detail: distance in z at y = 0 must be compatible with straws
  // in different monolayers and distance in x must be compatible with the
  // pitch
  const double dz = std::abs(h1->hit()->zAtYEq0() - h2->hit()->zAtYEq0());
  if (dz > 6.5 || dz < 4.5) return false;
  const double dx = std::abs(h1->hit()->xAtYEq0() - h2->hit()->xAtYEq0());
  if (dx > 3.5 || dx < 1.5) return false;
  return true;
}

void PatSeedTool::resAmbFromPitchRes(PatSeedTrack& tr) const
{
  PatFwdHits::const_iterator beg(tr.coordBegin()), end(tr.coordEnd());
  for (PatFwdHits::const_iterator it = beg; end != it; ++it) {
    // skip IT hits
    if ((*it)->hit()->type() != Tf::RegionID::OT) continue;
    // skip hits with predetermined ambiguity
    if ((*it)->rlAmb()) continue;
    if (!(*it)->isSelected()) continue;
    for (PatFwdHits::const_iterator jt = it + 1; end != jt; ++jt ) {
      if (!isNeighbour(*it, *jt)) continue;
      // have two neighbouring hits, calculate pitch residual
      PatFwdHit *h1 = *it, *h2 = *jt;
      // get 2D wire distance vector in module coordinates
      const double sinT = h1->hit()->sinT(), cosT = h1->hit()->cosT();
      const double dzdy = h1->hit()->dzDy();
      const double dx = (h1->hit()->xAtYEq0() - h2->hit()->xAtYEq0()) * cosT;
      const double dz = (h1->hit()->zAtYEq0() - h2->hit()->zAtYEq0()) /
	std::sqrt(1. + dzdy * dzdy);
      // work out effective slope in module frame
      const double z0 = 0.5 * (h1->hit()->zAtYEq0() + h2->hit()->zAtYEq0());
      const double txeff = tr.xSlope(z0) * cosT + tr.ySlope(z0) * sinT;
      // calculate effective pitch (i.e. pitch seen by tr)
      const double scale = (dx * txeff + dz) / (1. + txeff * txeff);
      const double eprx = dx - scale * txeff, eprz = dz - scale;
      const double epr = std::sqrt(eprx * eprx + eprz * eprz);
      // get radii
      const double r1 = h1->driftDistance(), r2 = h2->driftDistance();
      // calculate pitch residual (convert back from effective pitch)
      // case 1: track passes between hits, case 2: not case 1
      const double pr1 = std::abs(dx / epr) * (epr - r1 - r2);
      const double pr2 = std::abs(dx / epr) * (epr - std::abs(r1 - r2));
      if (pr1 * pr1 <= pr2 * pr2) {
	// set ambiguities accordingly
	if (dx > 0.) {
	  h1->setRlAmb(-1);
	  h2->setRlAmb(+1);
	  break;
	} else {
	  h1->setRlAmb(+1);
	  h2->setRlAmb(-1);
	  break;
	}
      } else {
	// try to figure out ambiguity by comparing slope estimate obtained
	// from drift radii (assuming hits do not pass in between the wires)
	// to the (well measured) effective slope
	//
	// as the slopes are usually big, we have to correct for the fact that
	// the radius is no longer in x direction (using the measured txeff)
	const double corr = std::sqrt(1. + txeff * txeff);
	const double dslplus = (dx + (r1 - r2) * corr) - txeff * dz;
	const double dslminus = (dx - (r1 - r2) * corr) - txeff * dz;
	if (dslplus * dslplus < dslminus * dslminus) {
	  h1->setRlAmb(+1);
	  h2->setRlAmb(+1);
	  break;
	} else {
	  h1->setRlAmb(-1);
	  h2->setRlAmb(-1);
	  break;
	}
      }
    }
  }
}

bool PatSeedTool::fitSimultaneousXY(PatSeedTrack& track, bool forceDebug) const
{
  const bool debug = msgLevel(MSG::VERBOSE) || forceDebug;
  // up to 10 iterations of simultaneous fit
  double mat[20], *rhs = mat + 15, z0;
  unsigned nHitsX, nHitsStereo;
  for ( unsigned kk = 10 ; kk-- ; ) {
    PatFwdFitParabola  parabola;
    nHitsX = 0, nHitsStereo = 0;
    track.getParameters(z0, rhs[0], rhs[0], rhs[1], rhs[2], rhs[0], rhs[0]);
    const double dRatio = (std::abs(rhs[1]) > 1e-42) ? (1e3 * rhs[2] / rhs[1]) : 0.;
    std::fill(mat, mat + 20, 0.);
    BOOST_FOREACH( const PatFwdHit* hit, track.coords() ) {
      if ( !hit->isSelected() ) continue;
      if (hit->hit()->isX()) ++nHitsX;
      else ++nHitsStereo;
      const bool isOT = hit->hit()->type() == Tf::RegionID::OT;
      // prepare auxiliary variables
      const double dist = track.distanceForFit( hit );
      double w = hit->hit()->weight();
      if (isOT) w *= track.cosine() * track.cosine();
      const double dxdy = hit->hit()->dxDy();
      const double dz = 1e-3 * (hit->z() - z0); // better numerical stability
      const double wdz = w * dz;
      const double eta = dz * dz * (1. + dz * dRatio);
      const double weta = w * eta;
      const double wdxdy = w * dxdy;
      const double wdxdydz = wdxdy * dz;
      // fill matrix
      mat[0] += w;
      mat[1] += wdz;
      mat[2] += wdz * dz;
      mat[3] += weta;
      mat[4] += weta * dz;
      mat[5] += weta * eta;
      mat[6] -= wdxdy;
      mat[7] -= wdxdydz;
      mat[8] -= wdxdy * eta;
      mat[9] += wdxdy * dxdy;
      mat[10] -= wdxdydz;
      mat[11] -= wdxdydz * dz;
      mat[12] -= wdxdydz * eta;
      mat[13] += wdxdydz * dxdy;
      mat[14] += wdxdydz * dz * dxdy;
      // fill right hand side
      rhs[0] += w * dist;
      rhs[1] += wdz * dist;
      rhs[2] += weta * dist;
      rhs[3] -= wdxdy * dist;
      rhs[4] -= wdxdydz * dist;
    }
    // protect against too few hits for unique solution
    if (nHitsX < 3 || nHitsStereo < 2) return false;
    // decompose matrix, protect against numerical trouble
    ROOT::Math::CholeskyDecomp<double, 5> decomp(mat);
    if (!decomp) return false;
    // solve linear system
    decomp.Solve(rhs);
    // undo dz scaling
    rhs[1] *= 1e-3; rhs[2] *= 1e-6; rhs[4] *= 1e-3;
    // PatSeedTrack y is expanded around z = 0
    rhs[3] -= rhs[4] * z0;
    if (debug) {
      info() << format( " dax %10.4f dbx %10.4f dcx %10.4f day %10.4f dby %10.4f",
	  rhs[0], 1e3 * rhs[1], 1e6 * rhs[2], rhs[3], 1e3 * rhs[4] ) << endmsg;
    }
    // protect against unreasonable track parameter corrections
    if (std::abs(rhs[0]) > 1e4 || std::abs(rhs[1]) > 5. ||
	std::abs(rhs[2]) > 1e-3 || std::abs(rhs[3]) > 1e4 ||
	std::abs(rhs[4]) > 1.) return false;
    // update track parameters
    track.updateParameters(rhs[0], rhs[1], rhs[2]);
    track.updateYParameters(rhs[3], rhs[4]);

    // wait until stable, due to OT.
    if (std::abs(rhs[0]) < 5e-3 && std::abs(rhs[1]) < 5e-6 &&
	std::abs(rhs[2]) < 5e-9 && std::abs(rhs[3]) < 5e-2 &&
	std::abs(rhs[4]) < 5e-6) break;
  }
  return true;
}

bool PatSeedTool::refitStub(PatSeedTrack& track, double dRatio, double arrow) const
{
  unsigned iter = 0;
  double rhs[9], *mat = rhs + 3;
  do {
    const double z0 = track.z0();
    std::fill(rhs, rhs + 9, 0.);
    BOOST_FOREACH( const PatFwdHit* hit, track.coords() ) {
      const double z = hit->z();
      const double w = hit->hit()->weight();
      const double dz = 1e-3 * (z - z0);
      const double dx = hit->x() - track.xAtZ(z);
      const double eta = - hit->hit()->dxDy() * z / z0;
      rhs[0] += w * eta * dx;
      rhs[1] += w * dx;
      rhs[2] += w * dz * dx;
      mat[0] += w * eta * eta;
      mat[1] += w * eta;
      mat[2] += w;
      mat[3] += w * eta * dz;
      mat[4] += w * dz;
      mat[5] += w * dz * dz;
    }
    ROOT::Math::CholeskyDecomp<double, 3> decomp(mat);
    // protect against singular matrix and no convergence
    const bool ok = decomp && (++iter < 10);
    if (!ok) return false;
    // solve linear system
    decomp.Solve(rhs);
    rhs[2] *= 1e-3;
    // protect against unreasonably large changes in parameters
    if (std::abs(rhs[0]) > 1e3 || std::abs(rhs[1]) > 1e3 || std::abs(rhs[2]) > 5.0)
      return false;
    // update track parameters
    track.updateParameters(rhs[1], rhs[2],
	-arrow * (rhs[1] - z0 * rhs[2]),
	-arrow * (rhs[1] - z0 * rhs[2]) * dRatio);
    track.updateYParameters(0., rhs[0] / z0);
    // until track parameters are stable
  } while (std::abs(rhs[1]) > 1e-3 || std::abs(rhs[2]) > 1e-6 ||
      std::abs(rhs[0]) > 1e-5);
  return true;
}

// vim:shiftwidth=2:tw=78
