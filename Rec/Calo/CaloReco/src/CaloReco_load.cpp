// $Id: CaloReco_load.cpp,v 1.20 2006-06-14 16:49:22 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
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
  DECLARE_ALGORITHM   ( CaloClusterCovarianceAlg              ) ;
  DECLARE_ALGORITHM   ( CaloMergedPi0Alg                      ) ;
  DECLARE_ALGORITHM   ( CaloCluster2TrackAlg                  ) ;
  DECLARE_ALGORITHM   ( CaloHypoAlg                           ) ;
  DECLARE_ALGORITHM   ( CaloSinglePhotonAlg                   ) ;
  DECLARE_ALGORITHM   ( CaloElectronAlg                       ) ;
  DECLARE_ALGORITHM   ( CaloHypo2TrackAlg                     ) ;
  DECLARE_ALGORITHM   ( CaloDigitsFilterAlg                   ) ;
  DECLARE_ALGORITHM   ( CaloClusterCorrect3x3Position         ) ;
  
  DECLARE_TOOL        ( ClusterCovarianceMatrixTool           ) ;
  DECLARE_TOOL        ( SubClusterSelector3x3                 ) ;
  DECLARE_TOOL        ( ClusterSpreadTool                     ) ;

  DECLARE_TOOL        ( CaloECorrection                       ) ;
  DECLARE_TOOL        ( CaloSCorrection                       ) ;
  DECLARE_TOOL        ( CaloLCorrection                       ) ;

  DECLARE_TOOL        ( CaloSelectNeutralCluster              ) ;

  DECLARE_TOOL        ( CaloSelector                         ) ;
  DECLARE_TOOL        ( CaloSelectorAND                      ) ;
  DECLARE_TOOL        ( CaloSelectorOR                       ) ;
  DECLARE_TOOL        ( CaloSelectorNOT                      ) ;

  DECLARE_TOOL        ( CaloTrackMatchPhoton                ) ;
  DECLARE_TOOL        ( CaloTrackMatchElectron              ) ;
  DECLARE_TOOL        ( CaloTrackMatchBremm                 ) ;
  DECLARE_TOOL        ( CaloTrackMatchBrem2                 ) ;

  DECLARE_TOOL        ( CaloExtraDigits                     ) ;

  DECLARE_TOOL        ( CaloSingleGammaTool                 ) ;
  DECLARE_TOOL        ( CaloPhotonEstimatorTool             ) ;//temporary


  // BELOW ARE OBSOLETE TOOLS/ALGO REMOVED FROM CVS
  // some of these unused tools/algo. will be reimplemented latter, though.

  //DECLARE_TOOL        ( CaloClusterToolSequence             ) ;
  //DECLARE_TOOL        ( CaloHypoToolSequence                ) ;
  //DECLARE_ALGORITHM   ( CaloHyposMerge                      ) ;  
  //DECLARE_ALGORITHM   ( CaloClusterisationAlg               ) ;

  // From CaloRec package  
  //DECLARE_ALGORITHM   ( CaloCluster2TrgTrackAlg             ) ;
  //DECLARE_ALGORITHM   ( CaloHypo2TrgTrackAlg                ) ;  
  //DECLARE_TOOL        ( CaloTrgSelectNeutralCluster         ) ;

  // from CaloTools package 
  //DECLARE_TOOL        ( CaloClusterResetFractions           ) ;
  //DECLARE_TOOL        ( SubClusterSelectorAll               ) ;
  //DECLARE_TOOL        ( SubClusterSelectorMax2x2            ) ;
  //DECLARE_TOOL        ( SubClusterSelectorSwissCross        ) ;
  //DECLARE_TOOL        ( SubClusterSelectorNeighbors         ) ;

  //DECLARE_TOOL        ( CaloCorrASINH                       ) ;
  //DECLARE_TOOL        ( CaloCorr2TANH                       ) ;
  //DECLARE_TOOL        ( CaloSShape                          ) ;
  //DECLARE_TOOL        ( CaloLCorrection                     ) ;

  //DECLARE_TOOL        ( CaloPrsCorrection                   ) ;
  //DECLARE_TOOL        ( CaloRemoveMomentum                  ) ;
  //DECLARE_TOOL        ( CaloPhotonMomentumTool              ) ;
  //DECLARE_TOOL        ( CaloClusterSOMSelector              ) ;
  //DECLARE_TOOL        ( CaloLCorrectionSimple               ) ;
  
  //   DECLARE_TOOL        ( CaloSCorrectionSequence             ) ;
  //   DECLARE_TOOL        ( CaloSCorrectionCellCenter           ) ;
  //   DECLARE_TOOL        ( CaloSCorrectionBarycentre           ) ;
  //   DECLARE_TOOL        ( CaloSCorrectionArcSinh              ) ;
  //   DECLARE_TOOL        ( CaloSCorrectionLinear               ) ;
  //   DECLARE_TOOL        ( CaloSCorrectionDoubleTanh           ) ;
  //   DECLARE_TOOL        ( CaloSCorrectionComplex              ) ;
  //  DECLARE_TOOL        ( CaloSCorrectionFinal                ) ;


  //  DECLARE_ALGORITHM   ( CaloHypoReShuffleAlg                ) ;
  
  //   DECLARE_TOOL        ( CaloNewECorrection                  ) ;
  //   DECLARE_TOOL        ( CaloNewSCorrection                  ) ;
  //   DECLARE_TOOL        ( CaloNewLCorrection                  ) ;
  //   DECLARE_TOOL        ( Calo04ECorrection                   ) ;
  //   DECLARE_TOOL        ( Calo04SCorrection                   ) ;
  //   DECLARE_TOOL        ( Calo04LCorrection                   ) ;

  
};


// ============================================================================
// TheEND 
// ============================================================================
