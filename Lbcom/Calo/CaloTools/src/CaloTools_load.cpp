// $Id: CaloTools_load.cpp,v 1.14 2002-07-21 18:56:18 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES(CaloTools) {
  
  DECLARE_TOOL( CaloClusterResetFractions    ) ;
  DECLARE_TOOL( ClusterCovarianceMatrixTool  ) ;
  DECLARE_TOOL( ClusterSpreadTool            ) ;

  DECLARE_TOOL( SubClusterSelectorAll        ) ;
  DECLARE_TOOL( SubClusterSelector3x3        ) ;
  DECLARE_TOOL( SubClusterSelectorMax2x2     ) ;
  DECLARE_TOOL( SubClusterSelectorSwissCross ) ;
  DECLARE_TOOL( SubClusterSelectorNeighbors  ) ;

  DECLARE_TOOL( CaloCorrASINH                ) ;
  DECLARE_TOOL( CaloCorr2TANH                ) ;
  
  DECLARE_TOOL( CaloSelector                 ) ;
  DECLARE_TOOL( CaloSelectorAND              ) ;
  DECLARE_TOOL( CaloSelectorOR               ) ;
  DECLARE_TOOL( CaloSelectorNOT              ) ;
   
  DECLARE_TOOL( CaloClusterToolSequence      ) ;
  DECLARE_TOOL( CaloHypoToolSequence         ) ;
  
  DECLARE_TOOL( CaloExtraDigits              ) ;

  DECLARE_TOOL( CaloSShape                   ) ;
  DECLARE_TOOL( CaloLCorrection              ) ;
  DECLARE_TOOL( CaloECorrection              ) ;
  DECLARE_TOOL( CaloE1Correction             ) ;

  DECLARE_TOOL( CaloPhotonMomentumTool       ) ;

  DECLARE_TOOL( CaloClusterSOMSelector       ) ;

  DECLARE_TOOL( CaloSingleGammaTool          ) ;
  
  DECLARE_TOOL( CaloLCorrectionSimple        ) ;

  DECLARE_TOOL( CaloSCorrectionSequence      ) ;
  DECLARE_TOOL( CaloSCorrectionCellCenter    ) ;
  DECLARE_TOOL( CaloSCorrectionBarycentre    ) ;
  DECLARE_TOOL( CaloSCorrectionArcSinh       ) ;
  DECLARE_TOOL( CaloSCorrectionLinear        ) ;
  DECLARE_TOOL( CaloSCorrectionDoubleTanh    ) ;
  DECLARE_TOOL( CaloSCorrectionComplex       ) ;


};
// ============================================================================

// ============================================================================
// The End 
// ============================================================================
