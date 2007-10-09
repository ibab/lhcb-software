
#include "TsaKernel/Line3D.h"

namespace Tf
{
  namespace Tsa
  {

    Line3D createLine3D(const Line& xLine, const Line& yLine, const double zRef)
    {
      Gaudi::XYZVector direction =  Gaudi::XYZVector(xLine.m(), yLine.m(), 1.);
      direction = direction.Unit();
      Gaudi::XYZPoint point = Gaudi::XYZPoint(xLine.value(zRef),yLine.value(zRef),zRef);
      return Line3D(point,direction);
    }
    
  }
}
