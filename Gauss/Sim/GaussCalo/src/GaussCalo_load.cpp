// ============================================================================
#define GAUSSCALO_GAUSSCALO_LOAD_CPP 1 
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

void GaussCalo_load() 
{ 
  /// Sensitive Detector 
  DECLARE_GiGaFactory     (    CaloSensDet             );

  /// Converters 
  DECLARE_CONVERTER       (    GiGaCaloHitCnv          );  
};
// ============================================================================


// ============================================================================
extern "C" void GaussCalo_loadRef() { GaussCalo_load(); } ; 
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
