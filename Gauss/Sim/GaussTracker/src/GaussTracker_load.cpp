// ============================================================================
#define GAUSSTRACKER_GAUSSTRACKER_LOAD_CPP 1 
// ============================================================================
// include 
// GaudiKernel 
#include "GaudiKernel/DeclareFactoryEntries.h" 
// GiGa 
#include "GiGa/GiGaMACROs.h"

/** @file 
 *  The mandatory file for declaration of component library entries 
 *  @author Witold Pokorski Witold.Pokorski@cern.ch 
 *  @date 2002-09-26
 */

void GaussTracker_load() 
{ 
  /// Sensitive Detector 
  DECLARE_GiGaFactory     (    GiGaSensDetTracker      );
  DECLARE_GiGaFactory     (    GiGaSensDetVelo         );
  DECLARE_GiGaFactory     (    GiGaSensDetMuon         );

  /// Converters 
  DECLARE_CONVERTER       (    GiGaVeloHitCnv          );
  DECLARE_CONVERTER       (    GiGaTrackerHitCnv          );
  DECLARE_CONVERTER       (    GiGaMuonHitCnv          );
};
// ============================================================================


// ============================================================================
extern "C" void GaussTracker_loadRef() { GaussTracker_load(); } ; 
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
