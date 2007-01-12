// $Id: GiGa_load.cpp,v 1.16 2007-01-12 15:45:05 ranjard Exp $
// Include files 

// GaudiKernel 
#include "GaudiKernel/DeclareFactoryEntries.h" 
// GiGa 
//#include "GiGa/GiGaMACROs.h"

// ============================================================================
/** @file GiGa_load.cpp
 *  mandatory file for declaration of component library entries 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @author Gloria Corti
 *  @date 2007-01-08
 */
// ============================================================================

DECLARE_FACTORY_ENTRIES(GiGa) {

  /// GiGa Service itself
  DECLARE_SERVICE( GiGa );
 
  /// Run manager  (GiGa)
  DECLARE_TOOL( GiGaRunManager );

};









