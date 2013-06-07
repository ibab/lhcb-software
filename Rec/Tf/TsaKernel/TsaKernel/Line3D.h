#ifndef _TsaLine3D_H
#define _TsaLine3D_H

// from Kernel/LHCbDefinitions
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "LHCbMath/Line.h"

// local
#include "TsaKernel/Line.h"

namespace Tf
{
  namespace Tsa
  {

    /* @typedef for a line in 3-D
     *
     *  @author M.Needham
     *  @date   31/05/2004
     */
    typedef Gaudi::Math::Line<Gaudi::XYZPoint,Gaudi::XYZVector> Line3D;

    /// Create a Line3D from a point line and z reference point
    Line3D createLine3D(const Tsa::Line& xLine, const Tsa::Line& yLine, const double zRef);

  }
}



#endif
