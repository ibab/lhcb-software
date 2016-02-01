
// local
#include "TMVAImpFactory.h"

namespace ANNGlobalPID
{

  // Standard constructor
  TMVAImpFactory::TMVAImpFactory()
  {
    // Add the MC12TuneV2 implementations to the factory
    addMC12TuneV2();
    // Add the MC12TuneV3 implementations to the factory
    addMC12TuneV3();
    // Bs2MuMu developmental networks
    addBs2MuMuDev1();
    addBs2MuMuDev2();
  }

  // Method to get a static instance of the factory
  const TMVAImpFactory& tmvaFactory()
  {
    static TMVAImpFactory factory;
    return factory;
  }

}
