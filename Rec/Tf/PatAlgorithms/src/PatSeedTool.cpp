// $Id: PatSeedTool.cpp,v 1.5 2008-06-04 15:33:48 mschille Exp $
// Include files

#include <cmath>

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

// from boost
#include "boost/array.hpp"

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
    double maxChi2, int minPlanes, bool xOnly, bool forceDebug ) const
{
  bool isDebug = msgLevel( MSG::DEBUG ) || forceDebug;
  PatFwdHits::iterator itH;

  //== get enough planes fired
  if ( minPlanes > track.nPlanes() ) return false;

  if ( isDebug ) info() << "+++ Track fit, planeCount " << track.nPlanes()
    << " size " << track.coordEnd() - track.coordBegin() << endreq;

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
	for ( itH = track.coordBegin(); track.coordEnd() != itH ; ++itH ) {
	  PatFwdHit* hit = *itH;
	  if ( !hit->isSelected() ) continue;
	  if ( hit->hit()->isX() ) continue;
	  double z     = hit->z();
	  double dist  = - track.distanceForFit( hit ) / hit->hit()->dxDy();
	  double w     = hit->hit()->weight();
	  line.addPoint( z, dist, w );
	  ++nStereo;
	}
	// make sure we can solve below
	if ( 2 > nStereo ) return false;
	line.solve();
	double day = line.ax();
	double dby = line.bx();
	if ( isDebug )
	  info() << "    day " << day << " dby " << dby << endreq;
	if ( fabs( dby ) > 1.0 ) {
	  // if we get a track with stereo hits on it which do not
	  // belong together, the y fit may diverge. the problem about
	  // this is that the hits are shifted depending on their y
	  // coordinate. when the fit diverges, dby may become nan, and
	  // this nan will propagate everywhere, so hit coordinates also
	  // become nan. to avoid this bad case, we stop the fit
	  // iteration - and throw away the track in its entirety
	  if ( isDebug )
	    info() << "    fabs(dby) > 1.0, abandoning track!" << endreq;
	  return false;
	}
	track.updateYParameters( day, dby );
	if ( fabs( day ) < 0.05 && fabs( dby ) < 0.00005 ) break;
      }
    }

    highestChi2 = 0;
    PatFwdHits::iterator worst = track.coordBegin();

    double totChi2 = 0.;
    int nDoF = -3;
    if ( nStereo ) nDoF -= 2;

    for ( itH = track.coordBegin(); track.coordEnd() != itH ; ++itH ) {
      PatFwdHit* hit = *itH;
      if ( !hit->isSelected() ) continue;
      if ( ignoreX && hit->hit()->isX() ) continue;
      double chi2 = track.chi2Hit( hit );
      ++nDoF;
      totChi2 += chi2;
      if ( highestChi2 < chi2 ) {
	highestChi2 = chi2;
	worst = itH;
      }
    }
    if ( msgLevel( MSG::VERBOSE ) || forceDebug ) {
      MsgStream& msg = info();
      for ( itH = track.coordBegin(); track.coordEnd() != itH ; ++itH ) {
	PatFwdHit* hit = *itH;
	if ( !hit->isSelected() ) continue;
	printTCoord( msg, track, hit );
	if ( worst == itH ) msg << " -- worst";
	msg << endreq;
      }
    }
    if ( highestChi2 > maxChi2 ) {
      track.removeCoord( worst );
      --nDoF;
      if ( isDebug ) info() << " Remove hit and try again " << endreq;

      if ( minPlanes > track.nPlanes()  || 0 > nDoF ) {
	if ( isDebug ) info() << " Abandon: Only " << track.nPlanes()
	  << " planes, min " << minPlanes
	  << " highestChi2 " << highestChi2  << " nDoF " << nDoF << endreq;
	return false;
      }
    }
    //== If almost there, force one iteration with X fitting, at least...
    if ( highestChi2 < 2 * maxChi2 ) {
      if ( ignoreX ) {
	ignoreX = false;
	highestChi2 = 2.* maxChi2;
      }
    }
    // catch the case where there are too few hits left to determine track
    // parameters with some redundancy (i.e. number of data points <= number
    // of fit parameters)
    if (0 >= nDoF) {
      track.setChi2( HUGE_VAL );
    } else track.setChi2( totChi2 / double(nDoF) );
  } while ( highestChi2 > maxChi2 );
  if ( isDebug ) info() << ".. OK with " << track.nPlanes() << " planes, min " << minPlanes
    << " highestChi2 " << highestChi2 << endreq;
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
    for ( PatFwdHits::const_iterator itH = track.coordBegin(); 
	track.coordEnd()!= itH ; ++itH ) {
      PatFwdHit* hit = *itH;
      if ( !hit->isSelected() ) continue;
      double dist  = track.distanceForFit( hit );
      double dz    = hit->z() - track.z0();
      double w     = hit->hit()->weight();
      parabola.addPoint( dz, dist, w );
      ++nHits;
    }
    if (3 > nHits) return false;
    parabola.solve();
    double dax = parabola.ax();
    double dbx = parabola.bx();
    double dcx = parabola.cx();
    track.updateParameters( dax, dbx, dcx );

    if ( msgLevel( MSG::VERBOSE ) || forceDebug  ) {
      info() << format( " dax %10.4f dbx%10.4f dcx %10.4f ",
	  dax, dbx*1.e3, dcx*1.e6 ) << endreq;
    }

    if ( fabs( dax ) < 5.e-3 &&
	fabs( dbx ) < 5.e-6 &&
	fabs( dcx ) < 5.e-9    ) break;  // wait until stable, due to OT.
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
  PatFwdHits::const_iterator itH;

  // we use builtin arrays because we know there are three stations, and
  // the cost of allocating memory dynamically for std::vector is too
  // high
  boost::array<double, 3> largestDrift = { 0., 0., 0. };
  boost::array<PatFwdHit*, 3> seeds;
  for ( itH = track.coordBegin(); track.coordEnd()!= itH ; ++itH ) {
    PatFwdHit* hit = *itH;
    if ( !hit->isSelected() ) continue;
    if ( !hit->hit()->isX() ) continue;
    if ( hit->hit()->type() == Tf::RegionID::OT ) {
      hit->setRlAmb( 0 );
      int sta = hit->hit()->station();
      if ( largestDrift[sta] < hit->driftDistance() ) {
	largestDrift[sta] =  hit->driftDistance();
	seeds[sta] = hit;
      }
    }
  }
  // note that the next statement makes sure we can solve the fit equations
  // below: we have at least three hits for three parameters
  if ( 0.1 > largestDrift[0] || 0.1 > largestDrift[1]
		  || 0.1 > largestDrift[2] ) return;

  double bestChi2 = 1.e10;
  int bestMask    = 0;
  for ( int mask = 0; 8 > mask; ++mask ) {
    seeds[0]->setRlAmb( 2*(mask&1)-1 );
    seeds[1]->setRlAmb( 2*((mask>>1)&1) -1 );
    seeds[2]->setRlAmb( 2*((mask>>2)&1) -1 );
    PatFwdFitParabola  parabola;
    for ( boost::array<PatFwdHit*, 3>::const_iterator it = seeds.begin();
	seeds.end() != it; ++it ) {
      const PatFwdHit* hit = *it;
      double dist  = track.distanceWithRL( hit );
      double dz    = hit->z() - track.z0();
      double w     = hit->hit()->weight();
      parabola.addPoint( dz, dist, w );
    }
    parabola.solve();
    double dax = parabola.ax();
    double dbx = parabola.bx();
    double dcx = parabola.cx();
    track.updateParameters( dax, dbx, dcx );
    double totChi2 = 0.;

    for ( itH = track.coordBegin(); track.coordEnd() != itH ; ++itH ) {
      PatFwdHit* hit = *itH;
      if ( !hit->isSelected() ) continue;
      if ( !hit->hit()->isX() ) continue;
      totChi2 += track.chi2Hit( hit );
    }
    if ( totChi2 < bestChi2 ) {
      bestChi2 = totChi2;
      bestMask = mask;
    }
    if ( forceDebug ) {
      info() << format( "     mask %2d chi2 %10.2f", mask, totChi2) << endreq;
    }
  }
  seeds[0]->setRlAmb( 2*(bestMask&1)-1 );
  seeds[1]->setRlAmb( 2*((bestMask>>1)&1) -1 );
  seeds[2]->setRlAmb( 2*((bestMask>>2)&1) -1 );

  PatFwdFitParabola  parabola;
  for ( itH = track.coordBegin(); track.coordEnd() != itH ; ++itH ) {
    PatFwdHit* hit = *itH;
    if ( !hit->isSelected() ) continue;
    double dist  = track.distanceWithRL( hit );
    double dz    = hit->z() - track.z0();
    double w     = hit->hit()->weight();
    parabola.addPoint( dz, dist, w );
  }
  parabola.solve();
  double dax = parabola.ax();
  double dbx = parabola.bx();
  double dcx = parabola.cx();
  track.updateParameters( dax, dbx, dcx );
}
//=========================================================================
//  Initial fit: Use the RL flags set in fitLineInY.
//=========================================================================
bool PatSeedTool::fitInitialStereoProjection (
		PatSeedTrack& track, bool forceDebug ) const
{
  int nStereo = 0;
  PatFwdHits::const_iterator itH;
  PatFwdFitLine firstLine;
  for ( itH = track.coordBegin(); track.coordEnd() != itH ; ++itH ) {
    PatFwdHit* hit = *itH;
    if ( !hit->isSelected() ) continue;
    if ( hit->hit()->isX() ) continue;
    double z     = hit->z();
    double dist  = - track.distanceWithRL( hit ) / hit->hit()->dxDy();
    double w     = hit->hit()->weight();
    firstLine.addPoint( z, dist, w );
    ++nStereo;
  }

  if ( 2 > nStereo ) return false;
  firstLine.solve();
  double day = firstLine.ax();
  double dby = firstLine.bx();
  if ( forceDebug )
	  info() << "    day " << day << " dby " << dby << " initial" << endreq;
  track.updateYParameters( day, dby );
  return true;
}
//=============================================================================
