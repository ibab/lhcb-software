// Include files 

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
    // Bs2MuMu development networks
    addBs2MuMuDev1();
    addBs2MuMuDev2();
  }

  TMVAImpFactory::~TMVAImpFactory( )
  {
    for ( auto& i : m_map ) { delete i.second; }
    m_map.clear();
  }

  // Method to get a static instance of the factory
  const TMVAImpFactory& tmvaFactory()
  {
    static TMVAImpFactory factory;
    return factory;
  }

}
