// $Id: DetDescSvc_load.cpp,v 1.5 2006-10-11 13:41:05 marcocle Exp $
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
  DECLARE_SERVICE(   DetElemFinder           ) ;

  // tools
  DECLARE_TOOL(      PreloadGeometryTool     ) ;
  DECLARE_TOOL(      FakeEventTime           ) ;
  
};
