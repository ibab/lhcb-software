// $Id: DetDescChecks_load.cpp,v 1.1.1.1 2002-05-26 12:47:06 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// Include files
#include "GaudiKernel/DeclareFactoryEntries.h"

// ============================================================================
/** @file 
 *  Mandatory file for loading of factories 
 *  @date 2002-05-25 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 */
// ============================================================================

DECLARE_FACTORY_ENTRIES(DetDescChecks) 
{
  
  DECLARE_ALGORITHM( MaterialBudgetAlg ) ;
  DECLARE_ALGORITHM( VolumeCheckAlg    ) ;
  
};

// ============================================================================
// The END 
// ============================================================================


