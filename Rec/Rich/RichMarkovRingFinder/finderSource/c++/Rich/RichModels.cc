
#include "RichModels.h"

namespace Lester
{
  static std::string toPrepend()
  {
    const char * env = getenv("RICHMFINDERDATALOCATION");
    return ( env ? std::string(env) : "DATA" );
  }

  const Rich2AThetaDistribtion& rich2AThetaDistribution()
  {
    static Rich2AThetaDistribtion rich2AThetaDistribution(toPrepend()+"/rich2_thetaDist.txt");
    return rich2AThetaDistribution;
  }

  const Rich2AProbabilityThetaAbove& rich2AProbabilityThetaAbove()
  {
    static Rich2AProbabilityThetaAbove rich2AProbabilityThetaAbove(toPrepend()+"/rich2_thetaDist.txt");
    return rich2AProbabilityThetaAbove;
  }

  const Rich2AThetaSampler& rich2AThetaSampler()
  {
    static Rich2AThetaSampler rich2AThetaSampler(toPrepend()+"/rich2_thetaDist.txt");
    return rich2AThetaSampler;
  }

};
