// ============================================================================
#define GAUSSPHYSICS_GAUSSPHYSICS_LOAD_CPP 1 
// ============================================================================
// include 
// GaudiKernel 
#include "GaudiKernel/DeclareFactoryEntries.h" 
// GiGa 
#include "GiGa/GiGaMACROs.h"

/** @file 
 *  The mandatory file for declaration of component library entries 
 *  @author Witold Pokorsky Witold.Pokorsky@cern.ch 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 2002-09-26
 */

void GaussPhysics_load() 
{ 
  /// Physics Lists 
  DECLARE_GiGaFactory     (    GeneralPhysics          );
  DECLARE_GiGaFactory     (    EMPhysics               );
  DECLARE_GiGaFactory     (    EM_GNPhysics            );
  DECLARE_GiGaFactory     (    IonPhysics              );
  DECLARE_GiGaFactory     (    MuonPhysics             );
  DECLARE_GiGaFactory     (    HadronPhysicsLHEP       );
  DECLARE_GiGaFactory     (    HadronPhysicsLHEP_GN    );
  DECLARE_GiGaFactory     (    HadronPhysicsQGSP       );
};
// ============================================================================


// ============================================================================
extern "C" void GaussPhysics_loadRef() { GaussPhysics_load(); } ; 
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
