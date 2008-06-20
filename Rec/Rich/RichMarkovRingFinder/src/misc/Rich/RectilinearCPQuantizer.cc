
#include "RectilinearCPQuantizer.h"

namespace Lester
{

  QuantizedCircleParams
  RectilinearCPQuantizer::quantize(const Lester::CircleParams & cp) const
  {
    return QuantizedCircleParams( static_cast<int>(sx * (cp.centre().x())),
                                  static_cast<int>(sy * (cp.centre().y())),
                                  static_cast<int>(sr * (cp.radius()    )) );
  }

}
