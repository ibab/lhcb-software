// $Id: Test.h,v 1.1.1.1 2006-10-09 15:43:44 jpalac Exp $
#ifndef DICT_TEST_H 
#define DICT_TEST_H 1
// ============================================================================
// DaVinciMCTools
// ============================================================================
#include "DaVinciMCTools/IBackgroundCategory.h"
#include "DaVinciMCTools/ICheatedLifetimeFitter.h"
#include "DaVinciMCTools/IDebugTool.h"
#include "DaVinciMCTools/IMCReconstructible.h"
#include "DaVinciMCTools/IVisPrimVertTool.h"
// MCEvent
#include "Kernel/IMCDecayFinder.h"
//
//#include "Linker/LinkerWithKey.h"
//#include "Event/MCParticle.h"

namespace {
  struct __Instantiations 
  {
    Object2FromMC<LHCb::Particle>                   partLink0;
    Object2FromMC<LHCb::ProtoParticle>              protoPartLink0;
    LinkerWithKey<LHCb::MCParticle,ContainedObject> link_inst0;

  };
  
}

#endif // DICT_TEST_H
