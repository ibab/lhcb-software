// ============================================================================
#define GAUSSTOOLS_GAUSSTOOLS_LOAD_CPP 1 
// ============================================================================
// include 
// GaudiKernel 
#include "GaudiKernel/DeclareFactoryEntries.h" 
// GiGa 
#include "GiGa/GiGaMACROs.h"

void GaussTools_load() 
{ 
  /// Sensitive Detector 
  DECLARE_GiGaFactory     (    GiGaSensDetTracker      );
  DECLARE_GiGaFactory     (    GiGaSensDetVelo         );
  DECLARE_GiGaFactory     (    GiGaSensDetMuon         );
  /// Algorithms 
  DECLARE_ALGORITHM       (    GetGiGaHitsAlg           );
};

// ============================================================================
// ============================================================================
extern "C" void GaussTools_loadRef() { GaussTools_load(); } ; 

