// $Id: PatSeedTool.cpp,v 1.3 2008-01-27 23:02:00 mschille Exp $
// Include files

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

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
                            double maxChi2, int minPlanes, bool xOnly, bool forceDebug ) {
  bool isDebug = msgLevel( MSG::DEBUG ) || forceDebug;
  PatFwdHits::iterator itH;

  //== get enough planes fired
  if ( minPlanes > track.nPlanes() ) return false;

  if ( isDebug ) info() << "+++ Track fit, planeCount " << track.nPlanes()
                        << " size " << track.coordEnd() - track.coordBegin() << endreq;

  if ( !xOnly ) track.updateHits( );
  bool first = true;

  double highestChi2 = 10*maxChi2;
  bool   ignoreX = !xOnly;   // make sense only if some stereo...

  fitInitialXProjection( track, forceDebug );

  while ( highestChi2 > maxChi2 ) {
    //== Improve X parameterisation
    fitXProjection( track, isDebug );
    bool hasStereo = false;
    if ( !xOnly ) {
      if ( first ) {
        fitInitialStereoProjection( track, forceDebug );
        first = false;
      }

      for ( unsigned int kk = 0; 10 > kk; ++kk ) {
        PatFwdFitLine line;
        for ( itH = track.coordBegin(); track.coordEnd() != itH ; ++itH ) {
          PatFwdHit* hit = *itH;
          if ( !hit->isSelected() ) continue;
          if ( hit->hit()->isX() ) continue;
          double z     = hit->z();
          double dist  = - track.distanceForFit( hit ) / hit->hit()->dxDy();
          double w     = hit->hit()->weight();
          line.addPoint( z, dist, w );
          hasStereo = true;
        }
        if ( !hasStereo ) {
          ignoreX = false;
          break;
        }
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
    double nDoF = -3.;
    if ( hasStereo ) nDoF -= 2.;

    for ( itH = track.coordBegin(); track.coordEnd() != itH ; ++itH ) {
      PatFwdHit* hit = *itH;
      if ( !hit->isSelected() ) continue;
      if ( ignoreX && hit->hit()->isX() ) continue;
      double chi2 = track.chi2Hit( hit );
      nDoF += 1.;
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
      if ( isDebug ) info() << " Remove hit and try again " << endreq;

      if ( minPlanes > track.nPlanes() ) {
        if ( isDebug ) info() << " Abandon: Only " << track.nPlanes() << " planes, min " << minPlanes
                              << " highestChi2 " << highestChi2 << endreq;
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
    track.setChi2( totChi2 / nDoF );
  }
  if ( isDebug ) info() << ".. OK with " << track.nPlanes() << " planes, min " << minPlanes
                        << " highestChi2 " << highestChi2 << endreq;
  if ( minPlanes > track.nPlanes() ) return false;
  return true;

}
//=========================================================================
//  Parabolic fit to projection.
//=========================================================================
void PatSeedTool::fitXProjection ( PatSeedTrack& track, bool forceDebug ) {

  for ( unsigned int kk = 0 ; 10 > kk ; ++kk ) {
    PatFwdFitParabola  parabola;
    for ( PatFwdHits::const_iterator itH = track.coordBegin(); 
          track.coordEnd()!= itH ; ++itH ) {
      PatFwdHit* hit = *itH;
      if ( !hit->isSelected() ) continue;
      double dist  = track.distanceForFit( hit );
      double dz    = hit->z() - track.z0();
      double w     = hit->hit()->weight();
      parabola.addPoint( dz, dist, w );
    }
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
}

//=========================================================================
//  Initial fit: Find the best RL solution using the hits with highest
//  drift distance in teh 3 stations.
//=========================================================================
void PatSeedTool::fitInitialXProjection ( PatSeedTrack& track, bool forceDebug ) {

  PatFwdHits::const_iterator itH, itH1;

  std::vector<double> largestDrift( 3, 0. );
  PatFwdHits seeds(3);
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
  if ( 0.1 > largestDrift[0] || 0.1 > largestDrift[1] || 0.1 > largestDrift[2] ) return;

  double bestChi2 = 1.e10;
  int bestMask    = 0;
  for ( int mask = 0; 8 > mask; ++mask ) {
    seeds[0]->setRlAmb( 2*(mask&1)-1 );
    seeds[1]->setRlAmb( 2*((mask>>1)&1) -1 );
    seeds[2]->setRlAmb( 2*((mask>>2)&1) -1 );
    PatFwdFitParabola  parabola;
    for ( itH1 = seeds.begin(); seeds.end()!= itH1 ; ++itH1 ) {
      PatFwdHit* hit = *itH1;
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

    for ( itH1 = track.coordBegin(); track.coordEnd() != itH1 ; ++itH1 ) {
      PatFwdHit* hit = *itH1;
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
  for ( itH1 = track.coordBegin(); track.coordEnd() != itH1 ; ++itH1 ) {
    PatFwdHit* hit = *itH1;
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
void PatSeedTool::fitInitialStereoProjection ( PatSeedTrack& track, bool forceDebug ) {
  PatFwdHits::const_iterator itH, itH1;

  for ( itH = track.coordBegin(); track.coordEnd()!= itH ; ++itH ) {
    PatFwdHit* hit = *itH;
    if ( !hit->isSelected() ) continue;
    if ( hit->hit()->isX() ) continue;
  }

  PatFwdFitLine firstLine;
  for ( itH = track.coordBegin(); track.coordEnd() != itH ; ++itH ) {
    PatFwdHit* hit = *itH;
    if ( !hit->isSelected() ) continue;
    if ( hit->hit()->isX() ) continue;
    double z     = hit->z();
    double dist  = track.distanceWithRL( hit ) / hit->hit()->dxDy();
    double w     = hit->hit()->weight();
    firstLine.addPoint( z, dist, w );
  }

  firstLine.solve();
  double day = firstLine.ax();
  double dby = firstLine.bx();
  if ( forceDebug ) info() << "    day " << day << " dby " << dby << " initial" << endreq;
  track.updateYParameters( day, dby );
}
//=============================================================================
