// $Id: DetDescSvc_load.cpp,v 1.2 2005-12-07 17:15:50 cattanem Exp $
// Include files
#include "GaudiKernel/DeclareFactoryEntries.h"

// ============================================================================
/** @file 
 *  Declaration of all component factories from DetDescSvc package 
 *  @author M Needham
 */
// ============================================================================


DECLARE_FACTORY_ENTRIES(DetDescSvc) {
  
  // services  
  DECLARE_SERVICE(   TransportSvc            ) ;
  DECLARE_SERVICE(   UpdateManagerSvc        ) ;
  DECLARE_SERVICE(   EventClockSvc           ) ;
  
};
