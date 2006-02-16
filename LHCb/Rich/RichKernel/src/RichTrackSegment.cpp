
//-----------------------------------------------------------------------------
/** @file RichTrackSegment.cpp
 *
 *  Implementation file for class : RichTrackSegment
 *
 *  CVS Log :-
 *  $Id: RichTrackSegment.cpp,v 1.2 2006-02-16 15:54:33 jonrob Exp $
 *
 *  @author  Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @author  Antonis Papanestis
 *  @date    2002-06-10
 */
//-----------------------------------------------------------------------------

// local
#include "RichEvent/RichTrackSegment.h"

void LHCb::RichTrackSegment::updateState( const Gaudi::XYZPoint & rotPnt,
                                          const Gaudi::Transform3D & trans )
{
  // correct momentum directions
  m_entryMomentum  = trans( m_entryMomentum  );
  m_middleMomentum = trans( m_middleMomentum );
  m_exitMomentum   = trans( m_exitMomentum   );

  // Correct entry, exit and middle points
  Gaudi::XYZVector toEntry = entryPoint() - rotPnt;
  m_entryPoint             = rotPnt + trans(toEntry);
  Gaudi::XYZVector toMid   = middlePoint()  - rotPnt;
  m_middlePoint            = rotPnt + trans(toMid);
  Gaudi::XYZVector toExit  = exitPoint() - rotPnt;
  m_exitPoint              = rotPnt + trans(toExit);
  
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
