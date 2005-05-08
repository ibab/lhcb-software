// $Id: CaloAssociators_load.cpp,v 1.11 2005-05-08 09:19:06 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.11 $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel 
// ============================================================================
#include "GaudiKernel/DeclareFactoryEntries.h"
// ============================================================================

/** @file
 * 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   08 Apr 2002 
 */

DECLARE_FACTORY_ENTRIES(CaloAssociators) 
{
  
  DECLARE_ALGORITHM   ( CaloDigitMCTruth         ) ;
  DECLARE_ALGORITHM   ( CaloClusterMCTruth       ) ;
  DECLARE_ALGORITHM   ( CaloTrgClusterMCTruth    ) ;
  DECLARE_ALGORITHM   ( CaloDigit2MCLinks2Table  ) ;
  
};

// ============================================================================
// The END 
// ============================================================================
