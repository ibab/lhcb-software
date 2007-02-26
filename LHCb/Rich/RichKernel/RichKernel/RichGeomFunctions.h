
//-----------------------------------------------------------------------------
/** @file RichGeomFunctions.h
 *
 *  Header file for RICH geometrical utility functions
 *
 *  CVS Log :-
 *  $Id: RichGeomFunctions.h,v 1.5 2007-02-26 13:52:01 cattanem Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   20/01/2006
 */
//-----------------------------------------------------------------------------

#ifndef RICHKERNEL_RICHGEOMFUNCTIONS_H
#define RICHKERNEL_RICHGEOMFUNCTIONS_H 1

#include <cmath>

// Gaudi
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"

namespace Rich
{

  /** @namespace Geom
   *
   *  Namespace for a collection of geomtrical utilities, such as useful functions 
   *  not available in MathMore
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   20/01/2006
   */
  namespace Geom
  {

    /** Calculates the angle between two vectors
     *
     *  @param a First vector
     *  @param b Second vector
     *
     *  @return Angle between vectors (in rad)
     */
    inline double AngleBetween( const Gaudi::XYZVector & a,
                                const Gaudi::XYZVector & b )
    {
      const double demon = sqrt( a.Mag2() * b.Mag2() );
      const double acosT = a.Dot(b);
      return ( demon>0 && acosT<1 ? acos(acosT/demon) : 0 );
    }

  }

}

#endif // RICHKERNEL_RICHGEOMFUNCTIONS_H
