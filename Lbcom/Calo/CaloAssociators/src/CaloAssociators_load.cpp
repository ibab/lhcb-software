// $Id: CaloAssociators_load.cpp,v 1.10 2004-07-29 16:51:52 ibelyaev Exp $
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel 
// ============================================================================
#include "GaudiKernel/DeclareFactoryEntries.h"
// ============================================================================

/** @file CaloAssociators_load.cpp 
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   08 Apr 2002 
 */

DECLARE_FACTORY_ENTRIES(CaloAssociators) 
{
  
  DECLARE_ALGORITHM   ( CaloClustersMCTruthAlg             ) ;
  DECLARE_ALGORITHM   ( CaloClustersMCTruth2Alg            ) ;
  DECLARE_ALGORITHM   ( CaloClustersMCTruth3Alg            ) ;
  DECLARE_ALGORITHM   ( CaloClustersMCTruth4Alg            ) ;
  DECLARE_ALGORITHM   ( CaloClustersMCTruth5Alg            ) ;
  
};

// ============================================================================
// The END 
// ============================================================================
