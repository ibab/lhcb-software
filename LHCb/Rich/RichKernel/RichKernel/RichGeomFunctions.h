
//-----------------------------------------------------------------------------
/** @file RichGeomFunctions.h
 *
 *  Header file for RICH geometrical utility functions : RichSide
 *
 *  CVS Log :-
 *  $Id: RichGeomFunctions.h,v 1.2 2006-04-18 15:04:03 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   20/01/2006
 */
//-----------------------------------------------------------------------------

#ifndef RICHKERNEL_RICHGEOMFUNCTIONS_H
#define RICHKERNEL_RICHGEOMFUNCTIONS_H 1

#include <cmath>

// Gaudi
#include "Kernel/Point3DTypes.h"
#include "Kernel/Vector3DTypes.h"

// General namespace for RICH specific definitions documented in RichSide.h
namespace Rich
{

  /** @enum Rich::Geom
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
     *  @param a Second vector
     *
     *  @return Angle between vectors (in rad)
     *  @retval Values 0 to two PI indicates success
     *  @retval -999 indicates failure
     */
    inline double AngleBetween( const Gaudi::XYZVector & a,
                                const Gaudi::XYZVector & b )
    {
      const double demon = sqrt( a.Mag2() * b.Mag2() );
      return ( demon>0 ? acos(a.Dot(b)/demon) : -999 );
    }

  }

}

#endif // RICHKERNEL_RICHGEOMFUNCTIONS_H
