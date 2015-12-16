
//-----------------------------------------------------------------------------
/** @file RichTrackSegment.cpp
 *
 *  Implementation file for class : LHCb::RichTrackSegment
 *
 *  @author  Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @author  Antonis Papanestis
 *  @date    2002-06-10
 */
//-----------------------------------------------------------------------------

#ifdef __INTEL_COMPILER       // Disable ICC remark from ROOT
#pragma warning(disable:1572) // Floating-point comparisons are unreliable
#endif

// local
#include "RichKernel/RichTrackSegment.h"

// VDT
#include "vdt/sqrt.h"
#include "vdt/atan2.h"

// GSL
#include "gsl/gsl_math.h"

void
LHCb::RichTrackSegment::angleToDirection( const Gaudi::XYZVector & direction,
                                          double & theta,
                                          double & phi ) const
{
  // create vector in track reference frame
  const Gaudi::XYZVector rotDir ( rotationMatrix() * direction );

  // get the angles
  // phi   = rotDir.phi();
  // theta = rotDir.theta();

  // the above methods are :-
  // phi   : { return (fX==0 && fY==0) ? 0 : atan2(fY,fX);}
  // theta : { return (fX==0 && fY==0 && fZ==0) ? 0 : atan2(Rho(),Z());}
  // Rho   : { return std::sqrt( Perp2());}
  // Perp2 : { return fX*fX + fY*fY ;}

  // do it by hand, the same only faster ;)
  // Skip checks against 0 as we know that never happens here.
  phi   = vdt::fast_atan2( rotDir.y(), rotDir.x() );
  theta = vdt::fast_atan2( std::sqrt( gsl_pow_2(rotDir.x()) + 
                                      gsl_pow_2(rotDir.y()) ), 
                           rotDir.z() );
  
  // correct phi
  if ( phi < 0 ) phi += 2.0*M_PI;
}

void LHCb::RichTrackSegment::updateState( const Gaudi::XYZPoint & rotPnt,
                                          const Gaudi::Transform3D & trans )
{
  // Entry point
  auto toEntry = entryPoint() - rotPnt;
  setEntryState( rotPnt + trans(toEntry), trans(entryMomentum()) );

  // Middle point
  auto toMid   = middlePoint() - rotPnt;
  setMiddleState( rotPnt + trans(toMid), trans(middleMomentum()) );

  // exit point
  auto toExit  = exitPoint() - rotPnt;
  setExitState( rotPnt + trans(toExit), trans(exitMomentum()) );

  // reset
  reset();
}

void LHCb::RichTrackSegment::computeRotationMatrix2() const
{
  const auto z = bestMomentum().Unit();
  auto y = z.Cross( Gaudi::XYZVector(1,0,0) );
  y *= vdt::fast_isqrtf( y.Mag2() ); // maybe not needed ?
  const auto x = y.Cross(z);
  m_rotation2 = new Gaudi::Rotation3D( x.X(), y.X(), z.X(),
                                       x.Y(), y.Y(), z.Y(),
                                       x.Z(), y.Z(), z.Z() );
}

Gaudi::XYZPoint LHCb::RichTrackSegment::bestPoint( const double fractDist ) const
{
  const auto entryExitV ( entryPoint() - exitPoint() );
  if ( zCoordAt(fractDist) < middlePoint().z() )
  {
    const auto midEntryV ( middlePoint() - entryPoint());
    const auto invMidFrac1 = std::sqrt( entryExitV.mag2() / midEntryV.mag2() );
    return entryPoint() + (fractDist*invMidFrac1*midEntryV);
  }
  else
  {
    const auto exitMidV ( exitPoint() - middlePoint() );
    const auto midFrac2 = std::sqrt( exitMidV.mag2() / entryExitV.mag2() );
    return middlePoint() + (exitMidV*((fractDist-midFrac2)/midFrac2));
  }
}

Gaudi::XYZVector LHCb::RichTrackSegment::bestMomentum( const double fractDist ) const
{
  if ( zCoordAt(fractDist) < middlePoint().z() )
  {
    const auto midFrac =
      fractDist * std::sqrt( (entryPoint()-exitPoint()).mag2() / 
                             (entryPoint()-middlePoint()).mag2() );
    return entryMomentum()*(1-midFrac) + middleMomentum()*midFrac;
  }
  else
  {
    const auto midFrac =
      (fractDist * std::sqrt( (entryPoint()-exitPoint()).mag2() / 
                              (middlePoint()-exitPoint()).mag2() )) - 1;
    return middleMomentum()*(1-midFrac) + exitMomentum()*midFrac;
  }
}

void LHCb::RichTrackSegment::chordConstructorInit2()
{
  // the direction to use
  auto v = exitPoint()-entryPoint();
  if ( v.Mag2() > 0 )
  {
    // Update direction of entry state to chord direction
    v *= std::sqrt( entryMomentum().Mag2() / v.Mag2() );
    setEntryState( entryPoint(), v );
    // Update direction of middle state to chord direction
    v *= std::sqrt( ( (entryMomentum()+exitMomentum())*0.5 ).Mag2() / v.Mag2() );
    setMiddleState( add_points(entryPoint(),exitPoint())*0.5, v );
    // Update direction of exit state to chord direction
    v *= std::sqrt( exitMomentum().Mag2() / v.Mag2() );
    setExitState( exitPoint(), v );
  }
}

void LHCb::RichTrackSegment::chordConstructorInit3()
{
  // the direction to use
  auto v = exitPoint()-entryPoint();
  if ( v.Mag2() > 0 )
  {
    // Update direction of entry state to chord direction
    v *= std::sqrt( entryMomentum().Mag2() / v.Mag2() );
    setEntryState( entryPoint(), v );
    // Update direction of middle state to chord direction
    v *= std::sqrt( middleMomentum().Mag2() / v.Mag2() );
    setMiddleState( middlePoint(), v );
    // Update direction of exit state to chord direction
    v *= std::sqrt( exitMomentum().Mag2() / v.Mag2() );
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
