// $Id: CaloMoniSim_load.cpp,v 1.1.1.1 2004-10-25 08:53:25 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// Include files 
// ============================================================================
#include "GaudiKernel/DeclareFactoryEntries.h"
// ============================================================================

/** @file 
 *  Mandatory file for implementation of shared component library 
 *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
 *  @date 2004-10-25
 */

DECLARE_FACTORY_ENTRIES(CaloMoniSim) {
  
  DECLARE_ALGORITHM( MCCaloMonitor ) ;
  
};


// ============================================================================
// The END 
// ============================================================================
