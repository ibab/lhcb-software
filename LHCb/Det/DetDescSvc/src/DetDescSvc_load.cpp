// $Id: DetDescSvc_load.cpp,v 1.4 2006-09-26 10:45:48 marcocle Exp $
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
  DECLARE_TOOL(      FakeEventTime           ) ;
  
};
