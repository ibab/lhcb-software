
#include "RectilinearCPQuantizer.h"

#include "NimTypeRichModel.h"

namespace Lester
{

  /// implement the one pure virtual method required by the base class:
  QuantizedCircleParams
  RectilinearCPQuantizer::quantize(const Lester::CircleParams & cp) const
  {
    return QuantizedCircleParams(static_cast<int>(sx * (cp.centre().x())),
                                 static_cast<int>(sy * (cp.centre().y())),
                                 static_cast<int>(sr * (cp.radius()    )));
  }

  RectilinearCPQuantizer::RectilinearCPQuantizer(const Lester::NimTypeRichModel & ntrm,
                                                 const double fractionOfMeanRadius) :
    sx(1./(fractionOfMeanRadius*ntrm.circleMeanRadiusParameter)),
    sy(1./(fractionOfMeanRadius*ntrm.circleMeanRadiusParameter)),
    sr(1./(fractionOfMeanRadius*ntrm.circleMeanRadiusParameter)),
    invBVol(sx*sy*sr) {
  }

}
