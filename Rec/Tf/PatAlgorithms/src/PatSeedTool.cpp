// $Id: PatSeedTool.cpp,v 1.9 2009-04-20 06:24:33 cattanem Exp $
// Include files

#include <cmath>

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

// from boost
#include "boost/array.hpp"
#include "boost/foreach.hpp"

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
  bool ignoreX = !xOnly;   // make sense only if some stereo...
  bool first = true;

  fitInitialXProjection( track, forceDebug );

  do {
    //== Improve X parameterisation
    if ( !fitXProjection( track, isDebug ) ) return false;
    int nStereo = 0;
    if ( !xOnly ) {
      if ( first ) {
	if ( !fitInitialStereoProjection( track, forceDebug ) ) return false;
	first = false;
      }

      for ( unsigned kk = 10; kk--; ) {
	PatFwdFitLine line;
	nStereo = 0;
	BOOST_FOREACH( const PatFwdHit* hit, track.coords() ) {
	  if ( hit->hit()->isX() ) continue;
	  if ( !hit->isSelected() ) continue;
	  line.addPoint( hit->z(),
	     -track.distanceForFit( hit ) / hit->hit()->dxDy(),
	     hit->hit()->weight() );
	  ++nStereo;
	}
	// make sure we can solve
	if ( 2 > nStereo ) return false;
	line.solve();
	const double day = line.ax(), dby = line.bx();

	if ( isDebug )
	  info() << "    day " << day << " dby " << dby << endmsg;
	if ( fabs( dby ) > 1.0 ) {
	  // if we get a track with stereo hits on it which do not
	  // belong together, the y fit may diverge. the problem about
	  // this is that the hits are shifted depending on their y
	  // coordinate. when the fit diverges, dby may become nan, and
	  // this nan will propagate everywhere, so hit coordinates also
	  // become nan. to avoid this bad case, we stop the fit
	  // iteration - and throw away the track in its entirety
	  if ( isDebug )
	    info() << "    fabs(dby) > 1.0, abandoning track!" << endmsg;
	  return false;
	}
	// update track parameters
	track.updateYParameters( day, dby );
	// break if track parameters become stable
	if ( fabs( day ) < 0.05 && fabs( dby ) < 0.00005 ) break;
      }
    }

    // work out chi^2 and worst outlier
    highestChi2 = 0;
    double totChi2 = 0.;
    int nDoF  = ignoreX?0:(-3); // ndf for x layers
    int nDoFS = nStereo?(-2):0; // ndf for stereo layers
    PatFwdHits::iterator worst = track.coordBegin(),
      end = track.coordEnd();
    for ( PatFwdHits::iterator itH = worst; end != itH; ++itH) {
      const PatFwdHit* hit = *itH;
      if ( ignoreX && hit->hit()->isX() ) continue;
      if ( !hit->isSelected() ) continue;
      const double chi2 = track.chi2Hit( hit );
      totChi2 += chi2;
      hit->hit()->isX()?(++nDoF):(++nDoFS);
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

    if ( highestChi2 > maxChi2 ) {
      // remove worst outlier if above threshold
      (*worst)->hit()->isX()?(--nDoF):(--nDoFS);
      track.removeCoord( worst );
      if ( isDebug ) info() << " Remove hit and try again " << endmsg;

      if ( minPlanes > track.nPlanes() ||
	  (!ignoreX && (0 > nDoF)) || ((0 != nStereo) && (0 > nDoFS))) {
	if ( isDebug ) info() << " Abandon: Only " << track.nPlanes()
	  << " planes, min " << minPlanes  << " highestChi2 " << highestChi2
	    << " nDoF " << nDoF << " nDoFS " << nDoFS << endmsg;
	return false;
      }
    }

    //== If almost there, force one iteration with X fitting, at least...
    if ( highestChi2 < 2. * maxChi2 ) {
      if ( ignoreX ) {
	ignoreX = false;
	// next line triggers another iteration
	highestChi2 = 2. * maxChi2;
      }
    }
    // catch the case where there are too few hits left to determine track
    // parameters with some redundancy (i.e. number of data points <= number
    // of fit parameters)
    if (0 >= (nDoF + nDoFS)) {
      // not enough points to determine fit parameters: return false!
      if ((!ignoreX && (0 > nDoF)) || ((0 != nStereo) && (0 > nDoFS)))
	return false;
      else
	// have fit parameters, but ndf = 0: chi^2/ndf is infinity
	track.setChi2( HUGE_VAL );
    } else
      track.setChi2( totChi2 / double(nDoF + nDoFS) );
  } while ( highestChi2 > maxChi2 );

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
      parabola.addPoint( hit->z() - track.z0(),
	  track.distanceForFit( hit ), hit->hit()->weight() );
      ++nHits;
    }
    if (3 > nHits) return false;
    parabola.solve();
    track.updateParameters( parabola.ax(), parabola.bx(), parabola.cx() );

    if ( msgLevel( MSG::VERBOSE ) || forceDebug  ) {
      info() << format( " dax %10.4f dbx%10.4f dcx %10.4f ",
	  parabola.ax(), 1e3 * parabola.bx(), 1e6 * parabola.cx() ) << endmsg;
    }

    // wait until stable, due to OT.
    if ( fabs( parabola.ax() ) < 5.e-3 &&
	fabs( parabola.bx() ) < 5.e-6 &&
	fabs( parabola.cx() ) < 5.e-9    ) break;
  }
  return true;
}

