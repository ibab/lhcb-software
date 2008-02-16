
//-----------------------------------------------------------------------------
/** @file RichTrackSegment.cpp
 *
 *  Implementation file for class : RichTrackSegment
 *
 *  CVS Log :-
 *  $Id: RichTrackSegment.cpp,v 1.7 2008-02-16 11:45:58 jonrob Exp $
 *
 *  @author  Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @author  Antonis Papanestis
 *  @date    2002-06-10
 */
//-----------------------------------------------------------------------------

// local
#include "RichKernel/RichTrackSegment.h"

void LHCb::RichTrackSegment::updateState( const Gaudi::XYZPoint & rotPnt,
                                          const Gaudi::Transform3D & trans )
{
  // Entry point
  Gaudi::XYZVector toEntry = entryPoint() - rotPnt;
  setEntryState( rotPnt + trans(toEntry), trans(entryMomentum()) );

  // Middle point
  Gaudi::XYZVector toMid   = middlePoint() - rotPnt;
  setMiddleState( rotPnt + trans(toMid), trans(middleMomentum()) );

  // exit point
  Gaudi::XYZVector toExit  = exitPoint() - rotPnt;
  setExitState( rotPnt + trans(toExit), trans(exitMomentum()) );

  // reset
  reset();
}

void LHCb::RichTrackSegment::computeRotationMatrix2() const
{
  const Gaudi::XYZVector z = bestMomentum().Unit();
  Gaudi::XYZVector y = z.Cross( Gaudi::XYZVector(1,0,0) );
  y /= sqrt(y.Mag2()); // maybe not needed ?
  const Gaudi::XYZVector x = y.Cross(z);
  m_rotation2 = new Gaudi::Rotation3D( x.X(), y.X(), z.X(),
                                       x.Y(), y.Y(), z.Y(),
                                       x.Z(), y.Z(), z.Z() );
}

Gaudi::XYZPoint LHCb::RichTrackSegment::bestPoint( const double fractDist ) const
{
  if ( zCoordAt(fractDist) < middlePoint().z() )
  {
    const double midFrac1 =
      sqrt( (entryPoint()-middlePoint()).mag2() / (entryPoint()-exitPoint()).mag2() );
    return entryPoint() + (fractDist/midFrac1)*(middlePoint()-entryPoint());
  }
  else
  {
    const double midFrac2 =
      sqrt( (middlePoint()-exitPoint()).mag2() / (entryPoint()-exitPoint()).mag2() );
    return middlePoint() + ((fractDist-midFrac2)/midFrac2)*(exitPoint()-middlePoint());
  }
}

Gaudi::XYZVector LHCb::RichTrackSegment::bestMomentum( const double fractDist ) const
{
  if ( zCoordAt(fractDist) < middlePoint().z() )
  {
    const double midFrac =
      sqrt((entryPoint()-exitPoint()).mag2())*fractDist / sqrt((entryPoint()-middlePoint()).mag2());
    return entryMomentum()*(1-midFrac) + middleMomentum()*midFrac;
  }
  else
  {
    const double midFrac =
      sqrt((entryPoint()-exitPoint()).mag2())*fractDist/sqrt((middlePoint()-exitPoint()).mag2()) - 1;
    return middleMomentum()*(1-midFrac) + exitMomentum()*midFrac;
  }
}

void LHCb::RichTrackSegment::chordConstructorInit2()
{
  // the direction to use
  Gaudi::XYZVector v = exitPoint()-entryPoint();
  if ( v.Mag2() > 0 )
  {
    // Update direction of entry state to chord direction
    v *= sqrt( entryMomentum().Mag2() / v.Mag2() );
    setEntryState( entryPoint(), v );
    // Update direction of middle state to chord direction
    v *= sqrt( ((entryMomentum()+exitMomentum())/2).Mag2() / v.Mag2() );
    setMiddleState( add_points(entryPoint(),exitPoint())/2, v );
    // Update direction of exit state to chord direction
    v *= sqrt( exitMomentum().Mag2() / v.Mag2() );
    setExitState( exitPoint(), v );
  }
}

void LHCb::RichTrackSegment::chordConstructorInit3()
{
  // the direction to use
  Gaudi::XYZVector v = exitPoint()-entryPoint();
  if ( v.Mag2() > 0 )
  {
    // Update direction of entry state to chord direction
    v *= sqrt( entryMomentum().Mag2() / v.Mag2() );
    setEntryState( entryPoint(), v );
    // Update direction of middle state to chord direction
    v *= sqrt( middleMomentum().Mag2() / v.Mag2() );
    setMiddleState( middlePoint(), v );
    // Update direction of exit state to chord direction
    v *= sqrt( exitMomentum().Mag2() / v.Mag2() );
    setExitState( exitPoint(), v );
  }
}

std::ostream & LHCb::RichTrackSegment::fillStream ( std::ostream& s ) const
{
  s << "{ " << std::endl
    << " entryPoint:\t" << entryPoint() << std::endl
    << " middlePoint:\t" << middlePoint() << std::endl
    << " exitPoint:\t" << exitPoint() << std::endl
    << " entryMomentum:\t" << entryMomentum() << std::endl
    << " middleMomentum:\t" << middleMomentum() << std::endl
    << " exitMomentum:\t" << exitMomentum() << std::endl
    << " radiator:\t" << Rich::text( radiator() ) << std::endl
    << " rich:\t" << Rich::text( rich() ) << std::endl
    << " entryErrors:\t" << entryErrors() << std::endl
    << " middleErrors:\t" << middleErrors() << std::endl
    << " exitErrors:\t" << exitErrors() << std::endl
    << " } ";
  return s;
}
