// $Id: CaloMoniDigi_load.cpp,v 1.1.1.1 2004-10-25 08:57:08 ibelyaev Exp $
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
 *  The mandatory file to build shared component library 
 *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
 *  @date 2004-10-25
 */

DECLARE_FACTORY_ENTRIES(CaloMoniDigi) {
  
  DECLARE_ALGORITHM( CaloDigitMonitor ) ;

};

// ============================================================================
// The END
// ============================================================================

