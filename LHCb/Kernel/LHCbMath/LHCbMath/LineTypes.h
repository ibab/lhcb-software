// $Id$
#ifndef LHCBMATH_LINETYPES_H 
#define LHCBMATH_LINETYPES_H 1

// Include files
#include "LHCbMath/Line.h"

/** @file
 *  
 *  Typedefs for 3D line types
 * 
 *  @author Juan PALACIOS
 *  @date   2005-11-21
 */
namespace Gaudi 
{

  namespace Math
  {
    
    // NB : Please remember to give a simple doxygen comment for each typedef

    typedef Line<XYZPoint, XYZVector>                 XYZLine;          ///< 3D cartesian line (double)
    typedef Line<Polar3DPoint, Polar3DVector>         Polar3DLine;      ///< 3D ploar line (double)
    typedef Line<RhoEtaPhiPoint, RhoEtaPhiVector>     RhoEtaPhiLine;    ///< 3D RhoEtaPhi line (double)
    typedef Line<RhoZPhiPoint, RhoZPhiVector>         RhoZPhiLine;      ///< 3D RhoZPhi line (double)

    typedef Line<XYZPointF, XYZVectorF>               XYZLineF;         ///< 3D cartesian line (float)
    typedef Line<Polar3DPointF, Polar3DVectorF>       Polar3DLineF;     ///< 3D ploar line (float)
    typedef Line<RhoEtaPhiPointF, RhoEtaPhiVectorF>   RhoEtaPhiLineF;   ///< 3D RhoEtaPhi line (float)
    typedef Line<RhoZPhiPointF, RhoZPhiVectorF>       RhoZPhiLineF;     ///< 3D RhoZPhi line (float)
    
  }
  
}

#endif // LHCBMATH_LINETYPES_H
