
#include "RichModels.h"

namespace Lester
{
  static std::string toPrepend()
  {
    static const char * env = getenv("RICHMFINDERDATALOCATION");
    return ( env ? std::string(env) : "DATA" );
  }
  const Rich2AThetaSampler rich2AThetaSampler(toPrepend()+"/rich2_thetaDist.txt");
  const Rich2AThetaDistribtion rich2AThetaDistribution(toPrepend()+"/rich2_thetaDist.txt");
  const Rich2AProbabilityThetaAbove rich2AProbabilityThetaAbove(toPrepend()+"/rich2_thetaDist.txt");
};
