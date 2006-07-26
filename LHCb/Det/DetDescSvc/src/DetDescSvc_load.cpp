// $Id: DetDescSvc_load.cpp,v 1.3 2006-07-26 09:46:58 cattanem Exp $
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

  // tools
  DECLARE_TOOL(      PreloadGeometryTool     ) ;
  
};
