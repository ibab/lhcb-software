// $Id: CaloReco_load.cpp,v 1.21 2006-06-27 16:36:53 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.20  2006/06/14 16:49:22  odescham
// 2nd step of the migration towards DC06
//
// Revision 1.19  2006/05/30 09:42:03  odescham
// first release of the CaloReco migration
//
// Revision 1.18  2005/11/07 12:12:43  odescham
// v3r0 : adapt to the new Track Event Model
//
// Revision 1.17  2004/12/10 17:12:29  ibelyaev
//  steps towards 'Fast' Calo recontruction
//
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/DeclareFactoryEntries.h"
// ============================================================================

/** @file
 *  mandatory file for dynamic loading of CaloReco shared library.
 *  All 'loadable' components must be declard in this file 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 2002-10-31 
 */

DECLARE_FACTORY_ENTRIES(CaloReco) 
{

  DECLARE_ALGORITHM   ( CellularAutomaton                     ) ;
  DECLARE_ALGORITHM   ( CaloSharedCellAlg                     ) ;
  DECLARE_ALGORITHM   ( CaloClusterCorrect3x3Position         ) ;
  DECLARE_ALGORITHM   ( CaloClusterCovarianceAlg              ) ;

  DECLARE_ALGORITHM   ( CaloMergedPi0Alg                      ) ;
  DECLARE_ALGORITHM   ( CaloSinglePhotonAlg                   ) ;
  DECLARE_ALGORITHM   ( CaloElectronAlg                       ) ;
  DECLARE_ALGORITHM   ( CaloDigitsFilterAlg                   ) ;
  DECLARE_ALGORITHM   ( CaloHypoAlg                           ) ;

  
  DECLARE_TOOL        ( ClusterCovarianceMatrixTool           ) ;
  DECLARE_TOOL        ( SubClusterSelector3x3                 ) ;
  DECLARE_TOOL        ( ClusterSpreadTool                     ) ;
  DECLARE_TOOL        ( CaloExtraDigits                       ) ;

  DECLARE_TOOL        ( CaloECorrection                       ) ;
  DECLARE_TOOL        ( CaloSCorrection                       ) ;
  DECLARE_TOOL        ( CaloLCorrection                       ) ;

  DECLARE_TOOL        ( CaloSelector                          ) ;
  DECLARE_TOOL        ( CaloSelectorAND                       ) ;
  DECLARE_TOOL        ( CaloSelectorOR                        ) ;
  DECLARE_TOOL        ( CaloSelectorNOT                       ) ;

  DECLARE_TOOL        ( CaloSelectNeutralCluster              ) ;
};


