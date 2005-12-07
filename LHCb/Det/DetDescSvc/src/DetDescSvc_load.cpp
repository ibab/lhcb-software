// $Id: DetDescSvc_load.cpp,v 1.1.1.1 2005-12-07 16:01:45 mneedham Exp $
// Include files
#include "GaudiKernel/ObjectFactory.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

// ============================================================================
/** @file 
 *  Implementation of Data Object Factories and declaration of ALL 
 *  Factories from DetDescSvc package 
 *  @author M Needham
 *  (Borrowed from DetDesc version written by Vanya..
 *  @date xx/xx/xxxx 
 */
// ============================================================================


DECLARE_FACTORY_ENTRIES(DetDescSvc) {
  
  // services  
  DECLARE_SERVICE(   TransportSvc            ) ;
  
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
