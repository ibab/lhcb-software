// $Id: CaloTools_load.cpp,v 1.7 2002-04-27 19:21:30 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES(CaloTools) {
  
  DECLARE_TOOL( ClusterCovarianceMatrixTool  );
  DECLARE_TOOL( ClusterSpreadTool            );
  DECLARE_TOOL( SubClusterSelectorAll        );
  DECLARE_TOOL( SubClusterSelector3x3        );
  DECLARE_TOOL( SubClusterSelectorMax2x2     );
  DECLARE_TOOL( SubClusterSelectorSwissCross );
  DECLARE_TOOL( SubClusterSelectorNeighbors  );

  DECLARE_TOOL( CaloCorrASINH                );
  DECLARE_TOOL( CaloCorr2TANH                );
  
  DECLARE_TOOL( CaloSelector                 );
  DECLARE_TOOL( CaloSelectorAND              );
  DECLARE_TOOL( CaloSelectorOR               );
  DECLARE_TOOL( CaloSelectorNOT              );
  
  DECLARE_TOOL( CaloClusterToolSequence      );
  DECLARE_TOOL( CaloHypoToolSequence         );
  
};

// ============================================================================
// The End 
// ============================================================================
