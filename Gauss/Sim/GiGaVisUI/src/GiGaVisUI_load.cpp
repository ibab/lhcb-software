// ============================================================================
#define GIGAVISUI_GIGAVISUI_LOAD_CPP 1 
// ============================================================================
// include 
// GaudiKernel 
#include "GaudiKernel/DeclareFactoryEntries.h" 
// GiGa 
#include "GiGa/GiGaMACROs.h"

/** @file 
 *  The mandatory file for declaration of component library entries 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 2002-09-26
 */

void GiGaVisUI_load() 
{ 
  
  // UI & Vis 
  DECLARE_GiGaFactory     (    GiGaUIsession           );
  DECLARE_GiGaFactory     (    GiGaVisManager          );
  
};
// ============================================================================


// ============================================================================
extern "C" void GiGaVisUI_loadRef() { GiGaVisUI_load(); } ; 
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