//=========================================================================
//  Initial fit: Find the best RL solution using the hits with highest
//  drift distance in teh 3 stations.
//=========================================================================
void PatSeedTool::fitInitialXProjection (
		PatSeedTrack& track, bool forceDebug ) const
{
  // we use builtin arrays because we know there are three stations, and
  // the cost of allocating memory dynamically for std::vector is too
  // high
  boost::array<double, 3> largestDrift = {{ 0., 0., 0. }};
  boost::array<PatFwdHit*, 3> seeds;
  BOOST_FOREACH( PatFwdHit* hit, track.coords() ) {
    if ( hit->hit()->type() != Tf::RegionID::OT ) continue;
    if ( !hit->hit()->isX() ) continue;
    if ( !hit->isSelected() ) continue;
    hit->setRlAmb( 0 );
    const int sta = hit->hit()->station();
    if ( largestDrift[sta] < hit->driftDistance() ) {
      largestDrift[sta] = hit->driftDistance();
      seeds[sta] = hit;
    }
  }
  // note that the next statement makes sure we can solve the fit equations
  // below: we have at least three hits for three parameters
  if ( 0.1 > largestDrift[0] || 0.1 > largestDrift[1]
		  || 0.1 > largestDrift[2] ) return;

  // twiddle the ambiguities of the three seed hits to find best combination
  double bestChi2 = 1.e10;
  int bestMask    = 0;
  for ( unsigned mask = 8; mask--; ) {
    // fix ambiguities for current combination
    seeds[0]->setRlAmb( ((mask << 1) & 2) - 1 );
    seeds[1]->setRlAmb( (mask & 2) - 1 );
    seeds[2]->setRlAmb( ((mask >> 1) & 2) - 1 );

    // get track parameters for current combination
    PatFwdFitParabola  parabola;
    BOOST_FOREACH( const PatFwdHit* hit, seeds )
      parabola.addPoint( hit->z() - track.z0(),
	  track.distanceWithRL( hit ), hit->hit()->weight() );
    parabola.solve();
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
  seeds[0]->setRlAmb( ((bestMask << 1) & 2) - 1 );
  seeds[1]->setRlAmb( (bestMask & 2) - 1 );
  seeds[2]->setRlAmb( ((bestMask >> 1) & 2) - 1 );

  // work out final track parameters
  PatFwdFitParabola parabola;
  BOOST_FOREACH( const PatFwdHit* hit, track.coords() ) {
    if ( !hit->isSelected() ) continue;
    parabola.addPoint( hit->z() - track.z0(),
	track.distanceWithRL( hit ), hit->hit()->weight() );
  }
  parabola.solve();
  track.updateParameters( parabola.ax(), parabola.bx(), parabola.cx() );
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
    firstLine.addPoint( hit->z(),
	-track.distanceWithRL( hit ) / hit->hit()->dxDy(),
       	hit->hit()->weight() );
    ++nStereo;
  }

  if ( 2 > nStereo ) return false;
  firstLine.solve();
  if ( forceDebug ) info() << "    day " << firstLine.ax() << " dby "
    << firstLine.bx() << " initial" << endmsg;
  track.updateYParameters( firstLine.ax(), firstLine.bx() );
  return true;
}
//=============================================================================

// vim:shiftwidth=2:tw=78
